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

#include "complex"
#include "cstdint"
#include "mpi.h"
#include "tuple"

#include "boost/tti/has_member_function.hpp"


#ifndef GET_VAR_DATATYPE_HPP
#define GET_VAR_DATATYPE_HPP


namespace gensimcell {
namespace detail {


BOOST_TTI_HAS_MEMBER_FUNCTION(get_mpi_datatype)


/*!
Returns the mpi transfer info of given
variable with a get_mpi_datatype function.
*/
template <
	class Variable_T
> std::tuple<
	void*,
	int,
	MPI_Datatype
> get_var_datatype(const Variable_T& variable)
{
	static_assert(
		has_member_function_get_mpi_datatype<
			Variable_T,
			boost::mpl::vector<void*, int, MPI_Datatype>
		>::value,
		"Given Variable_T does not have a get_mpi_datatype "
		"member function that specifies the data to be "
		"transferred by MPI."
	);
	return variable.get_mpi_datatype();
};


/*!
Specializations of get_var_datatype for
standard C++ types with an MPI equivalent.
*/
#define GENSIMCELL_GET_VAR_DATATYPE(GIVEN_CPP_TYPE, GIVEN_MPI_TYPE)   \
template <> std::tuple<                                               \
	void*,                                                            \
	int,                                                              \
	MPI_Datatype                                                      \
> get_var_datatype<GIVEN_CPP_TYPE>(const GIVEN_CPP_TYPE& variable)    \
{                                                                     \
	return std::make_tuple((void*) &variable, 1, GIVEN_MPI_TYPE);     \
};

GENSIMCELL_GET_VAR_DATATYPE(bool, MPI_CXX_BOOL)
GENSIMCELL_GET_VAR_DATATYPE(char, MPI_CHAR)
GENSIMCELL_GET_VAR_DATATYPE(double, MPI_DOUBLE)
GENSIMCELL_GET_VAR_DATATYPE(float, MPI_FLOAT)
GENSIMCELL_GET_VAR_DATATYPE(long double, MPI_LONG_DOUBLE)
GENSIMCELL_GET_VAR_DATATYPE(signed char, MPI_SIGNED_CHAR)
GENSIMCELL_GET_VAR_DATATYPE(signed int, MPI_INT)
GENSIMCELL_GET_VAR_DATATYPE(signed long int, MPI_LONG)
GENSIMCELL_GET_VAR_DATATYPE(signed long long int, MPI_LONG_LONG_INT)
GENSIMCELL_GET_VAR_DATATYPE(signed short int, MPI_SHORT)
GENSIMCELL_GET_VAR_DATATYPE(std::complex<float>, MPI_CXX_FLOAT_COMPLEX)
GENSIMCELL_GET_VAR_DATATYPE(std::complex<double>, MPI_CXX_DOUBLE_COMPLEX)
GENSIMCELL_GET_VAR_DATATYPE(std::complex<long double>, MPI_CXX_LONG_DOUBLE_COMPLEX)
GENSIMCELL_GET_VAR_DATATYPE(unsigned char, MPI_UNSIGNED_CHAR)
GENSIMCELL_GET_VAR_DATATYPE(unsigned int, MPI_UNSIGNED)
GENSIMCELL_GET_VAR_DATATYPE(unsigned long int, MPI_UNSIGNED_LONG)
GENSIMCELL_GET_VAR_DATATYPE(unsigned long long int, MPI_UNSIGNED_LONG_LONG)
GENSIMCELL_GET_VAR_DATATYPE(unsigned short int, MPI_UNSIGNED_SHORT)
GENSIMCELL_GET_VAR_DATATYPE(wchar_t, MPI_WCHAR)


} // namespace detail
} // namespace gensimcell

#endif // ifndef  GET_VAR_DATATYPE_HPP
