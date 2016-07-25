/*
Example showing how to allow gencimcell to transfer a custom variable.

Copyright 2015, 2016 Ilja Honkonen
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
#include "vector"

#include "boost/logic/tribool.hpp"
#include "mpi.h"

#include "check_true.hpp"
#include "gensimcell.hpp"

using namespace std;

struct Custom_Variable {

	/*
	gensimcell knows how to transfer
	std::tuple<char, int, double> but
	for the sake of an example:
	*/
	struct Custom_Variable_Storage {
		char c;
		int i;
		double d;

		std::tuple<
			void*,
			int,
			MPI_Datatype
		> get_mpi_datatype() const {

			std::array<int, 3> counts{{1, 1, 1}};
			std::array<MPI_Aint, 3> displacements{{
				0,
				(char*) &(this->i) - (char*) &(this->c),
				(char*) &(this->d) - (char*) &(this->c)
			}};
			std::array<MPI_Datatype, 3> datatypes{{
				MPI_CHAR, MPI_INT, MPI_DOUBLE
			}};

			MPI_Datatype final_datatype;
			if (
				MPI_Type_create_struct(
					int(counts.size()),
					counts.data(),
					displacements.data(),
					datatypes.data(),
					&final_datatype
				) != MPI_SUCCESS
			) {
				return std::make_tuple(nullptr, -1, MPI_DATATYPE_NULL);
			}

			return std::make_tuple((void*) &(this->c), 1, final_datatype);
		}
	};

	using data_type = Custom_Variable_Storage;
};


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
		cerr << "This test must be run with at least 2 processes" << endl;
		abort();
	}

	const Custom_Variable v{};

	gensimcell::Cell<gensimcell::Optional_Transfer, Custom_Variable> c;

	void* address = NULL;
	int count = -1;
	MPI_Datatype datatype = MPI_DATATYPE_NULL;

	if (rank == 0) {

		c.set_transfer_all(true, v);
		c[v].c = 'a';
		c[v].i = 1;
		c[v].d = 3.3;
		std::tie(address, count, datatype) = c.get_mpi_datatype();
		if (count != 1) {
			PRINT_ERROR(rank, "Couldn't get datatype from c.")
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
			PRINT_ERROR(rank, "Couldn't send c to process 1.")
			abort();
		}
		MPI_Type_free(&datatype);
		CHECK_TRUE(
			c[v].c == 'a'
			and c[v].i == 1
			and c[v].d == 3.3
		)

	} else if (rank == 1) {

		c.set_transfer_all(true, v);
		c[v].c = 'b';
		c[v].i = -1;
		c[v].d = -3.3;
		std::tie(address, count, datatype) = c.get_mpi_datatype();
		if (count != 1) {
			PRINT_ERROR(rank, "Couldn't get datatype from c.")
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
			PRINT_ERROR(rank, "Couldn't receive c from process 0.")
			abort();
		}
		MPI_Type_free(&datatype);
		CHECK_TRUE(
			c[v].c == 'a'
			and c[v].i == 1
			and c[v].d == 3.3
		)

	}


	MPI_Finalize();

	return EXIT_SUCCESS;
}
