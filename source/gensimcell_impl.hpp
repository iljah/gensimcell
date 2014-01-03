/*
Copyright (c) 2013, Ilja Honkonen
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

* Neither the name of NASA nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

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


#include "boost/logic/tribool.hpp"
#include "cstdlib"
#include "limits"
#include "mpi.h"
#include "vector"

#include "get_var_datatype.hpp"
#include "iostream"

namespace gensimcell {

/*!
Everything in this namespace should be considered as an
implementation detail that is subject to change without notice.
*/
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


	/*!
	Whether each instance tranfers
	this variable or not using MPI
	*/
	static boost::logic::tribool transfer_all;
	/*!
	Whether this instance sends this variable
	if transfer_all is indeterminite
	*/
	bool transfer = true;



protected:


	std::tuple<
		std::vector<void*>,
		std::vector<int>,
		std::vector<MPI_Datatype>
	> get_mpi_datatype_impl(
		std::tuple<
			std::vector<void*>,
			std::vector<int>,
			std::vector<MPI_Datatype>
		> transfer_info
	) const {

		/*!
		Put transfer info of variables closer
		to start of memory in front
		*/
		transfer_info = Cell_impl<
			number_of_variables,
			Rest_Of_Variables...
		>::get_mpi_datatype_impl(transfer_info);

		if (this->is_transferred(Current_Variable())) {
			void* address = NULL;
			int count = -1;
			MPI_Datatype datatype = MPI_DATATYPE_NULL;

			std::tie(
				address,
				count,
				datatype
			) = get_var_datatype(this->data);

			std::get<0>(transfer_info).push_back(address);
			std::get<1>(transfer_info).push_back(count);
			std::get<2>(transfer_info).push_back(datatype);
		}

		return transfer_info;
	}



public:


	/*
	Make all public functions of the inherited implementation(s)
	available also through the current iteration over user's variables.
	*/
	using Cell_impl<number_of_variables, Rest_Of_Variables...>::operator();
	using Cell_impl<number_of_variables, Rest_Of_Variables...>::set_transfer_all;
	using Cell_impl<number_of_variables, Rest_Of_Variables...>::get_transfer_all;
	using Cell_impl<number_of_variables, Rest_Of_Variables...>::set_transfer;
	using Cell_impl<number_of_variables, Rest_Of_Variables...>::get_transfer;
	using Cell_impl<number_of_variables, Rest_Of_Variables...>::is_transferred;


	typename Current_Variable::data_type& operator()(const Current_Variable&)
	{
		return this->data;
	};

	const typename Current_Variable::data_type& operator()(const Current_Variable&) const
	{
		return this->data;
	};


	static void set_transfer_all(
		const Current_Variable&,
		const boost::logic::tribool given
	) {
		transfer_all = given;
	}

	static boost::logic::tribool get_transfer_all(const Current_Variable&)
	{
		return transfer_all;
	}


	void set_transfer(
		const Current_Variable&,
		const bool given
	) {
		this->transfer = given;
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
		std::vector<void*> addresses;
		std::vector<int> counts;
		std::vector<MPI_Datatype> datatypes;

		std::tie(
			addresses,
			counts,
			datatypes
		) = this->get_mpi_datatype_impl(
			std::tuple<
				std::vector<void*>,
				std::vector<int>,
				std::vector<MPI_Datatype>
			>()
		);

		const size_t nr_vars_to_transfer = addresses.size();

		if (nr_vars_to_transfer == 0) {

			// assume NULL won't be dereferenced if count = 0
			return std::make_tuple((void*) NULL, 0, MPI_BYTE);

		} else if (nr_vars_to_transfer == 1) {

			if (counts.size() != 1 or datatypes.size() != 1) {
				// this shouldn't happen
				abort();
			}

			return std::make_tuple(
				(void*) addresses[0],
				counts[0],
				datatypes[0]
			);

		} else if (nr_vars_to_transfer <= std::numeric_limits<int>::max()) {

			// get displacements of variables to transfer
			std::vector<MPI_Aint> displacements(nr_vars_to_transfer, 0);
			for (size_t i = 1; i < nr_vars_to_transfer; i++) {
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

			return std::make_tuple((void*) addresses[0], 1, final_datatype);

		} else {

			return std::make_tuple(
				(void*) NULL,
				std::numeric_limits<int>::lowest(),
				MPI_DATATYPE_NULL
			);

		}
	}

};

template <
	size_t number_of_variables,
	class Current_Variable,
	class... Rest_Of_Variables
> boost::logic::tribool Cell_impl<
	number_of_variables,
	Current_Variable,
	Rest_Of_Variables...
>::transfer_all = true;



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


	/*!
	Whether each instance tranfers
	this variable or not using MPI
	*/
	static boost::logic::tribool transfer_all;
	/*!
	Whether this instance sends this variable
	if transfer_all is indeterminite
	*/
	bool transfer = true;



protected:


	std::tuple<
		std::vector<void*>,
		std::vector<int>,
		std::vector<MPI_Datatype>
	> get_mpi_datatype_impl(
		std::tuple<
			std::vector<void*>,
			std::vector<int>,
			std::vector<MPI_Datatype>
		> transfer_info
	) const {

		if (this->is_transferred(Variable())) {
			void* address = NULL;
			int count = -1;
			MPI_Datatype datatype = MPI_DATATYPE_NULL;

			std::tie(
				address,
				count,
				datatype
			) = get_var_datatype(this->data);

			std::get<0>(transfer_info).push_back(address);
			std::get<1>(transfer_info).push_back(count);
			std::get<2>(transfer_info).push_back(datatype);
		}

		return transfer_info;
	}



public:


	typename Variable::data_type& operator()(const Variable&)
	{
		return this->data;
	}

	const typename Variable::data_type& operator()(const Variable&) const
	{
		return this->data;
	}


	static void set_transfer_all(
		const Variable&,
		const boost::logic::tribool given
	) {
		transfer_all = given;
	}

	static boost::logic::tribool get_transfer_all(const Variable&)
	{
		return transfer_all;
	}


	void set_transfer(const Variable&, const bool given)
	{
		this->transfer = given;
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
		if (not this->is_transferred(Variable())) {

			return std::make_tuple((void*) NULL, 0, MPI_BYTE);

		} else {

			std::vector<void*> addresses;
			std::vector<int> counts;
			std::vector<MPI_Datatype> datatypes;

			std::tie(
				addresses,
				counts,
				datatypes
			) = this->get_mpi_datatype_impl(
				std::tuple<
					std::vector<void*>,
					std::vector<int>,
					std::vector<MPI_Datatype>
				>()
			);

			if (
				addresses.size() != 1
				or counts.size() != 1
				or datatypes.size() != 1
			) {
				abort();
			}

			return std::make_tuple(
				(void*) addresses[0],
				counts[0],
				datatypes[0]
			);
		}
	}

};

template <
	size_t number_of_variables,
	class Variable
> boost::logic::tribool Cell_impl<
	number_of_variables,
	Variable
>::transfer_all = true;



} // namespace detail
} // namespace gensimcell


#endif // ifndef GENSIMCELL_IMPL_HPP
