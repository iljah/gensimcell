/*
Tests that transfer of a cell with one Eigen variable between processes works.

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

#include "boost/logic/tribool.hpp"
#include "cstdlib"
#include "Eigen/Core"
#include "iostream"
#include "mpi.h"
#include "vector"

#include "check_true.hpp"
#include "gensimcell.hpp"

using namespace std;


struct test_variable1 {
	using data_type = Eigen::Matrix3d;
};


#define PRINT_ERROR(rank, msg) \
std::cerr \
	<< __FILE__ << ":" << __LINE__ \
	<< " Process " << rank << ": " msg \
	<< std::endl;


void transfer_c1_v1(MPI_Comm comm, const int rank)
{
	const test_variable1 v1;
	gensimcell::Cell<test_variable1> c1_1;

	void* address = NULL;
	int count = -1;
	MPI_Datatype datatype = MPI_DATATYPE_NULL;

	if (rank == 0) {

		// send v1 from process 0 to 1
		c1_1.set_transfer_all(true, v1);
		c1_1[v1] <<
			1, 2, 3,
			4, 5, 6,
			7, 8, 9;
		std::tie(address, count, datatype) = c1_1.get_mpi_datatype();
		if (count < 0) {
			PRINT_ERROR(rank, "Couldn't get datatype from c1_1.")
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
			PRINT_ERROR(rank, "Couldn't send c1_1 to process 1.")
			abort();
		}
		CHECK_TRUE(c1_1[v1](0, 0) == 1)
		CHECK_TRUE(c1_1[v1](2, 2) == 9)


		// don't send v1
		c1_1.set_transfer_all(false, v1);
		std::tie(address, count, datatype) = c1_1.get_mpi_datatype();
		if (count < 0) {
			PRINT_ERROR(rank, "Couldn't get datatype from c1_1.")
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
			PRINT_ERROR(rank, "Couldn't send c1_1 to process 1.")
			abort();
		}
		CHECK_TRUE(c1_1[v1](0, 0) == 1)
		CHECK_TRUE(c1_1[v1](2, 2) == 9)


		// send v1
		c1_1.set_transfer_all(boost::logic::indeterminate, v1);
		c1_1.set_transfer(true, v1);
		std::tie(address, count, datatype) = c1_1.get_mpi_datatype();
		if (count < 0) {
			PRINT_ERROR(rank, "Couldn't get datatype from c1_1.")
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
			PRINT_ERROR(rank, "Couldn't send c1_1 to process 1.")
			abort();
		}
		CHECK_TRUE(c1_1[v1](0, 0) == 1)
		CHECK_TRUE(c1_1[v1](2, 2) == 9)


		// send v1 from process 1 to 0
		c1_1[v1] <<
			-1, -2, -3,
			-4, -5, -6,
			-7, -8, -9;
		std::tie(address, count, datatype) = c1_1.get_mpi_datatype();
		if (count < 0) {
			PRINT_ERROR(rank, "Couldn't get datatype from c1_1.")
			abort();
		}
		if (
			MPI_Recv(
				address,
				count,
				datatype,
				1,
				0,
				comm,
				MPI_STATUS_IGNORE
			) != MPI_SUCCESS
		) {
			PRINT_ERROR(rank, "Couldn't send c1_1 to process 1.")
			abort();
		}
		CHECK_TRUE(c1_1[v1](0, 0) == 9)
		CHECK_TRUE(c1_1[v1](2, 2) == 1)
		c1_1[v1] <<
			-1, -2, -3,
			-4, -5, -6,
			-7, -8, -9;


	} else if (rank == 1) {

		// comments in the proc == 0 part

		c1_1.set_transfer_all(true, v1);
		c1_1[v1] <<
			-1, -2, -3,
			-4, -5, -6,
			-7, -8, -9;
		std::tie(address, count, datatype) = c1_1.get_mpi_datatype();
		if (count < 0) {
			PRINT_ERROR(rank, "Couldn't get datatype from c1_1.")
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
			PRINT_ERROR(rank, "Couldn't receive c1_1 from process 0.")
			abort();
		}
		CHECK_TRUE(c1_1[v1](0, 0) == 1)
		CHECK_TRUE(c1_1[v1](2, 2) == 9)
		c1_1[v1] <<
			-1, -2, -3,
			-4, -5, -6,
			-7, -8, -9;


		c1_1.set_transfer_all(false, v1);
		std::tie(address, count, datatype) = c1_1.get_mpi_datatype();
		if (count < 0) {
			PRINT_ERROR(rank, "Couldn't get datatype from c1_1.")
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
			PRINT_ERROR(rank, "Couldn't receive c1_1 from process 0.")
			abort();
		}
		CHECK_TRUE(c1_1[v1](0, 0) == -1)
		CHECK_TRUE(c1_1[v1](2, 2) == -9)


		c1_1.set_transfer_all(boost::logic::indeterminate, v1);
		c1_1.set_transfer(true, v1);
		std::tie(address, count, datatype) = c1_1.get_mpi_datatype();
		if (count < 0) {
			PRINT_ERROR(rank, "Couldn't get datatype from c1_1.")
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
			PRINT_ERROR(rank, "Couldn't receive c1_1 from process 0.")
			abort();
		}
		CHECK_TRUE(c1_1[v1](0, 0) == 1)
		CHECK_TRUE(c1_1[v1](2, 2) == 9)
		c1_1[v1] <<
			-1, -2, -3,
			-4, -5, -6,
			-7, -8, -9;


		c1_1[v1] <<
			9, 8, 7,
			6, 5, 4,
			3, 2, 1;
		std::tie(address, count, datatype) = c1_1.get_mpi_datatype();
		if (count < 0) {
			PRINT_ERROR(rank, "Couldn't get datatype from c1_1.")
			abort();
		}
		if (
			MPI_Send(
				address,
				count,
				datatype,
				0,
				0,
				comm
			) != MPI_SUCCESS
		) {
			PRINT_ERROR(rank, "Couldn't receive c1_1 from process 0.")
			abort();
		}
		CHECK_TRUE(c1_1[v1](0, 0) == 9)
		CHECK_TRUE(c1_1[v1](2, 2) == 1)
		c1_1[v1] <<
			-1, -2, -3,
			-4, -5, -6,
			-7, -8, -9;
	}
}

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
		cerr << "This test must be run with at least 2 processes" << endl;
		abort();
	}

	transfer_c1_v1(comm, rank);

	MPI_Finalize();

	return EXIT_SUCCESS;
}
