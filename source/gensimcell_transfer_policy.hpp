/*


Copyright 2014 Ilja Honkonen
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

#ifndef GENSIMCELL_TRANSFER_POLICY_HPP
#define GENSIMCELL_TRANSFER_POLICY_HPP


#include "cstdlib"
#include "limits"
#include "vector"

#include "boost/logic/tribool.hpp"

#include "get_var_mpi_datatype.hpp"


namespace gensimcell {


/*!
Makes gensimcell never transfer any variables' data between processes.
*/
template<class Variable> class Never_Transfer
{
protected:

	#if defined(MPI_VERSION) && (MPI_VERSION >= 2)

	//! Sets global transfer info of given variable
	static void set_transfer_all_impl(
		const boost::logic::tribool,
		const Variable&
	) {}

	static boost::logic::tribool get_transfer_all(const Variable&)
	{
		return false;
	}

	//! Sets this cell instance's transfer info of given variable
	void set_transfer_impl(
		const bool given_transfer,
		const Variable&
	) {}

	bool get_transfer(const Variable&) const
	{
		return false;
	}

	bool is_transferred(const Variable&) const
	{
		return false;
	}

	#endif // if defined MPI...
};


/*!
Makes gensimcell always transfer all variables' data between processes.
*/
template<class Variable> class Always_Transfer
{
protected:

	#if defined(MPI_VERSION) && (MPI_VERSION >= 2)

	template<class... Variables> static void set_transfer_all_impl(
		const boost::logic::tribool given_transfer,
		const Variables&...
	) {}

	static boost::logic::tribool get_transfer_all(const Variable&)
	{
		return true;
	}

	template<class... Variables> void set_transfer_impl(
		const bool given_transfer,
		const Variables&...
	) {}

	bool get_transfer(const Variable&) const
	{
		return true;
	}

	bool is_transferred(const Variable&) const
	{
		return true;
	}

	#endif // if defined MPI...
};



/*!
User can choose on a per-cell and per-variable basis what to transfer.

When this class is given as a transfer policy the generic cell,
transfer of variable data between processes can be switched on or off
on a cell-by-cell basis or in all cells using the same variable.
*/
template<class Variable> class Optional_Transfer
{
protected:

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

	//! Sets global transfer info of given variable
	static void set_transfer_all_impl(
		const boost::logic::tribool given_transfer,
		const Variable&
	) {
		transfer_all = given_transfer;
	}


	//! Sets this cell instance's transfer info of given variable
	void set_transfer_impl(
		const bool given_transfer,
		const Variable&
	) {
		this->transfer = given_transfer;
	}


	//! Returns the value set by set_transfer_all() for given variable
	static boost::logic::tribool get_transfer_all(const Variable&)
	{
		return transfer_all;
	}


	//! Returns the value set by set_transfer() for given variable
	bool get_transfer(const Variable&) const
	{
		return this->transfer;
	}


	/*!
	Returns true if given variable will be added to the transfer
	info returned by get_mpi_datatype() and false otherwise.
	*/
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

	#endif // if defined MPI...
};



#if defined(MPI_VERSION) && (MPI_VERSION >= 2)
template<
	class Variable
> boost::logic::tribool Optional_Transfer<Variable>::transfer_all = false;
#endif


} // namespace gensimcell


#endif // ifndef GENSIMCELL_TRANSFER_POLICY_HPP
