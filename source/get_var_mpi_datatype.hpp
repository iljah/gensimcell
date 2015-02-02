/*
Functions that return the MPI transfer information of some C++ types.

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


mpi.h must be included prior to including this file.
To get support for Eigen types Eigen/Core must be included before this file.
*/

#ifndef GET_VAR_MPI_DATATYPE_HPP
#define GET_VAR_MPI_DATATYPE_HPP

#if defined(MPI_VERSION) && (MPI_VERSION >= 2)

#include "array"
#include "complex"
#include "cstddef"
#include "cstdint"
#include "limits"
#include "tuple"
#include "type_traits"
#include "vector"

#include "boost/function_types/property_tags.hpp"
#include "boost/mpl/vector.hpp"
#include "boost/tti/has_member_function.hpp"

#include "type_support.hpp"


namespace gensimcell {
namespace detail {


BOOST_TTI_HAS_MEMBER_FUNCTION(get_mpi_datatype)


/*!
Returns the mpi transfer info for types with a get_mpi_datatype() member.
*/
template <
	class Variable_T
> typename std::enable_if<
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
	std::tuple<
		void*,
		int,
		MPI_Datatype
	>
>::type get_var_mpi_datatype(
	const Variable_T& variable
) {
	return variable.get_mpi_datatype();
}


/*!
Specializations of get_var_mpi_datatype for
standard C++ types with an MPI equivalent.
*/
#define GENSIMCELL_GET_VAR_MPI_DATATYPE(GIVEN_CPP_TYPE, GIVEN_MPI_TYPE) \
std::tuple< \
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
Specializations of get_var_mpi_datatype for standard
C++ types with an MPI equivalent inside a vector.
*/
#define GENSIMCELL_GET_VECTOR_VAR_MPI_DATATYPE(GIVEN_CPP_TYPE, GIVEN_MPI_TYPE) \
std::tuple< \
	void*, \
	int, \
	MPI_Datatype \
> get_var_mpi_datatype( \
	const std::vector<GIVEN_CPP_TYPE>& variable \
) { \
	return std::make_tuple( \
		(void*) variable.data(), \
		variable.size(), \
		GIVEN_MPI_TYPE \
	); \
}

GENSIMCELL_GET_VECTOR_VAR_MPI_DATATYPE(char, MPI_CHAR)
GENSIMCELL_GET_VECTOR_VAR_MPI_DATATYPE(double, MPI_DOUBLE)
GENSIMCELL_GET_VECTOR_VAR_MPI_DATATYPE(float, MPI_FLOAT)
GENSIMCELL_GET_VECTOR_VAR_MPI_DATATYPE(long double, MPI_LONG_DOUBLE)
GENSIMCELL_GET_VECTOR_VAR_MPI_DATATYPE(signed char, MPI_SIGNED_CHAR)
GENSIMCELL_GET_VECTOR_VAR_MPI_DATATYPE(signed int, MPI_INT)
GENSIMCELL_GET_VECTOR_VAR_MPI_DATATYPE(signed long int, MPI_LONG)
GENSIMCELL_GET_VECTOR_VAR_MPI_DATATYPE(signed long long int, MPI_LONG_LONG_INT)
GENSIMCELL_GET_VECTOR_VAR_MPI_DATATYPE(signed short int, MPI_SHORT)
GENSIMCELL_GET_VECTOR_VAR_MPI_DATATYPE(unsigned char, MPI_UNSIGNED_CHAR)
GENSIMCELL_GET_VECTOR_VAR_MPI_DATATYPE(unsigned int, MPI_UNSIGNED)
GENSIMCELL_GET_VECTOR_VAR_MPI_DATATYPE(unsigned long int, MPI_UNSIGNED_LONG)
GENSIMCELL_GET_VECTOR_VAR_MPI_DATATYPE(unsigned long long int, MPI_UNSIGNED_LONG_LONG)
GENSIMCELL_GET_VECTOR_VAR_MPI_DATATYPE(unsigned short int, MPI_UNSIGNED_SHORT)
GENSIMCELL_GET_VECTOR_VAR_MPI_DATATYPE(wchar_t, MPI_WCHAR)

#ifdef MPI_CXX_FLOAT_COMPLEX
GENSIMCELL_GET_VECTOR_VAR_MPI_DATATYPE(std::complex<float>, MPI_CXX_FLOAT_COMPLEX)
#endif

#ifdef MPI_CXX_DOUBLE_COMPLEX
GENSIMCELL_GET_VECTOR_VAR_MPI_DATATYPE(std::complex<double>, MPI_CXX_DOUBLE_COMPLEX)
#endif

#ifdef MPI_CXX_LONG_DOUBLE_COMPLEX
GENSIMCELL_GET_VECTOR_VAR_MPI_DATATYPE(std::complex<long double>, MPI_CXX_LONG_DOUBLE_COMPLEX)
#endif

#undef GENSIMCELL_GET_VECTOR_VAR_MPI_DATATYPE



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



// forward declarations to support nested containers
template <
	class... T
> std::tuple<void*, int, MPI_Datatype> get_var_mpi_datatype(
	const std::vector<T...>& variables
);
template <
	class... Types
> std::tuple<void*, int, MPI_Datatype> get_var_mpi_datatype(
	const std::tuple<Types...>&
);
template<
	class T1, class T2
> std::tuple<void*, int, MPI_Datatype> get_var_mpi_datatype(
	const std::pair<T1, T2>&
);



/*!
Version for an empty array.

Returns nullptr, 0 and MPI_BYTE.
*/
template <
	class T
> std::tuple<
	void*,
	int,
	MPI_Datatype
> get_var_mpi_datatype(
	const std::array<T, 0>&
) {
	return std::make_tuple(nullptr, 0, MPI_BYTE);
}

/*!
Version for an array of size 1.

Returns the result of calling get_var_mpi_datatype on the inner type.
*/
template <
	class T
> std::tuple<
	void*,
	int,
	MPI_Datatype
> get_var_mpi_datatype(
	const std::array<T, 1>& variable
) {
	return get_var_mpi_datatype(variable[0]);
}

/*!
Returns transfer info for an array of items.

Works for items supported by get_var_mpi_datatype().

Skips items whose count == 0.
If one item has count > 0 then transfer info of that item
is returned, otherwise a structured datatype is returned with
count == 1.

Returns negative count and MPI_DATATYPE_NULL in case of error.
*/
template <
	class Inner_T,
	std::size_t Number_Of_Items
> std::tuple<
	void*,
	int,
	MPI_Datatype
> get_var_mpi_datatype(
	const std::array<Inner_T, Number_Of_Items>& variables
) {
	static_assert(Number_Of_Items > 1, "Internal error.");

	std::array<void*, Number_Of_Items> addresses{{nullptr}};
	std::array<int, Number_Of_Items> counts{{0}};
	std::array<MPI_Datatype, Number_Of_Items> datatypes{{MPI_BYTE}};

	// skip cells with nothing to transfer
	size_t items_to_transfer = 0;
	for (size_t i = 0; i < Number_Of_Items; i++) {
		std::tie(
			addresses[items_to_transfer],
			counts[items_to_transfer],
			datatypes[items_to_transfer]
		) = get_var_mpi_datatype(variables[i]);

		// assume 0 count means don't transfer
		if (counts[items_to_transfer] > 0) {
			items_to_transfer++;
		}
	}

	if (items_to_transfer == 0) {
		return std::make_tuple(nullptr, 0, MPI_BYTE);
	}

	if (items_to_transfer == 1) {
		return std::make_tuple(addresses[0], counts[0], datatypes[0]);
	}

	if (items_to_transfer > std::numeric_limits<int>::max()) {
		return std::make_tuple(nullptr, -1, MPI_DATATYPE_NULL);
	}

	std::array<MPI_Aint, Number_Of_Items> displacements{{0}};
	for (size_t i = 0; i < items_to_transfer; i++) {
		displacements[i]
			= static_cast<char*>(addresses[i])
			- static_cast<char*>(addresses[0]);
	}

	MPI_Datatype final_datatype = MPI_DATATYPE_NULL;
	if (
		MPI_Type_create_struct(
			int(items_to_transfer),
			counts.data(),
			displacements.data(),
			datatypes.data(),
			&final_datatype
		) != MPI_SUCCESS
	) {
		return std::make_tuple(nullptr, -2, MPI_DATATYPE_NULL);
	}

	// free component datatypes
	for (size_t i = 0; i < items_to_transfer; i++) {
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



/*!
Returns transfer info for a vector of items with get_mpi_datatype().

Works for items supported by get_var_mpi_datatype().

Skips items whose count == 0.
If one item has count > 0 then transfer info of that item
is returned, otherwise a structured datatype is returned with
count == 1.

Returns negative count and MPI_DATATYPE_NULL in case of error.
*/
template <
	class... T
> std::tuple<
	void*,
	int,
	MPI_Datatype
> get_var_mpi_datatype(
	const std::vector<T...>& variables
) {
	const auto size = variables.size();

	if (size == 0) {
		return std::make_tuple(nullptr, 0, MPI_BYTE);
	}

	if (size == 1) {
		return get_var_mpi_datatype(variables[0]);
	}


	std::vector<void*> addresses(size, nullptr);
	std::vector<int> counts(size, 0);
	std::vector<MPI_Datatype> datatypes(size, MPI_BYTE);

	// skip cells with nothing to transfer
	size_t items_to_transfer = 0;
	for (size_t i = 0; i < size; i++) {
		std::tie(
			addresses[items_to_transfer],
			counts[items_to_transfer],
			datatypes[items_to_transfer]
		) = get_var_mpi_datatype(variables[i]);

		// assume 0 count means don't transfer
		if (counts[items_to_transfer] > 0) {
			items_to_transfer++;
		}
	}

	if (items_to_transfer == 0) {
		return std::make_tuple(nullptr, 0, MPI_BYTE);
	}

	if (items_to_transfer == 1) {
		return std::make_tuple(addresses[0], counts[0], datatypes[0]);
	}

	if (items_to_transfer > std::numeric_limits<int>::max()) {
		return std::make_tuple(nullptr, -1, MPI_DATATYPE_NULL);
	}


	std::vector<MPI_Aint> displacements(items_to_transfer, 0);
	for (size_t i = 0; i < items_to_transfer; i++) {
		displacements[i]
			= static_cast<char*>(addresses[i])
			- static_cast<char*>(addresses[0]);
	}

	MPI_Datatype final_datatype = MPI_DATATYPE_NULL;
	if (
		MPI_Type_create_struct(
			int(items_to_transfer),
			counts.data(),
			displacements.data(),
			datatypes.data(),
			&final_datatype
		) != MPI_SUCCESS
	) {
		return std::make_tuple(nullptr, -2, MPI_DATATYPE_NULL);
	}

	// free component datatypes
	for (size_t i = 0; i < items_to_transfer; i++) {
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



/*!
Helper class for get_var_mpi_datatype().

Fills the transfer info of type at index 0 in given tuple.
*/
template <
	std::size_t Index,
	class... Types
> typename std::enable_if<Index == 0, void>::type get_var_mpi_datatype_tuple(
	const std::tuple<Types...>& variables,
	std::array<void*, sizeof...(Types)>& addresses,
	std::array<int, sizeof...(Types)>& counts,
	std::array<MPI_Datatype, sizeof...(Types)>& datatypes
) {
	std::tie(addresses[0], counts[0], datatypes[0])
		= get_var_mpi_datatype(std::get<0>(variables));
}

/*!
See the enable_if<Index == 0... version.
*/
template <
	std::size_t Index,
	class... Types
> typename std::enable_if<Index != 0, void>::type get_var_mpi_datatype_tuple(
	const std::tuple<Types...>& variables,
	std::array<void*, sizeof...(Types)>& addresses,
	std::array<int, sizeof...(Types)>& counts,
	std::array<MPI_Datatype, sizeof...(Types)>& datatypes
) {
	std::tie(addresses[Index], counts[Index], datatypes[Index])
		= get_var_mpi_datatype(std::get<Index>(variables));

	get_var_mpi_datatype_tuple<Index - 1>(
		variables,
		addresses,
		counts,
		datatypes
	);
}



/*!
Version for an empty tuple.

Returns nullptr, 0 and MPI_BYTE.
*/
std::tuple<
	void*,
	int,
	MPI_Datatype
> get_var_mpi_datatype(
	const std::tuple<>&
) {
	return std::make_tuple(nullptr, 0, MPI_BYTE);
}

/*!
Version for a tuple with one item

Returns the result of calling get_var_mpi_datatype on the inner type.
*/
template <
	class Type
> std::tuple<
	void*,
	int,
	MPI_Datatype
> get_var_mpi_datatype(
	const std::tuple<Type>& variables
) {
	return get_var_mpi_datatype(std::get<0>(variables));
}

/*!
Specialization of get_var_mpi_datatype for a tuple of items.

Works for items supported by get_var_mpi_datatype().

Returns a structured datatype and count = 1.
Returns negative count and MPI_DATATYPE_NULL in case of error.
*/
template <
	class... Types
> std::tuple<
	void*,
	int,
	MPI_Datatype
> get_var_mpi_datatype(
	const std::tuple<Types...>& variables
) {
	constexpr size_t nr_of_items = sizeof...(Types);
	std::array<void*, nr_of_items> addresses;
	std::array<int, nr_of_items> counts;
	std::array<MPI_Datatype, nr_of_items> datatypes;

	get_var_mpi_datatype_tuple<nr_of_items - 1>(
		variables,
		addresses,
		counts,
		datatypes
	);

	std::vector<MPI_Aint> displacements(nr_of_items, 0);
	for (size_t i = 0; i < nr_of_items; i++) {
		displacements[i]
			= static_cast<char*>(addresses[i])
			- static_cast<char*>(addresses[0]);
	}

	MPI_Datatype final_datatype = MPI_DATATYPE_NULL;
	if (
		MPI_Type_create_struct(
			int(nr_of_items),
			counts.data(),
			displacements.data(),
			datatypes.data(),
			&final_datatype
		) != MPI_SUCCESS
	) {
		return std::make_tuple(nullptr, -1, MPI_DATATYPE_NULL);
	}

	// free component datatypes
	for (size_t i = 0; i < nr_of_items; i++) {
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


/*!
Specialization of get_var_mpi_datatype for a pair of items.

Works for items supported by get_var_mpi_datatype().

Skips items whose count == 0, if all are skippt then returns
nullptr, 0 and MPI_BYTE.
If one item has count > 0 then transfer info of that item
is returned, otherwise a structured datatype is returned with
count == 1.

Returns negative count and MPI_DATATYPE_NULL in case of error.
*/
template <
	class First,
	class Second
> std::tuple<
	void*,
	int,
	MPI_Datatype
> get_var_mpi_datatype(
	const std::pair<First, Second>& variables
) {
	std::array<void*, 2> addresses;
	std::array<int, 2> counts;
	std::array<MPI_Datatype, 2> datatypes;

	std::tie(addresses[0], counts[0], datatypes[0])
		= get_var_mpi_datatype(variables.first);
	std::tie(addresses[1], counts[1], datatypes[1])
		= get_var_mpi_datatype(variables.second);

	if (counts[0] <= 0 and counts[1] <= 0) {
		return std::make_tuple(nullptr, 0, MPI_BYTE);
	} else if (counts[0] <= 0) {
		return std::make_tuple(addresses[1], counts[1], datatypes[1]);
	} else if (counts[1] <= 0) {
		return std::make_tuple(addresses[0], counts[0], datatypes[0]);
	}

	std::vector<MPI_Aint> displacements{
		0,
		static_cast<char*>(addresses[1]) - static_cast<char*>(addresses[0])
	};

	MPI_Datatype final_datatype = MPI_DATATYPE_NULL;
	if (
		MPI_Type_create_struct(
			int(2),
			counts.data(),
			displacements.data(),
			datatypes.data(),
			&final_datatype
		) != MPI_SUCCESS
	) {
		return std::make_tuple(nullptr, -1, MPI_DATATYPE_NULL);
	}

	// free component datatypes
	for (size_t i = 0; i < 2; i++) {
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


} // namespace detail
} // namespace gensimcell

#endif // ifdef MPI_VERSION

#endif // ifndef  GET_VAR_DATATYPE_HPP
