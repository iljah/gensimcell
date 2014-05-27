/*
Tests that get_var_mpi_datatype returns correct datatypes for Eigen variables.

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

#include "array"
#include "complex"
#include "cstdlib"
#include "Eigen/Core"
#include "iostream"
#include "mpi.h"

#include "check_true.hpp"
#include "get_var_mpi_datatype.hpp"

using namespace std;

int main(int, char**) {

	void* address = NULL;
	int count = -1;
	MPI_Datatype datatype = MPI_DATATYPE_NULL;


	Eigen::Vector2f a;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(a);
	CHECK_TRUE(
		address == a.data()
		and count == 2
		and datatype == MPI_FLOAT
	)


	Eigen::Vector3d b;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(b);
	CHECK_TRUE(
		address == b.data()
		and count == 3
		and datatype == MPI_DOUBLE
	)


	Eigen::Matrix3i c;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(c);
	CHECK_TRUE(
		address == c.data()
		and count == 9
		and datatype == MPI_INT
	)


	Eigen::Matrix<std::complex<long double>, 2, 4> d;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(d);
	CHECK_TRUE(
		address == d.data()
		and count == 8
		and datatype == MPI_CXX_LONG_DOUBLE_COMPLEX
	)


	return EXIT_SUCCESS;
}
