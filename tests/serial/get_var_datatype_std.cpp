/*
Tests that get_var_mpi_datatype returns correct datatypes for standard variables.

Copyright 2013, 2014, 2015, 2016 Ilja Honkonen
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

int main(int argc, char* argv[]) {
	if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
		std::cerr << "Couldn't initialize MPI." << std::endl;
		abort();
	}

	void* address = NULL;
	int count = -1;
	MPI_Datatype datatype = MPI_DATATYPE_NULL;


	#ifdef MPI_CXX_BOOL
	bool b;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(b);
	CHECK_TRUE(
		address == &b
		and count == 1
		and datatype == MPI_CXX_BOOL
	)
	#endif


	char c;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(c);
	CHECK_TRUE(
		address == &c
		and count == 1
		and datatype == MPI_CHAR
	)


	signed long long int a;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(a);
	CHECK_TRUE(
		address == &a
		and count == 1
		and datatype == MPI_LONG_LONG_INT
	)


	#ifdef MPI_CXX_LONG_DOUBLE_COMPLEX
	std::complex<long double> d;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(d);
	CHECK_TRUE(
		address == &d
		and count == 1
		and datatype == MPI_CXX_LONG_DOUBLE_COMPLEX
	)
	#endif


	std::array<char, 5> e;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(e);
	CHECK_TRUE(
		address == e.data()
		and count == 5
		and datatype == MPI_CHAR
	)


	#ifdef MPI_CXX_DOUBLE_COMPLEX
	std::array<std::complex<double>, 3> f;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(f);
	CHECK_TRUE(
		address == f.data()
		and count == 3
		and datatype == MPI_CXX_DOUBLE_COMPLEX
	)
	#endif


	std::vector<int> g;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(g);
	CHECK_TRUE(
		address == g.data()
		and count == 0
		and datatype == MPI_INT
	)

	g.insert(g.end(), {-1, 1, -2, 2});
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(g);
	CHECK_TRUE(
		address == g.data()
		and count == 4
		and datatype == MPI_INT
		and *(static_cast<int*>(address) + 2) == -2
	)


	std::tuple<> h0;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(h0);
	CHECK_TRUE(
		address == nullptr
		and count == 0
	)

	std::tuple<long double> h1;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(h1);
	CHECK_TRUE(
		address == &(std::get<0>(h1))
		and count == 1
		and datatype == MPI_LONG_DOUBLE
	)

	std::tuple<char, int, double> h;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(h);
	CHECK_TRUE(
		address == &(std::get<0>(h))
		and count == 1
	)


	std::vector<std::tuple<double, char>> i;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(i);
	CHECK_TRUE(
		address == nullptr
		and count == 0
	)

	i.emplace_back(3, 'a');
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(i);
	CHECK_TRUE(
		address == &(get<0>(i[0]))
		and count == 1
	)


	std::vector<std::vector<std::array<std::tuple<int, char>, 3>>> j;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(j);
	CHECK_TRUE(
		address == nullptr
		and count == 0
	)

	j.resize(1);
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(j);
	CHECK_TRUE(
		address == nullptr
		and count == 0
	)

	j[0].resize(1);
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(j);
	CHECK_TRUE(
		address == &(std::get<0>(j[0][0][0]))
		and count == 1
	)


	std::pair<std::tuple<int>, std::tuple<double, char>> k;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_mpi_datatype(k);
	CHECK_TRUE(
		address == &(std::get<0>(k.first))
		and count == 1
	)


	MPI_Finalize();

	return EXIT_SUCCESS;
}
