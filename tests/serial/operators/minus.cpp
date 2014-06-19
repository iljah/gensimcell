/*
Tests - and -= logic of generic cell.

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
#include "cstdlib"
#include "iostream"
#include "tuple"
#include "vector"

#include "check_true.hpp"
#include "gensimcell.hpp"

using namespace std;

struct test_variable1 {
	using data_type = int;
};

using cell1_t = gensimcell::Cell<test_variable1>;

struct test_variable2 {
	using data_type = cell1_t;
};

using cell2_t = gensimcell::Cell<test_variable2>;

using cell3_t = gensimcell::Cell<
	test_variable1,
	test_variable2
>;

struct test_variable3 {
	using data_type = double;
};

struct test_variable4 {
	using data_type = long long int;
};

using cell4_t = gensimcell::Cell<
	test_variable1,
	test_variable3,
	test_variable4
>;

int main(int, char**)
{
	const test_variable1 v1{};
	const test_variable2 v2{};
	const test_variable3 v3{};
	const test_variable4 v4{};

	cell1_t c1_1, c1_2;
	c1_1[v1] = 3;
	c1_2[v1] = 4;
	c1_1 -= c1_2;
	CHECK_TRUE(c1_1[v1] == -1)
	CHECK_TRUE(c1_2[v1] == 4)
	c1_1[v1] = 3;
	c1_2[v1] = 4;
	c1_1 = c1_1 - c1_2;
	CHECK_TRUE(c1_1[v1] == -1)
	CHECK_TRUE(c1_2[v1] == 4)
	c1_1 -= 1;
	c1_2 = c1_2 - 3;
	CHECK_TRUE(c1_1[v1] == -2)
	CHECK_TRUE(c1_2[v1] == 1)

	cell2_t c2_1, c2_2;
	c2_1[v2][v1] = -3;
	c2_2[v2][v1] = -4;
	c2_1[v2] -= c2_2[v2];
	CHECK_TRUE(c2_1[v2][v1] == 1)
	CHECK_TRUE(c2_2[v2][v1] == -4)

	cell3_t c3_1, c3_2;
	c3_1[v1] = 3;
	c3_2[v1] = 4;
	c3_1[v2][v1] = -3;
	c3_2[v2][v1] = -4;
	c3_1[v1] -= c3_2[v1];
	CHECK_TRUE(c3_1[v1] == -1)
	CHECK_TRUE(c3_2[v1] == 4)
	CHECK_TRUE(c3_1[v2][v1] == -3)
	c3_1[v2] -= c3_2[v2];
	CHECK_TRUE(c3_1[v2][v1] == 1)
	CHECK_TRUE(c3_2[v2][v1] == -4)
	c3_2 -= c3_1;
	CHECK_TRUE(c3_2[v1] == 5)
	CHECK_TRUE(c3_2[v2][v1] == -5)
	c3_1 = c3_2 - c3_1;
	CHECK_TRUE(c3_1[v1] == 6)
	CHECK_TRUE(c3_1[v2][v1] == -6)

	cell4_t c4_1, c4_2;
	c4_1[v1] = 3;
	c4_2[v1] = 4;
	c4_1[v3] = -3;
	c4_2[v3] = -4;
	c4_1[v4] = 300;
	c4_2[v4] = 400;
	c4_1.minus_equal(c4_2, v1, v3);
	CHECK_TRUE(c4_1[v1] == -1)
	CHECK_TRUE(c4_2[v1] == 4)
	CHECK_TRUE(c4_1[v3] == 1)
	CHECK_TRUE(c4_2[v3] == -4)
	CHECK_TRUE(c4_1[v4] == 300)
	CHECK_TRUE(c4_2[v4] == 400)
	c4_1.minus_equal(c4_2, v3, v4);
	CHECK_TRUE(c4_1[v1] == -1)
	CHECK_TRUE(c4_2[v1] == 4)
	CHECK_TRUE(c4_1[v3] == 5)
	CHECK_TRUE(c4_2[v3] == -4)
	CHECK_TRUE(c4_1[v4] == -100)
	CHECK_TRUE(c4_2[v4] == 400)
	c4_2 -= c4_1;
	CHECK_TRUE(c4_1[v1] == -1)
	CHECK_TRUE(c4_2[v1] == 5)
	CHECK_TRUE(c4_1[v3] == 5)
	CHECK_TRUE(c4_2[v3] == -9)
	CHECK_TRUE(c4_1[v4] == -100)
	CHECK_TRUE(c4_2[v4] == 500)
	c4_1 -= c4_1 - c4_2 - c4_1;
	CHECK_TRUE(c4_1[v1] == 4)
	CHECK_TRUE(c4_1[v3] == -4)
	CHECK_TRUE(c4_1[v4] == 400)

	c4_1[v1] = 3;
	c4_1[v3] = -3;
	c4_1[v4] = 300;
	c4_1 -= 1;
	c4_1 -= c4_1 - float(1);
	c4_1 -= c4_1 - double(1);
	CHECK_TRUE(c4_1[v1] == 1)
	CHECK_TRUE(c4_1[v3] == 1)
	CHECK_TRUE(c4_1[v4] == 1)

	return EXIT_SUCCESS;
}
