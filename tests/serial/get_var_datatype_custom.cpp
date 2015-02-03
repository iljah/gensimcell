/*
Tests that get_var_mpi_datatype can be customized non-intrusively.

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
#include "complex"
#include "cstdlib"
#include "iostream"
#include "tuple"
#include "vector"

#include "mpi.h"

#include "check_true.hpp"
#include "get_var_mpi_datatype.hpp"

using namespace std;


struct Custom1 {
	int data;
};

namespace gensimcell {
namespace detail {
	std::tuple<
		void*,
		int,
		MPI_Datatype
	> get_var_mpi_datatype(const Custom1& variable) {
		return std::make_tuple((void*) &(variable.data), 1, MPI_INT);
	}
}}


struct Custom2 {
	double x, y, z;
};

namespace gensimcell {
namespace detail {
	std::tuple<
		void*,
		int,
		MPI_Datatype
	> get_var_mpi_datatype(const Custom2& variable) {
		// assumes no padding between x, y and z
		return std::make_tuple((void*) &(variable.x), 3, MPI_DOUBLE);
	}
}}


int main(int argc, char* argv[]) {
	if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
		std::cerr << "Couldn't initialize MPI." << std::endl;
		abort();
	}

	void* address = NULL;
	int count = -1;
	MPI_Datatype datatype = MPI_DATATYPE_NULL;


	Custom1 c1;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(c1);
	CHECK_TRUE(
		address == &(c1.data)
		and count == 1
		and datatype == MPI_INT
	)


	Custom2 c2;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(c2);
	CHECK_TRUE(
		address == &(c2.x)
		and count == 3
		and datatype == MPI_DOUBLE
	)


	MPI_Finalize();

	return EXIT_SUCCESS;
}
