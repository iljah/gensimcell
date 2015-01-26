/*
Tests get_var_mpi_datatype for an array of generic simulation cells.

Copyright 2014, 2015 Ilja Honkonen
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

#include "array"
#include "cstdlib"
#include "iostream"
#include "mpi.h"

#include "check_true.hpp"
#include "gensimcell.hpp"


struct test_variable1 {
	using data_type = int;
};

struct test_variable2 {
	using data_type = float;
};

struct test_variable3 {
	using data_type = char;
};


using cell1_t = gensimcell::Cell<
	gensimcell::Never_Transfer,
	test_variable1
>;

using cell2_t = gensimcell::Cell<
	gensimcell::Always_Transfer,
	test_variable1,
	test_variable2
>;

using cell3_t = gensimcell::Cell<
	gensimcell::Optional_Transfer,
	test_variable1,
	test_variable2,
	test_variable3
>;



int main(int argc, char* argv[])
{
	const test_variable1 v1{};
	const test_variable2 v2{};
	const test_variable3 v3{};

	if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
		std::cerr << "Couldn't initialize MPI." << std::endl;
		abort();
	}


	void* address = nullptr;
	int count = -1, size_in_bytes = -1;
	MPI_Datatype datatype = MPI_DATATYPE_NULL;

	// cell1_t
	std::array<cell1_t, 0> a1_0;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(a1_0);
	CHECK_TRUE(
		address == nullptr
		and count == 0
		and datatype == MPI_BYTE
	)


	std::array<cell1_t, 1> a1_1;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(a1_1);
	CHECK_TRUE(
		address == nullptr
		and count == 0
		and datatype == MPI_BYTE
	)


	std::array<cell1_t, 2> a1_2;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(a1_2);
	CHECK_TRUE(
		address == nullptr
		and count == 0
		and datatype == MPI_BYTE
	)


	// cell2_t
	std::array<cell2_t, 0> a2_0;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(a2_0);
	CHECK_TRUE(
		address == nullptr
		and count == 0
		and datatype == MPI_BYTE
	)


	std::array<cell2_t, 1> a2_1;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(a2_1);
	MPI_Type_size(datatype, &size_in_bytes);
	CHECK_TRUE(
		address == &(a2_1[0][v1])
		and count == 1
		and size_in_bytes
			== sizeof(test_variable1::data_type)
			+ sizeof(test_variable2::data_type)
	)


	std::array<cell2_t, 2> a2_2;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(a2_2);
	MPI_Type_size(datatype, &size_in_bytes);
	CHECK_TRUE(
		address == &(a2_2[0][v1])
		and count == 1
		and size_in_bytes
			== 2 * sizeof(test_variable1::data_type)
			+ 2 * sizeof(test_variable2::data_type)
	)


	// cell3_t
	std::array<cell3_t, 0> a3_0;

	cell3_t::set_transfer_all(false, v1, v2, v3);
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(a3_0);
	CHECK_TRUE(
		address == nullptr
		and count == 0
		and datatype == MPI_BYTE
	)

	cell3_t::set_transfer_all(true, v2);
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(a3_0);
	CHECK_TRUE(
		address == nullptr
		and count == 0
		and datatype == MPI_BYTE
	)

	cell3_t::set_transfer_all(true, v1, v3);
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(a3_0);
	CHECK_TRUE(
		address == nullptr
		and count == 0
		and datatype == MPI_BYTE
	)


	std::array<cell3_t, 1> a3_1;

	cell3_t::set_transfer_all(false, v1, v2, v3);
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(a3_1);
	CHECK_TRUE(
		address == nullptr
		and count == 0
		and datatype == MPI_BYTE
	)

	cell3_t::set_transfer_all(true, v1);
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(a3_1);
	CHECK_TRUE(
		address == &(a3_1[0][v1])
		and count == 1
		and datatype == MPI_INT
	)

	cell3_t::set_transfer_all(false, v1);
	cell3_t::set_transfer_all(true, v2);
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(a3_1);
	CHECK_TRUE(
		address == &(a3_1[0][v2])
		and count == 1
		and datatype == MPI_FLOAT
	)

	cell3_t::set_transfer_all(false, v2);
	cell3_t::set_transfer_all(true, v3);
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(a3_1);
	CHECK_TRUE(
		address == &(a3_1[0][v3])
		and count == 1
		and datatype == MPI_CHAR
	)


	std::array<cell3_t, 2> a3_2;

	cell3_t::set_transfer_all(false, v1, v2, v3);
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(a3_2);
	CHECK_TRUE(
		address == nullptr
		and count == 0
		and datatype == MPI_BYTE
	)

	cell3_t::set_transfer_all(true, v1);
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(a3_2);
	MPI_Type_size(datatype, &size_in_bytes);
	CHECK_TRUE(
		address == &(a3_2[0][v1])
		and count == 1
		and size_in_bytes == 2 * sizeof(test_variable1::data_type)
	)

	cell3_t::set_transfer_all(false, v1);
	cell3_t::set_transfer_all(true, v2, v3);
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(a3_2);
	MPI_Type_size(datatype, &size_in_bytes);
	CHECK_TRUE(
		address == &(a3_2[0][v2])
		and count == 1
		and size_in_bytes
			== 2 * sizeof(test_variable2::data_type)
			+ 2 * sizeof(test_variable3::data_type)
	)

	cell3_t::set_transfer_all(true, v2);
	cell3_t::set_transfer_all(boost::logic::indeterminate, v1, v3);
	a3_2[0].set_transfer(true, v3);
	a3_2[0].set_transfer(false, v1);
	a3_2[1].set_transfer(true, v1);
	a3_2[1].set_transfer(false, v3);
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(a3_2);
	MPI_Type_size(datatype, &size_in_bytes);
	CHECK_TRUE(
		address == &(a3_2[0][v2])
		and count == 1
		and size_in_bytes
			== 2 * sizeof(test_variable2::data_type)
			+ sizeof(test_variable1::data_type)
			+ sizeof(test_variable3::data_type)
	)

	cell3_t::set_transfer_all(boost::logic::indeterminate, v1, v2, v3);
	a3_2[0].set_transfer(false, v1, v2, v3);
	a3_2[1].set_transfer(true, v2, v3);
	a3_2[1].set_transfer(false, v1);
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(a3_2);
	MPI_Type_size(datatype, &size_in_bytes);
	CHECK_TRUE(
		address == &(a3_2[1][v2])
		and count == 1
		and size_in_bytes
			== sizeof(test_variable2::data_type)
			+ sizeof(test_variable3::data_type)
	)


	MPI_Finalize();

	return EXIT_SUCCESS;
}
