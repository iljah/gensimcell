/*
Tests that transfer logic of multiple cells each with multiple variables works.

Copyright (c) 2013, 2014, Ilja Honkonen
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

#include "boost/logic/tribool.hpp"
#include "cstdlib"

#include "check_true.hpp"
#include "gensimcell.hpp"


struct test_variable1 {
	using data_type = int;
};

struct test_variable2 {
	using data_type = float;
};

struct test_variable3 {
	using data_type = char;
};


using cell1_t = gensimcell::Cell<
	test_variable1,
	test_variable2
>;

using cell2_t = gensimcell::Cell<
	test_variable2,
	test_variable3
>;

using cell3_t = gensimcell::Cell<
	test_variable1,
	test_variable2,
	test_variable3
>;


int main(int, char**)
{
	const test_variable1 v1;
	const test_variable2 v2;
	const test_variable3 v3;

	cell1_t c1_1, c1_2;
	cell2_t c2_1, c2_2;
	cell3_t c3_1, c3_2;

	// check default properties
	CHECK_TRUE(not c3_1.get_transfer_all(v1))
	CHECK_TRUE(not c3_1.get_transfer(v1))
	CHECK_TRUE(not c3_1.is_transferred(v1))
	CHECK_TRUE(not c3_2.get_transfer_all(v1))
	CHECK_TRUE(not c3_2.get_transfer(v1))
	CHECK_TRUE(not c3_2.is_transferred(v1))
	CHECK_TRUE(not c3_1.get_transfer_all(v2))
	CHECK_TRUE(not c3_1.get_transfer(v2))
	CHECK_TRUE(not c3_1.is_transferred(v2))
	CHECK_TRUE(not c3_2.get_transfer_all(v2))
	CHECK_TRUE(not c3_2.get_transfer(v2))
	CHECK_TRUE(not c3_2.is_transferred(v2))
	CHECK_TRUE(not c3_1.get_transfer_all(v3))
	CHECK_TRUE(not c3_1.get_transfer(v3))
	CHECK_TRUE(not c3_1.is_transferred(v3))
	CHECK_TRUE(not c3_2.get_transfer_all(v3))
	CHECK_TRUE(not c3_2.get_transfer(v3))
	CHECK_TRUE(not c3_2.is_transferred(v3))

	// test static variable logic
	c1_1.set_transfer_all(true, v2);
	CHECK_TRUE(c1_1.is_transferred(v2))
	CHECK_TRUE(c1_2.is_transferred(v2))
	CHECK_TRUE(not c2_1.is_transferred(v2))
	CHECK_TRUE(not c2_2.is_transferred(v2))
	CHECK_TRUE(not c3_1.is_transferred(v2))
	CHECK_TRUE(not c3_2.is_transferred(v2))

	// test combinations of static and cell specific logic
	c3_1.set_transfer_all(boost::logic::indeterminate, v2);
	c3_1.set_transfer(true, v2);
	CHECK_TRUE(c3_1.is_transferred(v2))
	CHECK_TRUE(not c3_2.is_transferred(v2))

	c2_2.set_transfer(true, v3);
	c2_2.set_transfer_all(boost::logic::indeterminate, v3);
	c3_1.set_transfer_all(boost::logic::indeterminate, v3);
	c3_1.set_transfer(false, v3);
	c3_2.set_transfer(true, v3);
	CHECK_TRUE(not c2_1.is_transferred(v3))
	CHECK_TRUE(c2_2.is_transferred(v3))
	CHECK_TRUE(not c3_1.is_transferred(v3))
	CHECK_TRUE(c3_2.is_transferred(v3))


	return EXIT_SUCCESS;
}
