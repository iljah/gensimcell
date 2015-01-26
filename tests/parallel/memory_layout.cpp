/*
Tests effect of different transfer policies on memory layout of variables.

Copyright 2014, 2015 Ilja Honkonen
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

/*!
First variable given as a template argument should
end up as first in memory when transferred.
*/

#include "cstdlib"
#include "mpi.h"

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


using cell_never_t = gensimcell::Cell<
	gensimcell::Never_Transfer,
	test_variable1,
	test_variable2,
	test_variable3
>;

using cell_always_t = gensimcell::Cell<
	gensimcell::Always_Transfer,
	test_variable1,
	test_variable2,
	test_variable3
>;

using cell_optional_t = gensimcell::Cell<
	gensimcell::Optional_Transfer,
	test_variable1,
	test_variable2,
	test_variable3
>;


int main(int, char**)
{
	const test_variable1 v1{};
	const test_variable2 v2{};
	const test_variable3 v3{};

	cell_never_t never;
	char
		*never_addr_v1 = &never[v1],
		*never_addr_v2 = &never[v2],
		*never_addr_v3 = &never[v3];

	if (abs(never_addr_v1 - never_addr_v2) != 1) {
		std::cerr << __FILE__ << ":" << __LINE__
			<< " Memory offset between variables should be 1 byte."
			<< std::endl;
	}
	if (abs(never_addr_v2 - never_addr_v3) != 1) {
		std::cerr << __FILE__ << ":" << __LINE__
			<< " Memory offset between variables should be 1 byte."
			<< std::endl;
	}

	cell_always_t always;
	char
		*always_addr_v1 = &always[v1],
		*always_addr_v2 = &always[v2],
		*always_addr_v3 = &always[v3];

	if (abs(always_addr_v1 - always_addr_v2) != 1) {
		std::cerr << __FILE__ << ":" << __LINE__
			<< " Memory offset between variables should be 1 byte."
			<< std::endl;
	}
	if (abs(always_addr_v2 - always_addr_v3) != 1) {
		std::cerr << __FILE__ << ":" << __LINE__
			<< " Memory offset between variables should be 1 byte."
			<< std::endl;
	}

	cell_optional_t optional;
	char
		*optional_addr_v1 = &optional[v1],
		*optional_addr_v2 = &optional[v2],
		*optional_addr_v3 = &optional[v3];

	if (abs(optional_addr_v1 - optional_addr_v2) == 1) {
		std::cerr << __FILE__ << ":" << __LINE__
			<< " Unexpected memory offset of 1 byte between variables."
			<< std::endl;
	}
	if (abs(optional_addr_v2 - optional_addr_v3) == 1) {
		std::cerr << __FILE__ << ":" << __LINE__
			<< " Unexpected memory offset of 1 byte between variables."
			<< std::endl;
	}

	return EXIT_SUCCESS;
}
