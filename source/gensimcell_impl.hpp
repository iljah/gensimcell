/*
Implementation of the generic simulation cell class.

Copyright 2013, 2014, 2015 Ilja Honkonen
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


#include "array"
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
	template<class> class Transfer_Policy,
	size_t number_of_variables,
	class... Variables
> class Cell_impl {};


/*!
Starts or continues the iteration over variables.

See the version that stops the iteration for documentation.
*/
template <
	template<class> class Transfer_Policy,
	size_t number_of_variables,
	class Current_Variable,
	class... Rest_Of_Variables
> class Cell_impl<
	Transfer_Policy,
	number_of_variables,
	Current_Variable,
	Rest_Of_Variables...
> :
	public Cell_impl<Transfer_Policy, number_of_variables, Rest_Of_Variables...>,
	public Transfer_Policy<Current_Variable>
{

private:

	typename Current_Variable::data_type data;


protected:


	#if defined(MPI_VERSION) && (MPI_VERSION >= 2)

	using Transfer_Policy<Current_Variable>::set_transfer_all_impl;
	using Transfer_Policy<Current_Variable>::set_transfer_impl;

	using Cell_impl<
		Transfer_Policy,
		number_of_variables,
		Rest_Of_Variables...
	>::set_transfer_all_impl;

	using Cell_impl<
		Transfer_Policy,
		number_of_variables,
		Rest_Of_Variables...
	>::set_transfer_impl;

	using Cell_impl<
		Transfer_Policy,
		number_of_variables,
		Rest_Of_Variables...
	>::get_mpi_datatype_impl;


	/*!
	Fill given arrays at given index with current
	variable's MPI transfer info.
	*/
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
				Transfer_Policy,
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

	#endif // if defined MPI...


	#define GENSIMCELL_COMMA ,
	#define GENSIMCELL_MAKE_OPERATOR_IMPLEMENTATION(NAME, OPERATOR) \
	using Cell_impl< \
		Transfer_Policy GENSIMCELL_COMMA \
		number_of_variables GENSIMCELL_COMMA \
		Rest_Of_Variables... \
	>::NAME; \
	\
	template<class Other_T> void NAME( \
		const Current_Variable& GENSIMCELL_COMMA \
		const Other_T& rhs \
	) { \
		this->data OPERATOR rhs; \
	}

	GENSIMCELL_MAKE_OPERATOR_IMPLEMENTATION(plus_equal_impl, +=)
	GENSIMCELL_MAKE_OPERATOR_IMPLEMENTATION(minus_equal_impl, -=)
	GENSIMCELL_MAKE_OPERATOR_IMPLEMENTATION(mul_equal_impl, *=)
	GENSIMCELL_MAKE_OPERATOR_IMPLEMENTATION(div_equal_impl, /=)
	#undef GENSIMCELL_MAKE_OPERATOR_IMPLEMENTATION


public:


	/*!
	Make all public functions of the inherited implementation(s)
	available also through the current iteration over user's variables.
	*/
	using Cell_impl<
		Transfer_Policy,
		number_of_variables,
		Rest_Of_Variables...
	>::operator[];


	//! Returns a reference to the data of given variable.
	typename Current_Variable::data_type& operator[](const Current_Variable&)
	{
		return this->data;
	};

	//! Returns a const reference to the data of given variable.
	const typename Current_Variable::data_type& operator[](const Current_Variable&) const
	{
		return this->data;
	};


	/*
	Operators
	*/

	#define GENSIMCELL_MAKE_OPERATOR(NAME, IMPLEMENTATION_NAME, OPERATOR) \
	template< \
		class... Operator_Variables \
	> void NAME( \
		const Cell_impl< \
			Transfer_Policy GENSIMCELL_COMMA \
			number_of_variables GENSIMCELL_COMMA \
			Current_Variable GENSIMCELL_COMMA \
			Rest_Of_Variables... \
		>& GENSIMCELL_COMMA \
		const Operator_Variables&... \
	) {} \
	\
	template< \
		class First_Op_Var GENSIMCELL_COMMA \
		class... Rest_Op_Vars \
	> void NAME( \
		const Cell_impl< \
			Transfer_Policy GENSIMCELL_COMMA \
			number_of_variables GENSIMCELL_COMMA \
			Current_Variable GENSIMCELL_COMMA \
			Rest_Of_Variables... \
		>& rhs GENSIMCELL_COMMA \
		const First_Op_Var& first_op_var GENSIMCELL_COMMA \
		const Rest_Op_Vars&... rest_op_vars \
	) { \
		this->IMPLEMENTATION_NAME(first_op_var GENSIMCELL_COMMA rhs[first_op_var]); \
		this->NAME(rhs GENSIMCELL_COMMA rest_op_vars...); \
	} \
	\
	template< \
		class Last_Op_Var \
	> void NAME( \
		const Cell_impl< \
			Transfer_Policy GENSIMCELL_COMMA \
			number_of_variables GENSIMCELL_COMMA \
			Current_Variable GENSIMCELL_COMMA \
			Rest_Of_Variables... \
		>& rhs GENSIMCELL_COMMA \
		const Last_Op_Var& last_op_var \
	) { \
		this->IMPLEMENTATION_NAME(last_op_var GENSIMCELL_COMMA rhs[last_op_var]); \
	} \
	\
	Cell_impl< \
		Transfer_Policy GENSIMCELL_COMMA \
		number_of_variables GENSIMCELL_COMMA \
		Current_Variable GENSIMCELL_COMMA \
		Rest_Of_Variables... \
	>& operator OPERATOR ( \
		const Cell_impl< \
			Transfer_Policy GENSIMCELL_COMMA \
			number_of_variables GENSIMCELL_COMMA \
			Current_Variable GENSIMCELL_COMMA \
			Rest_Of_Variables... \
		>& rhs \
	) { \
		this->NAME( \
			rhs GENSIMCELL_COMMA \
			Current_Variable() GENSIMCELL_COMMA \
			Rest_Of_Variables()... \
		); \
		return *this; \
	}

	GENSIMCELL_MAKE_OPERATOR(plus_equal, plus_equal_impl, +=)
	GENSIMCELL_MAKE_OPERATOR(minus_equal, minus_equal_impl, -=)
	GENSIMCELL_MAKE_OPERATOR(mul_equal, mul_equal_impl, *=)
	GENSIMCELL_MAKE_OPERATOR(div_equal, div_equal_impl, /=)

	#undef GENSIMCELL_MAKE_OPERATOR


	#define GENSIMCELL_MAKE_OPERATOR_OTHER(NAME, IMPL_NAME, OPERATOR, OTHER_TYPE) \
	template< \
		class... Operator_Variables \
	> void NAME( \
		const OTHER_TYPE& GENSIMCELL_COMMA \
		const Operator_Variables&... \
	) {} \
	\
	template< \
		class First_Op_Var GENSIMCELL_COMMA \
		class... Rest_Op_Vars \
	> void NAME( \
		const OTHER_TYPE& rhs GENSIMCELL_COMMA \
		const First_Op_Var& first_op_var GENSIMCELL_COMMA \
		const Rest_Op_Vars&... rest_op_vars \
	) { \
		this->IMPL_NAME(first_op_var GENSIMCELL_COMMA rhs); \
		this->NAME(rhs GENSIMCELL_COMMA rest_op_vars...); \
	} \
	\
	template< \
		class Last_Op_Var \
	> void NAME( \
		const OTHER_TYPE& rhs GENSIMCELL_COMMA \
		const Last_Op_Var& last_op_var \
	) { \
		this->IMPL_NAME(last_op_var GENSIMCELL_COMMA rhs); \
	} \
	\
	Cell_impl< \
		Transfer_Policy GENSIMCELL_COMMA \
		number_of_variables GENSIMCELL_COMMA \
		Current_Variable GENSIMCELL_COMMA \
		Rest_Of_Variables... \
	>& operator OPERATOR ( \
		const OTHER_TYPE& rhs \
	) { \
		this->NAME( \
			rhs GENSIMCELL_COMMA \
			Current_Variable() GENSIMCELL_COMMA \
			Rest_Of_Variables()... \
		); \
		return *this; \
	}

	GENSIMCELL_MAKE_OPERATOR_OTHER(plus_equal, plus_equal_impl, +=, bool)
	GENSIMCELL_MAKE_OPERATOR_OTHER(plus_equal, plus_equal_impl, +=, char)
	GENSIMCELL_MAKE_OPERATOR_OTHER(plus_equal, plus_equal_impl, +=, char16_t)
	GENSIMCELL_MAKE_OPERATOR_OTHER(plus_equal, plus_equal_impl, +=, char32_t)
	GENSIMCELL_MAKE_OPERATOR_OTHER(plus_equal, plus_equal_impl, +=, wchar_t)
	GENSIMCELL_MAKE_OPERATOR_OTHER(plus_equal, plus_equal_impl, +=, short)
	GENSIMCELL_MAKE_OPERATOR_OTHER(plus_equal, plus_equal_impl, +=, int)
	GENSIMCELL_MAKE_OPERATOR_OTHER(plus_equal, plus_equal_impl, +=, long)
	GENSIMCELL_MAKE_OPERATOR_OTHER(plus_equal, plus_equal_impl, +=, long long)
	GENSIMCELL_MAKE_OPERATOR_OTHER(plus_equal, plus_equal_impl, +=, signed char)
	GENSIMCELL_MAKE_OPERATOR_OTHER(plus_equal, plus_equal_impl, +=, unsigned char)
	GENSIMCELL_MAKE_OPERATOR_OTHER(plus_equal, plus_equal_impl, +=, unsigned short)
	GENSIMCELL_MAKE_OPERATOR_OTHER(plus_equal, plus_equal_impl, +=, unsigned int)
	GENSIMCELL_MAKE_OPERATOR_OTHER(plus_equal, plus_equal_impl, +=, unsigned long)
	GENSIMCELL_MAKE_OPERATOR_OTHER(plus_equal, plus_equal_impl, +=, unsigned long long)
	GENSIMCELL_MAKE_OPERATOR_OTHER(plus_equal, plus_equal_impl, +=, float)
	GENSIMCELL_MAKE_OPERATOR_OTHER(plus_equal, plus_equal_impl, +=, double)
	GENSIMCELL_MAKE_OPERATOR_OTHER(plus_equal, plus_equal_impl, +=, long double)
	GENSIMCELL_MAKE_OPERATOR_OTHER(minus_equal, minus_equal_impl, -=, bool)
	GENSIMCELL_MAKE_OPERATOR_OTHER(minus_equal, minus_equal_impl, -=, char)
	GENSIMCELL_MAKE_OPERATOR_OTHER(minus_equal, minus_equal_impl, -=, char16_t)
	GENSIMCELL_MAKE_OPERATOR_OTHER(minus_equal, minus_equal_impl, -=, char32_t)
	GENSIMCELL_MAKE_OPERATOR_OTHER(minus_equal, minus_equal_impl, -=, wchar_t)
	GENSIMCELL_MAKE_OPERATOR_OTHER(minus_equal, minus_equal_impl, -=, short)
	GENSIMCELL_MAKE_OPERATOR_OTHER(minus_equal, minus_equal_impl, -=, int)
	GENSIMCELL_MAKE_OPERATOR_OTHER(minus_equal, minus_equal_impl, -=, long)
	GENSIMCELL_MAKE_OPERATOR_OTHER(minus_equal, minus_equal_impl, -=, long long)
	GENSIMCELL_MAKE_OPERATOR_OTHER(minus_equal, minus_equal_impl, -=, signed char)
	GENSIMCELL_MAKE_OPERATOR_OTHER(minus_equal, minus_equal_impl, -=, unsigned char)
	GENSIMCELL_MAKE_OPERATOR_OTHER(minus_equal, minus_equal_impl, -=, unsigned short)
	GENSIMCELL_MAKE_OPERATOR_OTHER(minus_equal, minus_equal_impl, -=, unsigned int)
	GENSIMCELL_MAKE_OPERATOR_OTHER(minus_equal, minus_equal_impl, -=, unsigned long)
	GENSIMCELL_MAKE_OPERATOR_OTHER(minus_equal, minus_equal_impl, -=, unsigned long long)
	GENSIMCELL_MAKE_OPERATOR_OTHER(minus_equal, minus_equal_impl, -=, float)
	GENSIMCELL_MAKE_OPERATOR_OTHER(minus_equal, minus_equal_impl, -=, double)
	GENSIMCELL_MAKE_OPERATOR_OTHER(minus_equal, minus_equal_impl, -=, long double)
	GENSIMCELL_MAKE_OPERATOR_OTHER(mul_equal, mul_equal_impl, *=, bool)
	GENSIMCELL_MAKE_OPERATOR_OTHER(mul_equal, mul_equal_impl, *=, char)
	GENSIMCELL_MAKE_OPERATOR_OTHER(mul_equal, mul_equal_impl, *=, char16_t)
	GENSIMCELL_MAKE_OPERATOR_OTHER(mul_equal, mul_equal_impl, *=, char32_t)
	GENSIMCELL_MAKE_OPERATOR_OTHER(mul_equal, mul_equal_impl, *=, wchar_t)
	GENSIMCELL_MAKE_OPERATOR_OTHER(mul_equal, mul_equal_impl, *=, short)
	GENSIMCELL_MAKE_OPERATOR_OTHER(mul_equal, mul_equal_impl, *=, int)
	GENSIMCELL_MAKE_OPERATOR_OTHER(mul_equal, mul_equal_impl, *=, long)
	GENSIMCELL_MAKE_OPERATOR_OTHER(mul_equal, mul_equal_impl, *=, long long)
	GENSIMCELL_MAKE_OPERATOR_OTHER(mul_equal, mul_equal_impl, *=, signed char)
	GENSIMCELL_MAKE_OPERATOR_OTHER(mul_equal, mul_equal_impl, *=, unsigned char)
	GENSIMCELL_MAKE_OPERATOR_OTHER(mul_equal, mul_equal_impl, *=, unsigned short)
	GENSIMCELL_MAKE_OPERATOR_OTHER(mul_equal, mul_equal_impl, *=, unsigned int)
	GENSIMCELL_MAKE_OPERATOR_OTHER(mul_equal, mul_equal_impl, *=, unsigned long)
	GENSIMCELL_MAKE_OPERATOR_OTHER(mul_equal, mul_equal_impl, *=, unsigned long long)
	GENSIMCELL_MAKE_OPERATOR_OTHER(mul_equal, mul_equal_impl, *=, float)
	GENSIMCELL_MAKE_OPERATOR_OTHER(mul_equal, mul_equal_impl, *=, double)
	GENSIMCELL_MAKE_OPERATOR_OTHER(mul_equal, mul_equal_impl, *=, long double)
	GENSIMCELL_MAKE_OPERATOR_OTHER(div_equal, div_equal_impl, /=, bool)
	GENSIMCELL_MAKE_OPERATOR_OTHER(div_equal, div_equal_impl, /=, char)
	GENSIMCELL_MAKE_OPERATOR_OTHER(div_equal, div_equal_impl, /=, char16_t)
	GENSIMCELL_MAKE_OPERATOR_OTHER(div_equal, div_equal_impl, /=, char32_t)
	GENSIMCELL_MAKE_OPERATOR_OTHER(div_equal, div_equal_impl, /=, wchar_t)
	GENSIMCELL_MAKE_OPERATOR_OTHER(div_equal, div_equal_impl, /=, short)
	GENSIMCELL_MAKE_OPERATOR_OTHER(div_equal, div_equal_impl, /=, int)
	GENSIMCELL_MAKE_OPERATOR_OTHER(div_equal, div_equal_impl, /=, long)
	GENSIMCELL_MAKE_OPERATOR_OTHER(div_equal, div_equal_impl, /=, long long)
	GENSIMCELL_MAKE_OPERATOR_OTHER(div_equal, div_equal_impl, /=, signed char)
	GENSIMCELL_MAKE_OPERATOR_OTHER(div_equal, div_equal_impl, /=, unsigned char)
	GENSIMCELL_MAKE_OPERATOR_OTHER(div_equal, div_equal_impl, /=, unsigned short)
	GENSIMCELL_MAKE_OPERATOR_OTHER(div_equal, div_equal_impl, /=, unsigned int)
	GENSIMCELL_MAKE_OPERATOR_OTHER(div_equal, div_equal_impl, /=, unsigned long)
	GENSIMCELL_MAKE_OPERATOR_OTHER(div_equal, div_equal_impl, /=, unsigned long long)
	GENSIMCELL_MAKE_OPERATOR_OTHER(div_equal, div_equal_impl, /=, float)
	GENSIMCELL_MAKE_OPERATOR_OTHER(div_equal, div_equal_impl, /=, double)
	GENSIMCELL_MAKE_OPERATOR_OTHER(div_equal, div_equal_impl, /=, long double)

	#undef GENSIMCELL_MAKE_OPERATOR_OTHER


	#if defined(MPI_VERSION) && (MPI_VERSION >= 2)

	using Transfer_Policy<Current_Variable>::get_transfer_all;
	using Transfer_Policy<Current_Variable>::get_transfer;
	using Transfer_Policy<Current_Variable>::is_transferred;

	using Cell_impl<
		Transfer_Policy,
		number_of_variables,
		Rest_Of_Variables...
	>::set_transfer_all;

	using Cell_impl<
		Transfer_Policy,
		number_of_variables,
		Rest_Of_Variables...
	>::get_transfer_all;

	using Cell_impl<
		Transfer_Policy,
		number_of_variables,
		Rest_Of_Variables...
	>::set_transfer;

	using Cell_impl<
		Transfer_Policy,
		number_of_variables,
		Rest_Of_Variables...
	>::get_transfer;

	using Cell_impl<
		Transfer_Policy,
		number_of_variables,
		Rest_Of_Variables...
	>::is_transferred;


	/*!
	Sets the MPI transfer info of given variables.

	If given a determined tribool (true or false) the given
	variables will or will not be included in the MPI_Datatype
	returned by get_mpi_datatype() of all instances of this
	cell type. Setting an underemined value here will make
	the decision of whether to include a variable or not in
	the MPI transfer info to be decided on a cell by cell
	basis controlled by set_transfer().
	*/
	template<class... Given_Vars> static void set_transfer_all(
		const boost::logic::tribool given_transfer,
		const Given_Vars&...
	);

	//! See the general version for documentation
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

	//! See the general version for documentation
	template<class Given_Var> static void set_transfer_all(
		const boost::logic::tribool given_transfer,
		const Given_Var& var
	) {
		set_transfer_all_impl(given_transfer, var);
	}


	//! same as set_transfer_all but for this cell instance
	template<class... Given_Vars> void set_transfer(
		const bool given_transfer,
		const Given_Vars&...
	);

	//! same as set_transfer_all but for this cell instance
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

	//! same as set_transfer_all but for this cell instance
	template<class Given_Var> void set_transfer(
		const bool given_transfer,
		const Given_Var& var
	) {
		this->set_transfer_impl(given_transfer, var);
	}


	/*!
	Returns the MPI transfer info of this cell instance.

	By default no variables are included in the transfer info.
	This can be changed with set_transfer() and
	set_transfer_all() and queried with get_transfer(),
	get_transfer_all() and is_transferred().
	*/
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

		} else if (nr_vars_to_transfer <= size_t(std::numeric_limits<int>::max())) {

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

			// free user-defined component datatypes
			for (size_t i = 0; i < nr_vars_to_transfer; i++) {
				if (datatypes[i] == MPI_DATATYPE_NULL) {
					continue;
				}
				int combiner = -1, tmp1 = -1, tmp2 = -1, tmp3 = -1;
				MPI_Type_get_envelope(datatypes[i], &tmp1, &tmp2, &tmp3, &combiner);
				if (combiner != MPI_COMBINER_NAMED) {
					MPI_Type_free(&datatypes[i]);
				}
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


/*!
Stops the iteration over variables given by the user.

Operator () provides access to the user's data.
*/
template <
	template<class> class Transfer_Policy,
	size_t number_of_variables,
	class Variable
> class Cell_impl<
	Transfer_Policy,
	number_of_variables,
	Variable
> :
	public Transfer_Policy<Variable>
{


private:


	typename Variable::data_type data;



protected:


	#define GENSIMCELL_MAKE_OPERATOR_IMPLEMENTATION_LAST(NAME, OPERATOR) \
	template<class Other_T> void NAME( \
		const Variable& GENSIMCELL_COMMA \
		const Other_T& rhs \
	) { \
		this->data OPERATOR rhs; \
	}

	GENSIMCELL_MAKE_OPERATOR_IMPLEMENTATION_LAST(plus_equal_impl, +=)
	GENSIMCELL_MAKE_OPERATOR_IMPLEMENTATION_LAST(minus_equal_impl, -=)
	GENSIMCELL_MAKE_OPERATOR_IMPLEMENTATION_LAST(mul_equal_impl, *=)
	GENSIMCELL_MAKE_OPERATOR_IMPLEMENTATION_LAST(div_equal_impl, /=)
	#undef GENSIMCELL_MAKE_OPERATOR_IMPLEMENTATION_LAST


	#if defined(MPI_VERSION) && (MPI_VERSION >= 2)

	using Transfer_Policy<Variable>::set_transfer_all_impl;
	using Transfer_Policy<Variable>::set_transfer_impl;

	//! See the variadic version of Cell_impl for documentation
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

	//! See the variadic version of Cell_impl for documentation
	typename Variable::data_type& operator[](const Variable&)
	{
		return this->data;
	}

	//! See the variadic version of Cell_impl for documentation
	const typename Variable::data_type& operator[](const Variable&) const
	{
		return this->data;
	}


	#define GENSIMCELL_MAKE_OPERATOR_LAST(NAME, OPERATOR) \
	Cell_impl< \
		Transfer_Policy GENSIMCELL_COMMA \
		number_of_variables GENSIMCELL_COMMA \
		Variable \
	>& operator OPERATOR( \
		const Cell_impl< \
			Transfer_Policy GENSIMCELL_COMMA \
			number_of_variables GENSIMCELL_COMMA \
			Variable \
		>& rhs \
	) { \
		this->NAME(Variable() GENSIMCELL_COMMA rhs[Variable()]); \
		return *this; \
	}

	GENSIMCELL_MAKE_OPERATOR_LAST(plus_equal_impl, +=)
	GENSIMCELL_MAKE_OPERATOR_LAST(minus_equal_impl, -=)
	GENSIMCELL_MAKE_OPERATOR_LAST(mul_equal_impl, *=)
	GENSIMCELL_MAKE_OPERATOR_LAST(div_equal_impl, /=)

	#undef GENSIMCELL_MAKE_OPERATOR_LAST


	#define GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(NAME, OPERATOR, OTHER_TYPE) \
	Cell_impl< \
		Transfer_Policy GENSIMCELL_COMMA \
		number_of_variables GENSIMCELL_COMMA \
		Variable \
	>& operator OPERATOR( \
		const OTHER_TYPE& rhs \
	) { \
		this->NAME(Variable() GENSIMCELL_COMMA rhs); \
		return *this; \
	}

	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(plus_equal_impl, +=, bool)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(plus_equal_impl, +=, char)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(plus_equal_impl, +=, char16_t)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(plus_equal_impl, +=, char32_t)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(plus_equal_impl, +=, wchar_t)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(plus_equal_impl, +=, short)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(plus_equal_impl, +=, int)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(plus_equal_impl, +=, long)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(plus_equal_impl, +=, long long)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(plus_equal_impl, +=, signed char)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(plus_equal_impl, +=, unsigned char)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(plus_equal_impl, +=, unsigned short)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(plus_equal_impl, +=, unsigned int)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(plus_equal_impl, +=, unsigned long)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(plus_equal_impl, +=, unsigned long long)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(plus_equal_impl, +=, float)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(plus_equal_impl, +=, double)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(plus_equal_impl, +=, long double)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(minus_equal_impl, -=, bool)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(minus_equal_impl, -=, char)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(minus_equal_impl, -=, char16_t)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(minus_equal_impl, -=, char32_t)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(minus_equal_impl, -=, wchar_t)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(minus_equal_impl, -=, short)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(minus_equal_impl, -=, int)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(minus_equal_impl, -=, long)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(minus_equal_impl, -=, long long)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(minus_equal_impl, -=, signed char)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(minus_equal_impl, -=, unsigned char)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(minus_equal_impl, -=, unsigned short)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(minus_equal_impl, -=, unsigned int)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(minus_equal_impl, -=, unsigned long)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(minus_equal_impl, -=, unsigned long long)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(minus_equal_impl, -=, float)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(minus_equal_impl, -=, double)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(minus_equal_impl, -=, long double)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(mul_equal_impl, *=, bool)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(mul_equal_impl, *=, char)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(mul_equal_impl, *=, char16_t)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(mul_equal_impl, *=, char32_t)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(mul_equal_impl, *=, wchar_t)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(mul_equal_impl, *=, short)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(mul_equal_impl, *=, int)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(mul_equal_impl, *=, long)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(mul_equal_impl, *=, long long)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(mul_equal_impl, *=, signed char)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(mul_equal_impl, *=, unsigned char)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(mul_equal_impl, *=, unsigned short)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(mul_equal_impl, *=, unsigned int)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(mul_equal_impl, *=, unsigned long)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(mul_equal_impl, *=, unsigned long long)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(mul_equal_impl, *=, float)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(mul_equal_impl, *=, double)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(mul_equal_impl, *=, long double)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(div_equal_impl, /=, bool)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(div_equal_impl, /=, char)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(div_equal_impl, /=, char16_t)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(div_equal_impl, /=, char32_t)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(div_equal_impl, /=, wchar_t)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(div_equal_impl, /=, short)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(div_equal_impl, /=, int)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(div_equal_impl, /=, long)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(div_equal_impl, /=, long long)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(div_equal_impl, /=, signed char)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(div_equal_impl, /=, unsigned char)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(div_equal_impl, /=, unsigned short)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(div_equal_impl, /=, unsigned int)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(div_equal_impl, /=, unsigned long)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(div_equal_impl, /=, unsigned long long)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(div_equal_impl, /=, float)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(div_equal_impl, /=, double)
	GENSIMCELL_MAKE_OPERATOR_LAST_OTHER(div_equal_impl, /=, long double)

	#undef GENSIMCELL_MAKE_OPERATOR_LAST_OTHER
	#undef GENSIMCELL_COMMA


	#if defined(MPI_VERSION) && (MPI_VERSION >= 2)

	using Transfer_Policy<Variable>::get_transfer_all;
	using Transfer_Policy<Variable>::get_transfer;
	using Transfer_Policy<Variable>::is_transferred;


	//! See the variadic version of Cell_impl for documentation
	template<class... Given_Vars> static void set_transfer_all(
		const boost::logic::tribool given_transfer,
		const Given_Vars&...
	);

	//! See the variadic version of Cell_impl for documentation
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

	//! See the variadic version of Cell_impl for documentation
	template<class Given_Var> static void set_transfer_all(
		const boost::logic::tribool given_transfer,
		const Given_Var& var
	) {
		set_transfer_all_impl(given_transfer, var);
	}


	//! See the variadic version of Cell_impl for documentation
	template<class... Given_Vars> void set_transfer(
		const bool given_transfer,
		const Given_Vars&...
	);

	//! See the variadic version of Cell_impl for documentation
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

	//! See the variadic version of Cell_impl for documentation
	template<class Given_Var> void set_transfer(
		const bool given_transfer,
		const Given_Var& var
	) {
		this->set_transfer_impl(given_transfer, var);
	}


	//! See the variadic version of Cell_impl for documentation
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



} // namespace detail
} // namespace gensimcell


#endif // ifndef GENSIMCELL_IMPL_HPP
