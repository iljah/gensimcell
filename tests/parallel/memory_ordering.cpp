/*
Tests the ordering of the cell's variables when transferred by MPI.

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


Author(s): Ilja Honkonen
*/

/*!
First variable given as a template argument should
end up as first in memory when transferred.
*/

#include "cstdlib"
#include "iostream"
#include "mpi.h"
#include "tuple"

#include "gensimcell.hpp"

using namespace std;

struct test_variable1 {
	using data_type = char;
};

struct test_variable2 {
	using data_type = char;
};

struct test_variable3 {
	using data_type = char;
};

using cell123_t = gensimcell::Cell<
	gensimcell::Optional_Transfer,
	test_variable1,
	test_variable2,
	test_variable3
>;

using cell321_t = gensimcell::Cell<
	gensimcell::Optional_Transfer,
	test_variable3,
	test_variable2,
	test_variable1
>;


int main(int argc, char* argv[])
{
	using std::cerr;
	using std::endl;

	if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
		cerr << "Couldn't initialize MPI." << endl;
		abort();
	}

	int rank = -1;
	MPI_Comm_rank(MPI_COMM_SELF, &rank);

	void* address = NULL;
	int count = -1;
	MPI_Datatype datatype = MPI_DATATYPE_NULL;

	const test_variable1 v1{};
	const test_variable2 v2{};
	const test_variable3 v3{};

	vector<char> recv_buffer;
	int sizeof_datatype;
	MPI_Request request;



	cell123_t cell123;
	cell123.set_transfer_all(true, v1, v2, v3);
	cell123.set_transfer(true, v1, v2, v3);

	cell123[v1] = 0x11;
	cell123[v2] = 0x12;
	cell123[v3] = 0x13;

	std::tie(address, count, datatype) = cell123.get_mpi_datatype();
	MPI_Type_commit(&datatype);

	MPI_Type_size(datatype, &sizeof_datatype);
	if (sizeof_datatype != 3) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		abort();
	}

	recv_buffer.resize(sizeof_datatype);
	for (auto& c: recv_buffer) {
		c = 0x0F;
	}

	MPI_Irecv(
		recv_buffer.data(),
		sizeof_datatype,
		MPI_BYTE,
		rank,
		0,
		MPI_COMM_SELF,
		&request
	);

	MPI_Send(
		address,
		count,
		datatype,
		rank,
		0,
		MPI_COMM_SELF
	);

	MPI_Wait(&request, MPI_STATUS_IGNORE);
	MPI_Type_free(&datatype);

	if (recv_buffer[0] != 0x11) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		abort();
	}
	if (recv_buffer[1] != 0x12) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		abort();
	}
	if (recv_buffer[2] != 0x13) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		abort();
	}



	cell321_t cell321;
	cell321.set_transfer_all(true, v1, v2, v3);
	cell321.set_transfer(true, v1, v2, v3);

	cell321[v1] = 0x31;
	cell321[v2] = 0x32;
	cell321[v3] = 0x33;

	std::tie(address, count, datatype) = cell321.get_mpi_datatype();
	MPI_Type_commit(&datatype);

	MPI_Type_size(datatype, &sizeof_datatype);
	if (sizeof_datatype != 3) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		abort();
	}

	recv_buffer.resize(sizeof_datatype);
	for (auto& c: recv_buffer) {
		c = 0x0F;
	}

	MPI_Irecv(
		recv_buffer.data(),
		sizeof_datatype,
		MPI_BYTE,
		rank,
		0,
		MPI_COMM_SELF,
		&request
	);

	MPI_Send(
		address,
		count,
		datatype,
		rank,
		0,
		MPI_COMM_SELF
	);

	MPI_Wait(&request, MPI_STATUS_IGNORE);
	MPI_Type_free(&datatype);

	if (recv_buffer[0] != 0x33) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		abort();
	}
	if (recv_buffer[1] != 0x32) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		abort();
	}
	if (recv_buffer[2] != 0x31) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		abort();
	}


	MPI_Finalize();

	return EXIT_SUCCESS;
}
