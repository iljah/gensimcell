/*
Implementation of the generic simulation cell class.

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

#ifndef GENSIMCELL_IMPL_HPP
#define GENSIMCELL_IMPL_HPP


#include "cstdlib"
#include "limits"
#include "vector"


#if defined(MPI_VERSION) && (MPI_VERSION >= 2)

#include "boost/logic/tribool.hpp"

#endif // ifdef MPI_VERSION


#include "get_var_mpi_datatype.hpp"


namespace gensimcell {
namespace detail {



/*!
Generic version of the implementation that doesn't do anything.

See the version that stops the iteration
over Variables for documentation.
*/
template <
	size_t number_of_variables,
	class... Variables
> class Cell_impl {};



/*!
Starts or continues the iteration over variables.

See the version that stops the iteration for documentation.
*/
template <
	size_t number_of_variables,
	class Current_Variable,
	class... Rest_Of_Variables
> class Cell_impl<
	number_of_variables,
	Current_Variable,
	Rest_Of_Variables...
> :
	public Cell_impl<number_of_variables, Rest_Of_Variables...>
{

private:


	typename Current_Variable::data_type data;


	#if defined(MPI_VERSION) && (MPI_VERSION >= 2)

	/*!
	Whether each instance tranfers
	this variable or not using MPI
	*/
	static boost::logic::tribool transfer_all;
	/*!
	Whether this instance sends this variable
	if transfer_all is indeterminite
	*/
	bool transfer = false;

	#endif // ifdef MPI_VERSION


protected:


	#if defined(MPI_VERSION) && (MPI_VERSION >= 2)

	static void set_transfer_all_impl(
		const boost::logic::tribool given_transfer,
		const Current_Variable&
	) {
		transfer_all = given_transfer;
	}


	void set_transfer_impl(
		const bool given_transfer,
		const Current_Variable&
	) {
		this->transfer = given_transfer;
	}


	size_t get_mpi_datatype_impl(
		size_t index,
		std::array<void*, number_of_variables>& addresses,
		std::array<int, number_of_variables>& counts,
		std::array<MPI_Datatype, number_of_variables>& datatypes
	) const {

		size_t nr_transferred = 0;
		if (this->is_transferred(Current_Variable())) {
			std::tie(
				addresses[index],
				counts[index],
				datatypes[index]
			) = get_var_mpi_datatype(this->data);
			index++;
			nr_transferred++;
		}

		/*
		Make the order of variables in the final data type
		the same as in the template arguments list to the cell
		*/
		nr_transferred
			+= Cell_impl<
				number_of_variables,
				Rest_Of_Variables...
			>::get_mpi_datatype_impl(
				index,
				addresses,
				counts,
				datatypes
			);

		return nr_transferred;
	}

	#endif // ifdef MPI_VERSION



public:


	/*
	Make all public functions of the inherited implementation(s)
	available also through the current iteration over user's variables.
	*/
	using Cell_impl<number_of_variables, Rest_Of_Variables...>::operator[];


	typename Current_Variable::data_type& operator[](const Current_Variable&)
	{
		return this->data;
	};

	const typename Current_Variable::data_type& operator[](const Current_Variable&) const
	{
		return this->data;
	};


	#if defined(MPI_VERSION) && (MPI_VERSION >= 2)

	using Cell_impl<number_of_variables, Rest_Of_Variables...>::set_transfer_all_impl;
	using Cell_impl<number_of_variables, Rest_Of_Variables...>::get_transfer_all;
	using Cell_impl<number_of_variables, Rest_Of_Variables...>::set_transfer_impl;
	using Cell_impl<number_of_variables, Rest_Of_Variables...>::get_transfer;
	using Cell_impl<number_of_variables, Rest_Of_Variables...>::is_transferred;


	template<class... Given_Vars> static void set_transfer_all(
		const boost::logic::tribool given_transfer,
		const Given_Vars&...
	);

	template<
		class First_Given_Var,
		class... Rest_Given_Vars
	> static void set_transfer_all(
		const boost::logic::tribool given_transfer,
		const First_Given_Var& first,
		const Rest_Given_Vars&... rest
	) {
		set_transfer_all_impl(given_transfer, first);
		set_transfer_all(given_transfer, rest...);
	}

	template<class Given_Var> static void set_transfer_all(
		const boost::logic::tribool given_transfer,
		const Given_Var& var
	) {
		set_transfer_all_impl(given_transfer, var);
	}


	static boost::logic::tribool get_transfer_all(const Current_Variable&)
	{
		return transfer_all;
	}


	template<class... Given_Vars> void set_transfer(
		const bool given_transfer,
		const Given_Vars&...
	);

	template<
		class First_Given_Var,
		class... Rest_Given_Vars
	> void set_transfer(
		const bool given_transfer,
		const First_Given_Var& first,
		const Rest_Given_Vars&... rest
	) {
		this->set_transfer_impl(given_transfer, first);
		this->set_transfer(given_transfer, rest...);
	}

	template<class Given_Var> void set_transfer(
		const bool given_transfer,
		const Given_Var& var
	) {
		this->set_transfer_impl(given_transfer, var);
	}


	bool get_transfer(const Current_Variable&) const
	{
		return this->transfer;
	}


	bool is_transferred(const Current_Variable&) const
	{
		if (transfer_all) {
			return true;
		} else if (not transfer_all) {
			return false;
		} else {
			if (this->transfer) {
				return true;
			} else {
				return false;
			}
		}
	}


	std::tuple<
		void*,
		int,
		MPI_Datatype
	> get_mpi_datatype() const
	{
		std::array<void*, number_of_variables> addresses;
		std::array<int, number_of_variables> counts;
		std::array<MPI_Datatype, number_of_variables> datatypes;

		const size_t nr_vars_to_transfer
			= this->get_mpi_datatype_impl(
				0,
				addresses,
				counts,
				datatypes
			);

		if (nr_vars_to_transfer == 0) {

			// assume NULL won't be dereferenced if count = 0
			return std::make_tuple((void*) NULL, 0, MPI_BYTE);

		} else if (nr_vars_to_transfer == 1) {

			return std::make_tuple(
				addresses[0],
				counts[0],
				datatypes[0]
			);

		} else if (nr_vars_to_transfer <= std::numeric_limits<int>::max()) {

			// get displacements of variables to transfer
			std::array<MPI_Aint, number_of_variables> displacements;
			for (size_t i = 0; i < nr_vars_to_transfer; i++) {
				displacements[i]
					= static_cast<char*>(addresses[i])
					- static_cast<char*>(addresses[0]);
			}

			MPI_Datatype final_datatype = MPI_DATATYPE_NULL;
			if (
				MPI_Type_create_struct(
					int(nr_vars_to_transfer),
					counts.data(),
					displacements.data(),
					datatypes.data(),
					&final_datatype
				) != MPI_SUCCESS
			) {
				return std::make_tuple((void*) NULL, -1, MPI_DATATYPE_NULL);
			}

			return std::make_tuple(addresses[0], 1, final_datatype);

		} else {

			return std::make_tuple(
				(void*) NULL,
				std::numeric_limits<int>::lowest(),
				MPI_DATATYPE_NULL
			);

		}
	}

	#endif // ifdef MPI_VERSION
};

#if defined(MPI_VERSION) && (MPI_VERSION >= 2)

template <
	size_t number_of_variables,
	class Current_Variable,
	class... Rest_Of_Variables
> boost::logic::tribool Cell_impl<
	number_of_variables,
	Current_Variable,
	Rest_Of_Variables...
>::transfer_all = false;

#endif // ifdef MPI_VERSION


/*!
Stops the iteration over variables given by the user.

Operator () provides access to the user's data.
*/
template <
	size_t number_of_variables,
	class Variable
> class Cell_impl<
	number_of_variables,
	Variable
> {


private:


	typename Variable::data_type data;


	#if defined(MPI_VERSION) && (MPI_VERSION >= 2)

	/*!
	Whether each instance tranfers
	this variable or not using MPI
	*/
	static boost::logic::tribool transfer_all;
	/*!
	Whether this instance sends this variable
	if transfer_all is indeterminite
	*/
	bool transfer = false;

	#endif // ifdef MPI_VERSION



protected:


	#if defined(MPI_VERSION) && (MPI_VERSION >= 2)

	static void set_transfer_all_impl(
		const boost::logic::tribool given_transfer,
		const Variable&
	) {
		transfer_all = given_transfer;
	}


	void set_transfer_impl(const bool given_transfer, const Variable&)
	{
		this->transfer = given_transfer;
	}


	size_t get_mpi_datatype_impl(
		const size_t index,
		std::array<void*, number_of_variables>& addresses,
		std::array<int, number_of_variables>& counts,
		std::array<MPI_Datatype, number_of_variables>& datatypes
	) const {

		if (this->is_transferred(Variable())) {
			std::tie(
				addresses[index],
				counts[index],
				datatypes[index]
			) = get_var_mpi_datatype(this->data);

			return 1;
		}

		return 0;
	}

	#endif // ifdef MPI_VERSION


public:


	typename Variable::data_type& operator[](const Variable&)
	{
		return this->data;
	}

	const typename Variable::data_type& operator[](const Variable&) const
	{
		return this->data;
	}


	#if defined(MPI_VERSION) && (MPI_VERSION >= 2)

	template<class... Given_Vars> static void set_transfer_all(
		const boost::logic::tribool given_transfer,
		const Given_Vars&...
	);

	template<
		class First_Given_Var,
		class... Rest_Given_Vars
	> static void set_transfer_all(
		const boost::logic::tribool given_transfer,
		const First_Given_Var& first,
		const Rest_Given_Vars&... rest
	) {
		set_transfer_all_impl(given_transfer, first);
		set_transfer_all(given_transfer, rest...);
	}

	template<class Given_Var> static void set_transfer_all(
		const boost::logic::tribool given_transfer,
		const Given_Var& var
	) {
		set_transfer_all_impl(given_transfer, var);
	}


	static boost::logic::tribool get_transfer_all(const Variable&)
	{
		return transfer_all;
	}


	/*!
	Note that this information is not transferred over MPI
	if the datatype is obtained from get_mpi_datatype().
	*/
	template<class... Given_Vars> void set_transfer(
		const bool given_transfer,
		const Given_Vars&...
	);

	template<
		class First_Given_Var,
		class... Rest_Given_Vars
	> void set_transfer(
		const bool given_transfer,
		const First_Given_Var& first,
		const Rest_Given_Vars&... rest
	) {
		this->set_transfer(given_transfer, rest...);
		this->set_transfer_impl(given_transfer, first);
	}

	template<class Given_Var> void set_transfer(
		const bool given_transfer,
		const Given_Var& var
	) {
		this->set_transfer_impl(given_transfer, var);
	}


	bool get_transfer(const Variable&) const
	{
		return this->transfer;
	}


	bool is_transferred(const Variable&) const
	{
		if (transfer_all) {
			return true;
		} else if (not transfer_all) {
			return false;
		} else {
			if (this->transfer) {
				return true;
			} else {
				return false;
			}
		}
	}


	std::tuple<
		void*,
		int,
		MPI_Datatype
	> get_mpi_datatype() const
	{
		std::array<void*, number_of_variables> addresses;
		std::array<int, number_of_variables> counts;
		std::array<MPI_Datatype, number_of_variables> datatypes;

		const size_t nr_vars_to_transfer
			= this->get_mpi_datatype_impl(
				0,
				addresses,
				counts,
				datatypes
			);

		if (nr_vars_to_transfer == 0) {
			addresses[0] = NULL;
			counts[0] = 0;
			datatypes[0] = MPI_BYTE;
		}

		return std::make_tuple(
			addresses[0],
			counts[0],
			datatypes[0]
		);
	}

	#endif // ifdef MPI_VERSION
};


#if defined(MPI_VERSION) && (MPI_VERSION >= 2)

template <
	size_t number_of_variables,
	class Variable
> boost::logic::tribool Cell_impl<
	number_of_variables,
	Variable
>::transfer_all = false;

#endif // ifdef MPI_VERSION



} // namespace detail
} // namespace gensimcell


#endif // ifndef GENSIMCELL_IMPL_HPP
