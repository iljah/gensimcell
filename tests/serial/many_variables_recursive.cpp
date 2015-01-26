/*
Tests that transfer logic of a cell with multiple cells as variables works.

Copyright 2013, 2014, 2015 Ilja Honkonen
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

struct test_variable2 {
	using data_type = float;
};

struct test_variable3 {
	using data_type = std::array<char, 2>;
};


struct test_variable4 {
	using data_type = gensimcell::Cell<
		gensimcell::Optional_Transfer,
		test_variable1,
		test_variable2,
		test_variable3
	>;
};

struct test_variable5 {
	using data_type = gensimcell::Cell<
		gensimcell::Optional_Transfer,
		test_variable1,
		test_variable4
	>;
};

struct test_variable6 {
	using data_type = gensimcell::Cell<
		gensimcell::Optional_Transfer,
		test_variable4,
		test_variable5
	>;
};


int main(int, char**)
{
	const test_variable1 v1{};
	const test_variable2 v2{};
	const test_variable3 v3{};
	const test_variable4 v4{};
	const test_variable5 v5{};
	const test_variable6 v6{};

	gensimcell::Cell<gensimcell::Optional_Transfer, test_variable4> cell4;
	cell4[v4][v1] = 3;
	cell4[v4][v2] = 1.5;
	cell4[v4][v3] = {{'a', 'b'}};
	CHECK_TRUE(cell4[v4][v1] == 3)
	CHECK_TRUE(cell4[v4][v2] == 1.5)
	CHECK_TRUE(cell4[v4][v3][0] == 'a')
	CHECK_TRUE(cell4[v4][v3][1] == 'b')


	gensimcell::Cell<gensimcell::Optional_Transfer, test_variable5> cell5;
	cell5[v5][v1] = 4;
	cell5[v5][v4][v1] = 5;
	cell5[v5][v4][v2] = 2.5;
	cell5[v5][v4][v3] = {{'c', 'd'}};
	CHECK_TRUE(cell5[v5][v1] == 4)
	CHECK_TRUE(cell5[v5][v4][v1] == 5)
	CHECK_TRUE(cell5[v5][v4][v2] == 2.5)
	CHECK_TRUE(cell5[v5][v4][v3][0] == 'c')
	CHECK_TRUE(cell5[v5][v4][v3][1] == 'd')


	gensimcell::Cell<gensimcell::Optional_Transfer, test_variable6> cell6;
	cell6[v6][v4][v1] = 6;
	cell6[v6][v4][v2] = 4.5;
	cell6[v6][v4][v3] = {{'e', 'f'}};
	cell6[v6][v5][v1] = 7;
	cell6[v6][v5][v4][v1] = 8;
	cell6[v6][v5][v4][v2] = 8.5;
	cell6[v6][v5][v4][v3] = {{'g', 'h'}};
	CHECK_TRUE(cell6[v6][v4][v1] == 6)
	CHECK_TRUE(cell6[v6][v4][v2] == 4.5)
	CHECK_TRUE(cell6[v6][v4][v3][0] == 'e')
	CHECK_TRUE(cell6[v6][v4][v3][1] == 'f')
	CHECK_TRUE(cell6[v6][v5][v1] == 7)
	CHECK_TRUE(cell6[v6][v5][v4][v1] == 8)
	CHECK_TRUE(cell6[v6][v5][v4][v2] == 8.5)
	CHECK_TRUE(cell6[v6][v5][v4][v3][0] == 'g')
	CHECK_TRUE(cell6[v6][v5][v4][v3][1] == 'h')


	return EXIT_SUCCESS;
}
