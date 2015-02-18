/*
Tests assignment of generic cells of different types.

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

#include "cstdlib"
#include "iostream"

#include "check_true.hpp"
#include "gensimcell.hpp"

using namespace std;

struct test_variable1 {
	using data_type = int;
};

struct test_variable2 {
	using data_type = int;
};

using cell_n1_t = gensimcell::Cell<
	gensimcell::Never_Transfer,
	test_variable1
>;
using cell_n2_t = gensimcell::Cell<
	gensimcell::Never_Transfer,
	test_variable2
>;
using cell_n_t = gensimcell::Cell<
	gensimcell::Never_Transfer,
	test_variable1,
	test_variable2
>;
using cell_a1_t = gensimcell::Cell<
	gensimcell::Always_Transfer,
	test_variable1
>;
using cell_a2_t = gensimcell::Cell<
	gensimcell::Always_Transfer,
	test_variable2
>;
using cell_a_t = gensimcell::Cell<
	gensimcell::Always_Transfer,
	test_variable1,
	test_variable2
>;
using cell_o1_t = gensimcell::Cell<
	gensimcell::Optional_Transfer,
	test_variable1
>;
using cell_o2_t = gensimcell::Cell<
	gensimcell::Optional_Transfer,
	test_variable2
>;
using cell_o_t = gensimcell::Cell<
	gensimcell::Optional_Transfer,
	test_variable1,
	test_variable2
>;

int main(int, char**)
{
	using gensimcell::assign;

	constexpr test_variable1 v1{};
	constexpr test_variable2 v2{};

	cell_n1_t n1;
	cell_n2_t n2;
	cell_n_t n;

	cell_a1_t a1;
	cell_a2_t a2;
	cell_a_t a;

	cell_o1_t o1;
	cell_o2_t o2;
	cell_o_t o;

	// different transfer policy
	n1[v1] = 1;
	a1[v1] = 2;
	assign(n1, a1);
	CHECK_TRUE(n1[v1] == a1[v1])
	n1[v1] = 1;
	o1[v1] = 2;
	assign(n1, o1);
	CHECK_TRUE(n1[v1] == o1[v1])
	n2[v2] = 1;
	a2[v2] = 2;
	assign(n2, a2);
	CHECK_TRUE(n2[v2] == a2[v2])
	n2[v2] = 1;
	o2[v2] = 2;
	assign(n2, o2);
	CHECK_TRUE(n2[v2] == o2[v2])
	n[v1] = 1;
	n[v2] = 2;
	a[v1] = 3;
	a[v2] = 4;
	assign(n, a);
	CHECK_TRUE(n[v1] == a[v1])
	CHECK_TRUE(n[v2] == a[v2])
	n[v1] = 1;
	n[v2] = 2;
	o[v1] = 3;
	o[v2] = 4;
	assign(n, o);
	CHECK_TRUE(n[v1] == o[v1])
	CHECK_TRUE(n[v2] == o[v2])

	// different variables
	n1[v1] = 1;
	n2[v2] = 2;
	assign(n1, n2);
	CHECK_TRUE(n1[v1] == 1)
	CHECK_TRUE(n2[v2] == 2)
	a1[v1] = 1;
	a2[v2] = 2;
	assign(a1, a2);
	CHECK_TRUE(a1[v1] == 1)
	CHECK_TRUE(a2[v2] == 2)
	o1[v1] = 1;
	o2[v2] = 2;
	assign(o1, o2);
	CHECK_TRUE(o1[v1] == 1)
	CHECK_TRUE(o2[v2] == 2)

	// different variables and transfer policies
	n1[v1] = 1;
	a2[v2] = 2;
	assign(n1, a2);
	CHECK_TRUE(n1[v1] == 1)
	CHECK_TRUE(a2[v2] == 2)
	n1[v1] = 1;
	o2[v2] = 2;
	assign(n1, o2);
	CHECK_TRUE(n1[v1] == 1)
	CHECK_TRUE(o2[v2] == 2)

	// subset of common variables
	n[v1] = 1;
	n[v2] = 2;
	n1[v1] = 3;
	assign(n, n1);
	CHECK_TRUE(n[v1] == 3)
	CHECK_TRUE(n[v2] == 2)
	CHECK_TRUE(n1[v1] == 3)
	n[v1] = 1;
	n[v2] = 2;
	n1[v1] = 3;
	assign(n1, n);
	CHECK_TRUE(n[v1] == 1)
	CHECK_TRUE(n[v2] == 2)
	CHECK_TRUE(n1[v1] == 1)
	a[v1] = 1;
	a[v2] = 2;
	a1[v1] = 3;
	assign(a, a1);
	CHECK_TRUE(a[v1] == 3)
	CHECK_TRUE(a[v2] == 2)
	CHECK_TRUE(a1[v1] == 3)
	o[v1] = 1;
	o[v2] = 2;
	o1[v1] = 3;
	assign(o, o1);
	CHECK_TRUE(o[v1] == 3)
	CHECK_TRUE(o[v2] == 2)
	CHECK_TRUE(o1[v1] == 3)


	return EXIT_SUCCESS;
}
