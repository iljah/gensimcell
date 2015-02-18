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
	assign(n1, a1);
	n1.assign(a1);
	assign(n1, o1);
	n1.assign(o1);
	assign(n2, a2);
	n2.assign(a2);
	assign(n2, o2);
	n2.assign(o2);
	assign(n, a);
	n.assign(a);
	assign(n, o);
	n.assign(o);

	// different variables
	assign(n1, n2);
	n1.assign(n2);
	assign(a1, a2);
	a1.assign(a2);
	assign(o1, o2);
	o1.assign(o2);

	// different variables and transfer policies
	assign(n1, a2);
	n1.assign(a2);
	assign(n1, o2);
	n1.assign(o2);

	// subset of common variables
	assign(n, n1);
	n.assign(n1);
	assign(a, a1);
	a.assign(a1);
	assign(o, o1);
	o.assign(o1);


	return EXIT_SUCCESS;
}
