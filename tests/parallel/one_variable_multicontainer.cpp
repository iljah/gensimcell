/*
Tests that transfer of one multicontainer variable between processes works.

Copyright 2015 Ilja Honkonen
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
#include "boost/logic/tribool.hpp"
#include "cstdlib"
#include "iostream"
#include "tuple"
#include "vector"

#include "mpi.h"

#include "check_true.hpp"
#include "gensimcell.hpp"

using namespace std;

struct test_variable1 {
	using data_type = std::vector<int>;
};

struct test_variable2 {
	using data_type = std::tuple<double, char>;
};

struct test_variable3 {
	using data_type
		= std::array<
			std::pair<long double, char>,
			2
		>;
};

struct test_variable4 {
	using data_type
		= std::vector<
			std::array<
				std::tuple<
					long double,
					char,
					unsigned long long int
				>,
				2
			>
		>;
};


#define PRINT_ERROR(rank, msg) \
std::cerr \
	<< __FILE__ << ":" << __LINE__ \
	<< " Process " << rank << ": " msg \
	<< std::endl;


int main(int argc, char* argv[])
{
	using std::get;

	constexpr test_variable1 v1{};
	constexpr test_variable2 v2{};
	constexpr test_variable3 v3{};
	constexpr test_variable4 v4{};

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

	void* address = NULL;
	int count = -1;
	MPI_Datatype datatype = MPI_DATATYPE_NULL;

	gensimcell::Cell<gensimcell::Always_Transfer, test_variable1> c1;
	gensimcell::Cell<gensimcell::Always_Transfer, test_variable2> c2;
	gensimcell::Cell<gensimcell::Always_Transfer, test_variable3> c3;
	gensimcell::Cell<gensimcell::Always_Transfer, test_variable4> c4;

	if (rank == 0) {

		c1[v1].insert(c1[v1].begin(), {-1, 1, -2, 2, -3, 3});
		std::tie(address, count, datatype) = c1.get_mpi_datatype();
		if (address != c1[v1].data()) {
			PRINT_ERROR(rank, "Wrong address from c1.")
			abort();
		}
		if (count != 6) {
			PRINT_ERROR(rank, "Wrong count from c1.")
			abort();
		}
		if (datatype != MPI_INT) {
			PRINT_ERROR(rank, "Wrong datatype from c1.")
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
			PRINT_ERROR(rank, "Couldn't send c1 to process 1.")
			abort();
		}
		CHECK_TRUE(
			c1[v1].size() == 6
			and c1[v1][5] == 3
		)

		get<0>(c2[v2]) = 3.0;
		get<1>(c2[v2]) = 'n';
		std::tie(address, count, datatype) = c2.get_mpi_datatype();
		if (address != &(get<0>(c2[v2]))) {
			PRINT_ERROR(rank, "Wrong address from c2.")
			abort();
		}
		if (count != 1) {
			PRINT_ERROR(rank, "Wrong count from c2.")
			abort();
		}
		MPI_Type_commit(&datatype);
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
			PRINT_ERROR(rank, "Couldn't send c2 to process 1.")
			abort();
		}
		MPI_Type_free(&datatype);
		CHECK_TRUE(
			get<0>(c2[v2]) == 3.0
			and get<1>(c2[v2]) == 'n'
		)

		get<0>(c3[v3][0]) = 4.0;
		get<1>(c3[v3][0]) = 'n';
		get<0>(c3[v3][1]) = -4.0;
		get<1>(c3[v3][1]) = 'a';
		std::tie(address, count, datatype) = c3.get_mpi_datatype();
		if (address != &(get<0>(c3[v3][0]))) {
			PRINT_ERROR(rank, "Wrong address from c3.")
			abort();
		}
		if (count != 1) {
			PRINT_ERROR(rank, "Wrong count from c3.")
			abort();
		}
		MPI_Type_commit(&datatype);
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
			PRINT_ERROR(rank, "Couldn't send c3 to process 1.")
			abort();
		}
		MPI_Type_free(&datatype);
		CHECK_TRUE(
			get<0>(c3[v3][0]) == 4.0
			and get<1>(c3[v3][0]) == 'n'
			and get<0>(c3[v3][1]) == -4.0
			and get<1>(c3[v3][1]) == 'a'
		)

		c4[v4].resize(2);
		get<0>(c4[v4][0][0]) = -5.0;
		get<1>(c4[v4][0][0]) = 'n';
		get<2>(c4[v4][0][0]) = 5;
		get<0>(c4[v4][0][1]) = -6.0;
		get<1>(c4[v4][0][1]) = 'a';
		get<2>(c4[v4][0][1]) = 6;
		get<0>(c4[v4][1][0]) = -7.0;
		get<1>(c4[v4][1][0]) = 's';
		get<2>(c4[v4][1][0]) = 7;
		get<0>(c4[v4][1][1]) = -8.0;
		get<1>(c4[v4][1][1]) = 'a';
		get<2>(c4[v4][1][1]) = 8;
		std::tie(address, count, datatype) = c4.get_mpi_datatype();
		if (address != &(get<0>(c4[v4][0][0]))) {
			PRINT_ERROR(rank, "Wrong address from c4.")
			abort();
		}
		if (count != 1) {
			PRINT_ERROR(rank, "Wrong count from c4.")
			abort();
		}
		MPI_Type_commit(&datatype);
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
			PRINT_ERROR(rank, "Couldn't send c4 to process 1.")
			abort();
		}
		MPI_Type_free(&datatype);
		CHECK_TRUE(
			get<0>(c4[v4][0][0]) == -5.0
			and get<1>(c4[v4][0][0]) == 'n'
			and get<2>(c4[v4][0][0]) == 5
			and get<0>(c4[v4][0][1]) == -6.0
			and get<1>(c4[v4][0][1]) == 'a'
			and get<2>(c4[v4][0][1]) == 6
			and get<0>(c4[v4][1][0]) == -7.0
			and get<1>(c4[v4][1][0]) == 's'
			and get<2>(c4[v4][1][0]) == 7
			and get<0>(c4[v4][1][1]) == -8.0
			and get<1>(c4[v4][1][1]) == 'a'
			and get<2>(c4[v4][1][1]) == 8
		)

	} else if (rank == 1) {

		// comments in the proc == 0 part

		c1[v1].resize(6, -5);
		std::tie(address, count, datatype) = c1.get_mpi_datatype();
		if (address != c1[v1].data()) {
			PRINT_ERROR(rank, "Wrong address from c1.")
			abort();
		}
		if (count != 6) {
			PRINT_ERROR(rank, "Wrong count from c1.")
			abort();
		}
		if (datatype != MPI_INT) {
			PRINT_ERROR(rank, "Wrong datatype from c1.")
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
			PRINT_ERROR(rank, "Couldn't receive c1 from process 0.")
			abort();
		}
		CHECK_TRUE(
			c1[v1].size() == 6
			and c1[v1][0] == -1
			and c1[v1][5] == 3
		)

		get<0>(c2[v2]) = -3.0;
		get<1>(c2[v2]) = 'a';
		std::tie(address, count, datatype) = c2.get_mpi_datatype();
		if (address != &(get<0>(c2[v2]))) {
			PRINT_ERROR(rank, "Wrong address from c2.")
			abort();
		}
		if (count != 1) {
			PRINT_ERROR(rank, "Wrong count from c2.")
			abort();
		}
		MPI_Type_commit(&datatype);
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
		MPI_Type_free(&datatype);
		CHECK_TRUE(
			get<0>(c2[v2]) == 3.0
			and get<1>(c2[v2]) == 'n'
		)

		get<0>(c3[v3][0]) = -4.0;
		get<1>(c3[v3][0]) = 'a';
		get<0>(c3[v3][1]) = 4.0;
		get<1>(c3[v3][1]) = 's';
		std::tie(address, count, datatype) = c3.get_mpi_datatype();
		if (address != &(get<0>(c3[v3][0]))) {
			PRINT_ERROR(rank, "Wrong address from c3.")
			abort();
		}
		if (count != 1) {
			PRINT_ERROR(rank, "Wrong count from c3.")
			abort();
		}
		MPI_Type_commit(&datatype);
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
			PRINT_ERROR(rank, "Couldn't receive c3 from process 0.")
			abort();
		}
		MPI_Type_free(&datatype);
		CHECK_TRUE(
			get<0>(c3[v3][0]) == 4.0)
		CHECK_TRUE(
			 get<1>(c3[v3][0]) == 'n')
		CHECK_TRUE(
			 get<0>(c3[v3][1]) == -4.0)
		CHECK_TRUE(
			 get<1>(c3[v3][1]) == 'a'
		)

		c4[v4].resize(2);
		get<0>(c4[v4][0][0]) = -8.0;
		get<1>(c4[v4][0][0]) = 'a';
		get<2>(c4[v4][0][0]) = 8;
		get<0>(c4[v4][0][1]) = -7.0;
		get<1>(c4[v4][0][1]) = 's';
		get<2>(c4[v4][0][1]) = 7;
		get<0>(c4[v4][1][0]) = -6.0;
		get<1>(c4[v4][1][0]) = 'a';
		get<2>(c4[v4][1][0]) = 6;
		get<0>(c4[v4][1][1]) = -5.0;
		get<1>(c4[v4][1][1]) = 'n';
		get<2>(c4[v4][1][1]) = 5;
		std::tie(address, count, datatype) = c4.get_mpi_datatype();
		if (address != &(get<0>(c4[v4][0][0]))) {
			PRINT_ERROR(rank, "Wrong address from c4.")
			abort();
		}
		if (count != 1) {
			PRINT_ERROR(rank, "Wrong count from c4.")
			abort();
		}
		MPI_Type_commit(&datatype);
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
			PRINT_ERROR(rank, "Couldn't receive c4 from process 0.")
			abort();
		}
		MPI_Type_free(&datatype);
		CHECK_TRUE(
			get<0>(c4[v4][0][0]) == -5.0
			and get<1>(c4[v4][0][0]) == 'n'
			and get<2>(c4[v4][0][0]) == 5
			and get<0>(c4[v4][0][1]) == -6.0
			and get<1>(c4[v4][0][1]) == 'a'
			and get<2>(c4[v4][0][1]) == 6
			and get<0>(c4[v4][1][0]) == -7.0
			and get<1>(c4[v4][1][0]) == 's'
			and get<2>(c4[v4][1][0]) == 7
			and get<0>(c4[v4][1][1]) == -8.0
			and get<1>(c4[v4][1][1]) == 'a'
			and get<2>(c4[v4][1][1]) == 8
		)
	}

	MPI_Finalize();

	return EXIT_SUCCESS;
}
