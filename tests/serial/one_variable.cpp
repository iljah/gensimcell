/*
Tests that transfer logic of a cell with one variable works.

Copyright 2013, 2014 Ilja Honkonen
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
	using data_type = char;
};

using cell_t = gensimcell::Cell<
	gensimcell::Always_Transfer,
	test_variable1
>;

struct test_variable4 {
	using data_type = std::array<cell_t, 2>;
};


int main(int, char**)
{
	gensimcell::Cell<gensimcell::Optional_Transfer, test_variable1> cell;
	cell[test_variable1()] = 3;
	CHECK_TRUE(cell[test_variable1()] == 3)


	array<gensimcell::Cell<gensimcell::Optional_Transfer, test_variable1>, 3> cell_array;
	cell_array[0][test_variable1()] = -1;
	cell_array[1][test_variable1()] = -2;
	cell_array[2][test_variable1()] = -3;
	CHECK_TRUE(cell_array[0][test_variable1()] == -1)
	CHECK_TRUE(cell_array[1][test_variable1()] == -2)
	CHECK_TRUE(cell_array[2][test_variable1()] == -3)


	tuple<
		gensimcell::Cell<gensimcell::Optional_Transfer, test_variable1>,
		gensimcell::Cell<gensimcell::Optional_Transfer, test_variable2>,
		gensimcell::Cell<gensimcell::Optional_Transfer, test_variable3>
	> cell_tuple;
	get<0>(cell_tuple)[test_variable1()] = -10;
	get<1>(cell_tuple)[test_variable2()] = -0.5;
	get<2>(cell_tuple)[test_variable3()] = '3';
	CHECK_TRUE(get<0>(cell_tuple)[test_variable1()] == -10)
	CHECK_TRUE(get<1>(cell_tuple)[test_variable2()] == -0.5)
	CHECK_TRUE(get<2>(cell_tuple)[test_variable3()] == '3')


	vector<gensimcell::Cell<gensimcell::Optional_Transfer, test_variable1>> cell_vector(3);
	cell_vector[0][test_variable1()] = int(100.1);
	cell_vector[1][test_variable1()] = int(200.3);
	cell_vector[2][test_variable1()] = int(300.6);
	CHECK_TRUE(cell_vector[0][test_variable1()] == 100)
	CHECK_TRUE(cell_vector[1][test_variable1()] == 200)
	CHECK_TRUE(cell_vector[2][test_variable1()] == 300)


	gensimcell::Cell<gensimcell::Optional_Transfer, test_variable4> array_cell;
	array_cell[test_variable4()][1][test_variable1()] = 4;
	CHECK_TRUE(array_cell[test_variable4()][1][test_variable1()] == 4)


	return EXIT_SUCCESS;
}
