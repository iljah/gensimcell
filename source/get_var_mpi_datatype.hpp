/*
Functions that return the MPI transfer information of some C++ types.

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

#ifndef GET_VAR_MPI_DATATYPE_HPP
#define GET_VAR_MPI_DATATYPE_HPP

#if defined(MPI_VERSION) && (MPI_VERSION >= 2)

#include "complex"
#include "cstddef"
#include "cstdint"
#include "tuple"
#include "type_traits"

#include "boost/function_types/property_tags.hpp"
#include "boost/mpl/vector.hpp"
#include "boost/tti/has_member_function.hpp"
#include "mpi.h"

#include "type_support.hpp"


namespace gensimcell {
namespace detail {


BOOST_TTI_HAS_MEMBER_FUNCTION(get_mpi_datatype)


/*!
Returns the mpi transfer info of given
variable with its get_mpi_datatype function.
*/
template <
	class Variable_T,
	std::size_t Number_Of_Items = 0
> std::tuple<
	void*,
	int,
	MPI_Datatype
> get_var_mpi_datatype(const Variable_T& variable)
{
	static_assert(
		has_member_function_get_mpi_datatype<
			Variable_T,
			std::tuple<void*, int, MPI_Datatype>
		>::value
		or
		has_member_function_get_mpi_datatype<
			Variable_T,
			std::tuple<void*, int, MPI_Datatype>,
			boost::mpl::vector<>,
			boost::function_types::const_qualified
		>::value,
		"Given Variable_T does not have a get_mpi_datatype() "
		"member function that must specify the data to be "
		"transferred by MPI, see the Particles class in "
		"gensimcell/examples/particle_propagation/parallel/particle_variables.hpp "
		"for an example."
	);
	return variable.get_mpi_datatype();
}


/*!
Specializations of get_var_mpi_datatype for
standard C++ types with an MPI equivalent.
*/
#define GENSIMCELL_GET_VAR_MPI_DATATYPE(GIVEN_CPP_TYPE, GIVEN_MPI_TYPE) \
template <> std::tuple< \
	void*, \
	int, \
	MPI_Datatype                                                      \
> get_var_mpi_datatype(const GIVEN_CPP_TYPE& variable) \
{ \
	return std::make_tuple((void*) &variable, 1, GIVEN_MPI_TYPE); \
}

GENSIMCELL_GET_VAR_MPI_DATATYPE(char, MPI_CHAR)
GENSIMCELL_GET_VAR_MPI_DATATYPE(double, MPI_DOUBLE)
GENSIMCELL_GET_VAR_MPI_DATATYPE(float, MPI_FLOAT)
GENSIMCELL_GET_VAR_MPI_DATATYPE(long double, MPI_LONG_DOUBLE)
GENSIMCELL_GET_VAR_MPI_DATATYPE(signed char, MPI_SIGNED_CHAR)
GENSIMCELL_GET_VAR_MPI_DATATYPE(signed int, MPI_INT)
GENSIMCELL_GET_VAR_MPI_DATATYPE(signed long int, MPI_LONG)
GENSIMCELL_GET_VAR_MPI_DATATYPE(signed long long int, MPI_LONG_LONG_INT)
GENSIMCELL_GET_VAR_MPI_DATATYPE(signed short int, MPI_SHORT)
GENSIMCELL_GET_VAR_MPI_DATATYPE(unsigned char, MPI_UNSIGNED_CHAR)
GENSIMCELL_GET_VAR_MPI_DATATYPE(unsigned int, MPI_UNSIGNED)
GENSIMCELL_GET_VAR_MPI_DATATYPE(unsigned long int, MPI_UNSIGNED_LONG)
GENSIMCELL_GET_VAR_MPI_DATATYPE(unsigned long long int, MPI_UNSIGNED_LONG_LONG)
GENSIMCELL_GET_VAR_MPI_DATATYPE(unsigned short int, MPI_UNSIGNED_SHORT)
GENSIMCELL_GET_VAR_MPI_DATATYPE(wchar_t, MPI_WCHAR)

#ifdef MPI_CXX_BOOL
GENSIMCELL_GET_VAR_MPI_DATATYPE(bool, MPI_CXX_BOOL)
#endif

#ifdef MPI_CXX_FLOAT_COMPLEX
GENSIMCELL_GET_VAR_MPI_DATATYPE(std::complex<float>, MPI_CXX_FLOAT_COMPLEX)
#endif

#ifdef MPI_CXX_DOUBLE_COMPLEX
GENSIMCELL_GET_VAR_MPI_DATATYPE(std::complex<double>, MPI_CXX_DOUBLE_COMPLEX)
#endif

#ifdef MPI_CXX_LONG_DOUBLE_COMPLEX
GENSIMCELL_GET_VAR_MPI_DATATYPE(std::complex<long double>, MPI_CXX_LONG_DOUBLE_COMPLEX)
#endif

#undef GENSIMCELL_GET_VAR_MPI_DATATYPE


/*!
Specializations of get_var_mpi_datatype for standard
C++ types with an MPI equivalent inside an array.
*/
#define GENSIMCELL_GET_ARRAY_VAR_MPI_DATATYPE(GIVEN_CPP_TYPE, GIVEN_MPI_TYPE) \
template < \
	std::size_t Number_Of_Items \
> std::tuple< \
	void*, \
	int, \
	MPI_Datatype \
> get_var_mpi_datatype( \
	const std::array< \
		GIVEN_CPP_TYPE, \
		Number_Of_Items \
	>& variable \
) { \
	return std::make_tuple( \
		(void*) variable.data(), \
		Number_Of_Items, \
		GIVEN_MPI_TYPE \
	); \
}

GENSIMCELL_GET_ARRAY_VAR_MPI_DATATYPE(char, MPI_CHAR)
GENSIMCELL_GET_ARRAY_VAR_MPI_DATATYPE(double, MPI_DOUBLE)
GENSIMCELL_GET_ARRAY_VAR_MPI_DATATYPE(float, MPI_FLOAT)
GENSIMCELL_GET_ARRAY_VAR_MPI_DATATYPE(long double, MPI_LONG_DOUBLE)
GENSIMCELL_GET_ARRAY_VAR_MPI_DATATYPE(signed char, MPI_SIGNED_CHAR)
GENSIMCELL_GET_ARRAY_VAR_MPI_DATATYPE(signed int, MPI_INT)
GENSIMCELL_GET_ARRAY_VAR_MPI_DATATYPE(signed long int, MPI_LONG)
GENSIMCELL_GET_ARRAY_VAR_MPI_DATATYPE(signed long long int, MPI_LONG_LONG_INT)
GENSIMCELL_GET_ARRAY_VAR_MPI_DATATYPE(signed short int, MPI_SHORT)
GENSIMCELL_GET_ARRAY_VAR_MPI_DATATYPE(unsigned char, MPI_UNSIGNED_CHAR)
GENSIMCELL_GET_ARRAY_VAR_MPI_DATATYPE(unsigned int, MPI_UNSIGNED)
GENSIMCELL_GET_ARRAY_VAR_MPI_DATATYPE(unsigned long int, MPI_UNSIGNED_LONG)
GENSIMCELL_GET_ARRAY_VAR_MPI_DATATYPE(unsigned long long int, MPI_UNSIGNED_LONG_LONG)
GENSIMCELL_GET_ARRAY_VAR_MPI_DATATYPE(unsigned short int, MPI_UNSIGNED_SHORT)
GENSIMCELL_GET_ARRAY_VAR_MPI_DATATYPE(wchar_t, MPI_WCHAR)

#ifdef MPI_CXX_BOOL
GENSIMCELL_GET_ARRAY_VAR_MPI_DATATYPE(bool, MPI_CXX_BOOL)
#endif

#ifdef MPI_CXX_FLOAT_COMPLEX
GENSIMCELL_GET_ARRAY_VAR_MPI_DATATYPE(std::complex<float>, MPI_CXX_FLOAT_COMPLEX)
#endif

#ifdef MPI_CXX_DOUBLE_COMPLEX
GENSIMCELL_GET_ARRAY_VAR_MPI_DATATYPE(std::complex<double>, MPI_CXX_DOUBLE_COMPLEX)
#endif

#ifdef MPI_CXX_LONG_DOUBLE_COMPLEX
GENSIMCELL_GET_ARRAY_VAR_MPI_DATATYPE(std::complex<long double>, MPI_CXX_LONG_DOUBLE_COMPLEX)
#endif

#undef GENSIMCELL_GET_ARRAY_VAR_MPI_DATATYPE



/*!
Returns transfer info for an array of generic simulation cells.

Returns count == 0 if Number_Of_Cells == 0.
Returns transfer info given by cell if Number_Of_Cells == 1.
Returns a structured datatype if Number_Of_Cells > 1 omitting
cells which return a count == 0.
*/
template <
	class Cell,
	std::size_t Number_Of_Cells
> typename std::enable_if<
	is_gensimcell<Cell>::value,
	std::tuple<
		void*,
		int,
		MPI_Datatype
	>
>::type get_var_mpi_datatype(
	const std::array<
		Cell,
		Number_Of_Cells
	>& variables
) {
	if (Number_Of_Cells == 0) {
		return std::make_tuple(nullptr, 0, MPI_BYTE);
	}

	if (Number_Of_Cells == 1) {
		return variables[0].get_mpi_datatype();
	}


	std::array<void*, Number_Of_Cells> addresses{{nullptr}};
	std::array<int, Number_Of_Cells> counts{{0}};
	std::array<MPI_Datatype, Number_Of_Cells> datatypes{{MPI_BYTE}};

	// skip cells with nothing to transfer
	size_t cells_to_transfer = 0;
	for (size_t i = 0; i < Number_Of_Cells; i++) {
		std::tie(
			addresses[cells_to_transfer],
			counts[cells_to_transfer],
			datatypes[cells_to_transfer]
		) = variables[i].get_mpi_datatype();

		// assume 0 count means don't transfer
		if (counts[cells_to_transfer] > 0) {
			cells_to_transfer++;
		}
	}

	if (cells_to_transfer == 0) {
		return std::make_tuple(nullptr, 0, MPI_BYTE);
	}


	std::array<MPI_Aint, Number_Of_Cells> displacements{{0}};
	for (size_t i = 0; i < cells_to_transfer; i++) {
		displacements[i]
			= static_cast<char*>(addresses[i])
			- static_cast<char*>(addresses[0]);
	}

	MPI_Datatype final_datatype = MPI_DATATYPE_NULL;
	if (
		MPI_Type_create_struct(
			int(cells_to_transfer),
			counts.data(),
			displacements.data(),
			datatypes.data(),
			&final_datatype
		) != MPI_SUCCESS
	) {
		return std::make_tuple(nullptr, -1, MPI_DATATYPE_NULL);
	}

	// free component datatypes
	for (size_t i = 0; i < cells_to_transfer; i++) {
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
}



#ifdef EIGEN_WORLD_VERSION
/*!
Specializations of get_var_mpi_datatype for standard
C++ types with an MPI equivalent inside an Eigen matrix.
*/
#define GENSIMCELL_GET_EIGEN_VAR_MPI_DATATYPE(GIVEN_CPP_TYPE, GIVEN_MPI_TYPE) \
template < \
	int Rows, \
	int Columns \
> std::tuple< \
	void*, \
	int, \
	MPI_Datatype \
> get_var_mpi_datatype( \
	const Eigen::Matrix< \
		GIVEN_CPP_TYPE, \
		Rows, \
		Columns \
	>& variable \
) { \
	static_assert( \
		Rows != Eigen::Dynamic, \
		"Only compile time sized Eigen matrices are supported" \
	); \
	static_assert( \
		Columns != Eigen::Dynamic, \
		"Only compile time sized Eigen matrices are supported" \
	); \
	return std::make_tuple( \
		(void*) variable.data(), \
		Rows * Columns, \
		GIVEN_MPI_TYPE \
	); \
}

GENSIMCELL_GET_EIGEN_VAR_MPI_DATATYPE(char, MPI_CHAR)
GENSIMCELL_GET_EIGEN_VAR_MPI_DATATYPE(double, MPI_DOUBLE)
GENSIMCELL_GET_EIGEN_VAR_MPI_DATATYPE(float, MPI_FLOAT)
GENSIMCELL_GET_EIGEN_VAR_MPI_DATATYPE(long double, MPI_LONG_DOUBLE)
GENSIMCELL_GET_EIGEN_VAR_MPI_DATATYPE(signed char, MPI_SIGNED_CHAR)
GENSIMCELL_GET_EIGEN_VAR_MPI_DATATYPE(signed int, MPI_INT)
GENSIMCELL_GET_EIGEN_VAR_MPI_DATATYPE(signed long int, MPI_LONG)
GENSIMCELL_GET_EIGEN_VAR_MPI_DATATYPE(signed long long int, MPI_LONG_LONG_INT)
GENSIMCELL_GET_EIGEN_VAR_MPI_DATATYPE(signed short int, MPI_SHORT)
GENSIMCELL_GET_EIGEN_VAR_MPI_DATATYPE(unsigned char, MPI_UNSIGNED_CHAR)
GENSIMCELL_GET_EIGEN_VAR_MPI_DATATYPE(unsigned int, MPI_UNSIGNED)
GENSIMCELL_GET_EIGEN_VAR_MPI_DATATYPE(unsigned long int, MPI_UNSIGNED_LONG)
GENSIMCELL_GET_EIGEN_VAR_MPI_DATATYPE(unsigned long long int, MPI_UNSIGNED_LONG_LONG)
GENSIMCELL_GET_EIGEN_VAR_MPI_DATATYPE(unsigned short int, MPI_UNSIGNED_SHORT)
GENSIMCELL_GET_EIGEN_VAR_MPI_DATATYPE(wchar_t, MPI_WCHAR)

#ifdef MPI_CXX_BOOL
GENSIMCELL_GET_EIGEN_VAR_MPI_DATATYPE(bool, MPI_CXX_BOOL)
#endif

#ifdef MPI_CXX_FLOAT_COMPLEX
GENSIMCELL_GET_EIGEN_VAR_MPI_DATATYPE(std::complex<float>, MPI_CXX_FLOAT_COMPLEX)
#endif

#ifdef MPI_CXX_DOUBLE_COMPLEX
GENSIMCELL_GET_EIGEN_VAR_MPI_DATATYPE(std::complex<double>, MPI_CXX_DOUBLE_COMPLEX)
#endif

#ifdef MPI_CXX_LONG_DOUBLE_COMPLEX
GENSIMCELL_GET_EIGEN_VAR_MPI_DATATYPE(std::complex<long double>, MPI_CXX_LONG_DOUBLE_COMPLEX)
#endif

#undef GENSIMCELL_GET_EIGEN_VAR_MPI_DATATYPE

#endif // ifdef EIGEN_WORLD_VERSION


} // namespace detail
} // namespace gensimcell

#endif // ifdef MPI_VERSION

#endif // ifndef  GET_VAR_DATATYPE_HPP
