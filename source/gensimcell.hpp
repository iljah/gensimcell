/*
Generic simulation cell class.

Copyright 2013, 2014 Ilja Honkonen
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

* Neither the name of copyright holders nor the names of their contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef GENSIMCELL_HPP
#define GENSIMCELL_HPP


#include "operators.hpp"
#include "type_support.hpp"
#include "gensimcell_impl.hpp"
#include "gensimcell_transfer_policy.hpp"


/*!
\mainpage Generic simulation cell class.

See gensimcell::Cell to get started with the API.
*/


/*!
Namespace where everything related to
generic simulation cell class is defined.
*/
namespace gensimcell {


/*!
Generic simulation cell that stores arbitrary simulation variables.

Transfer policy decides how generic cells of that type will transfer
variable data between processes. gensimcell::Optional_Transfer allows
to switch on and off transfer of variable data in individual cells.
gensimcell::Always_Transfer makes cells of that type always transfer
all of their variables' data. gensimcell::Never_Transfer never
transfers any variables' data between processes. The latter two
policies can be used to create cells with smaller memory footprint,
cells using gensimcell::Optional_Transfer as a transfer policy
store in memory one boolean before or after each variables' data.
See below for details on switching transfers on and off.

Simulation variables are classes given as template arguments.
Each variables must define its type as data_type which will
be the type stored in each instance of the cell class.
For example in Conway's Game of Life (GoL) the following variables
could be used:
@code
struct Is_Alive { using data_type = bool;};
struct Live_Neighbors { using data_type = int;};
@endcode
An instance of a GoL cell is created by:
@code
gensimcell::Cell<Is_Alive, Live_Neighbors> cell;
@endcode
And a grid of such cells can be created by:
@code
using Cell_T = gensimcell::Cell<Is_Alive, Live_Neighbors>;
std::array<std::array<Cell_T, 10>, 10> grid;
@endcode
Access to the data of each variable is provided by the []
operator overloaded for the class type:
@code
cell[Is_Alive()] = false;
cell[Live_Neighbors()] = 3;
@endcode
The life state of all cells in the previous grid can be printed by:
@code
for (const auto& row: grid) {
	for (const auto& cell: row) {
		cout << cell[Is_Alive()] << " ";
	}
	cout << endl;
}
@endcode
For complete examples see the following files in the git repository:
examples/game_of_life/serial.cpp
examples/advection/serial.cpp
examples/particle_propagation/serial.cpp

Distributed memory concurrency support is provided using the
Message Passing Interface (MPI) application programming interface
via the get_mpi_datatype() member function. Each variable stored
in the cell can be included in the transfer information returned
by get_mpi_datatype(). Whether a variable is included or not can
be switched with the set_transfer() and set_transfer_all()
methods. The latter switches the transfer of variables given to
the method on or off in all instances of that cell, the former
affects only the current cell. When the value set by
set_transfer_all() is equal to true of false, all cells
will behave identically regardless of the value set by
set_transfer(). To get individual cell behavior for a variable
set the transfer info using set_transfer_all() to an
undeterminate value for the specific variables.
For complete examples see the files in the following directories
in the git repository:
examples/game_of_life/parallel/
examples/advection/parallel/
examples/particle_propagation/parallel/
examples/combined/

For the full API see the comments in the
source/gensimcell_impl.hpp file.
*/
template <
	template<class> class Transfer_Policy,
	class... Variables
> class Cell :
	public detail::Cell_impl<Transfer_Policy, sizeof...(Variables), Variables...>
{
public:
	/*!
	Allows the cell class to be stored as a variable in another cell class.

	For example:
	@code
	struct GoL_Variables1 {
		using data_type = gensimcell:Cell<Is_Alive, Live_Neighbors>;
	};
	struct GoL_Variables2 {
		using data_type = gensimcell:Cell<Is_Alive, Live_Neighbors>;
	};
	gensimcell::Cell<GoL_Variables1, GoL_Variables2> cell1, cell2;

	cell1[GoL_Variables1()][Is_Alive()] =
	cell1[GoL_Variables2()][Is_Alive()] =
	cell2[GoL_Variables1()][Is_Alive()] =
	cell2[GoL_Variables2()][Is_Alive()] = true;
	@endcode
	*/
	using data_type = detail::Cell_impl<
		Transfer_Policy,
		sizeof...(Variables),
		Variables...
	>;


	#if defined(MPI_VERSION) && (MPI_VERSION >= 2)

	/*!
	Returns the MPI transfer info of this cell's variables.

	Each variable that has been set to be transferred with
	set_transfer_all() or set_transfer() will be added to the
	final datatype returned by this function. See the MPI API
	for details on the returned address, count and MPI_Datatype.
	*/
	std::tuple<
		void*,
		int,
		MPI_Datatype
	> get_mpi_datatype() const
	{
		return detail::Cell_impl<
			Transfer_Policy,
			sizeof...(Variables),
			Variables...
		>::get_mpi_datatype();
	}

	#endif // ifdef MPI_VERSION
};


namespace detail {

//! get_last::type is equal to last given template parameter
template<class... T> struct get_last;

template<class T, class... U> struct get_last<T, U...> {
	using type = typename get_last<U...>::type;
};

template<class T> struct get_last<T> {
	using type = T;
};

} // namespace detail


/*!
Returns a (const) reference to data stored recursively in Variables.

Helper function for accessing variables nested in several layers
of generic simulation cells.

Calls [] operator of given cell with first variable, calls []
of type returned by first [] with second variable, etc. The call
get(c, v1, v2, v3) is equal to c[v1][v2][v3].

Example:
@code
struct var1 {using data_type = int};
struct var2 {using data_type = gensimcell::Cell<..., var1>};
gensimcell::Cell<..., var2> cell;
gensimcell::get(cell, var2(), var1()) = 3;
@endcode
*/
template<class Cell, class... Variables> void get(
	const Cell&,
	const Variables&...
) {}


//! Starts/continues recursion over variables
template<
	class Cell_T,
	class First_Variable,
	class... Rest_Of_Variables
> const typename detail::get_last<
	First_Variable,
	Rest_Of_Variables...
>::type::data_type& get(
	const Cell_T& cell,
	const First_Variable& first_variable,
	const Rest_Of_Variables&... rest_of_variables
) {
	return get(cell[first_variable], rest_of_variables...);
}

//! Non-const version of recursion over variables
template<
	class Cell_T,
	class First_Variable,
	class... Rest_Of_Variables
> typename detail::get_last<
	First_Variable,
	Rest_Of_Variables...
>::type::data_type& get(
	Cell_T& cell,
	const First_Variable& first_variable,
	const Rest_Of_Variables&... rest_of_variables
) {
	return get(cell[first_variable], rest_of_variables...);
}


//! Stops recursion over variables
template<
	class Cell_T,
	class Last_Variable
> const typename Last_Variable::data_type& get(
	const Cell_T& cell,
	const Last_Variable& variable
) {
	return cell[variable];
}

//! Non-const version of stopping recursion
template<
	class Cell_T,
	class Last_Variable
> typename Last_Variable::data_type& get(
	Cell_T& cell,
	const Last_Variable& variable
) {
	return cell[variable];
}


} // namespace gensimcell


#endif // ifndef GENSIMCELL_HPP
