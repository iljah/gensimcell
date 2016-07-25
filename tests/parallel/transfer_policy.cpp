/*
Tests transfer policy logic.

Copyright 2014, 2015, 2016 Ilja Honkonen
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

#include "boost/logic/tribool.hpp"
#include "cstdlib"
#include "iostream"
#include "mpi.h"
#include "vector"

#include "check_true.hpp"
#include "gensimcell.hpp"

using namespace std;

struct test_variable1 {
	using data_type = int;
};

using cell1_t = gensimcell::Cell<
	gensimcell::Never_Transfer,
	test_variable1
>;

struct test_variable2 {
	using data_type = int;
};

using cell2_t = gensimcell::Cell<
	gensimcell::Always_Transfer,
	test_variable2
>;


#define PRINT_ERROR(rank, msg) \
std::cerr \
	<< __FILE__ << ":" << __LINE__ \
	<< " Process " << rank << ": " msg \
	<< std::endl;


int main(int argc, char* argv[])
{
	if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
		cerr << "Couldn't initialize MPI." << endl;
		abort();
	}

	MPI_Comm comm = MPI_COMM_WORLD;

	int rank = 0, comm_size = 0;
	MPI_Comm_rank(comm, &rank);
	MPI_Comm_size(comm, &comm_size);

	if (comm_size < 2) {
		cerr << "This test must be run with at least 2 processes." << endl;
		abort();
	}

	void* address = NULL;
	int count = -1;
	MPI_Datatype datatype = MPI_DATATYPE_NULL;

	const test_variable1 v1{};
	const test_variable2 v2{};

	cell1_t c1;
	cell2_t c2;

	cell1_t::set_transfer_all(true, v1);
	c1.set_transfer(true, v1);

	cell2_t::set_transfer_all(false, v1);
	c2.set_transfer(false, v1);

	if (rank == 0) {
		c1[v1] = 1;
		c2[v2] = 2;

		std::tie(address, count, datatype) = c1.get_mpi_datatype();
		if (count < 0) {
			PRINT_ERROR(rank, "Couldn't get datatype.")
			abort();
		}
		if (
			MPI_Send(
				address,
				count,
				datatype,
				1,
				0,
				comm
			) != MPI_SUCCESS
		) {
			PRINT_ERROR(rank, "Couldn't send.")
			abort();
		}

		std::tie(address, count, datatype) = c2.get_mpi_datatype();
		if (count < 0) {
			PRINT_ERROR(rank, "Couldn't get datatype.")
			abort();
		}
		if (
			MPI_Send(
				address,
				count,
				datatype,
				1,
				0,
				comm
			) != MPI_SUCCESS
		) {
			PRINT_ERROR(rank, "Couldn't send.")
			abort();
		}

	} else if (rank == 1) {
		c1[v1] = -1;
		c2[v2] = -2;

		std::tie(address, count, datatype) = c1.get_mpi_datatype();
		if (count < 0) {
			PRINT_ERROR(rank, "Couldn't get datatype from c2.")
			abort();
		}
		if (
			MPI_Recv(
				address,
				count,
				datatype,
				0,
				0,
				comm,
				MPI_STATUS_IGNORE
			) != MPI_SUCCESS
		) {
			PRINT_ERROR(rank, "Couldn't receive c2 from process 0.")
			abort();
		}

		std::tie(address, count, datatype) = c2.get_mpi_datatype();
		if (count < 0) {
			PRINT_ERROR(rank, "Couldn't get datatype from c2.")
			abort();
		}
		if (
			MPI_Recv(
				address,
				count,
				datatype,
				0,
				0,
				comm,
				MPI_STATUS_IGNORE
			) != MPI_SUCCESS
		) {
			PRINT_ERROR(rank, "Couldn't receive c2 from process 0.")
			abort();
		}

		CHECK_TRUE(c1[v1] == -1)
		CHECK_TRUE(c2[v2] == 2)

	}

	MPI_Finalize();

	return EXIT_SUCCESS;
}
