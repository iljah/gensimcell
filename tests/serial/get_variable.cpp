/*
Test for retrieving variable reference to arbitrary depth in cell.

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
*/

#include "cstdlib"
#include "iostream"

#include "gensimcell.hpp"


using namespace gensimcell;


struct test_variable1 {
	using data_type = int;
};
using cell1_t = gensimcell::Cell<
	gensimcell::Optional_Transfer,
	test_variable1
>;

struct test_variable2 {
	using data_type = gensimcell::Cell<
		gensimcell::Optional_Transfer,
		test_variable1
	>;
};
using cell2_t = gensimcell::Cell<
	gensimcell::Optional_Transfer,
	test_variable2
>;

struct test_variable3 {
	using data_type = gensimcell::Cell<
		gensimcell::Optional_Transfer,
		test_variable2
	>;
};
using cell3_t = gensimcell::Cell<
	gensimcell::Optional_Transfer,
	test_variable3
>;


int main(int, char**)
{
	cell1_t cell1;
	cell1[test_variable1()] = 1;
	// test access to const reference
	if (get(cell1, test_variable1()) != 1) {
		std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
		abort();
	}
	// test access to non-const reference
	get(cell1, test_variable1()) = -1;
	if (cell1[test_variable1()] != -1) {
		std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
		abort();
	}

	cell2_t cell2;
	cell2[test_variable2()][test_variable1()] = 2;
	if (get(cell2, test_variable2(), test_variable1()) != 2) {
		std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
		abort();
	}
	get(cell2, test_variable2(), test_variable1()) = -2;
	if (cell2[test_variable2()][test_variable1()] != -2) {
		std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
		abort();
	}

	cell3_t cell3;
	cell3[test_variable3()][test_variable2()][test_variable1()] = 3;
	if (get(cell3, test_variable3(), test_variable2(), test_variable1()) != 3) {
		std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
		abort();
	}
	get(cell3, test_variable3(), test_variable2(), test_variable1()) = -3;
	if (cell3[test_variable3()][test_variable2()][test_variable1()] != -3) {
		std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
		abort();
	}

	return EXIT_SUCCESS;
}
