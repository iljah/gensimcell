/*
Tests that transfer logic works when the cell is used as a variable in a cell.

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

#include "boost/logic/tribool.hpp"
#include "cstdlib"
#include "iostream"
#include "mpi.h"

#include "check_true.hpp"
#include "gensimcell.hpp"


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
	using data_type = cell1_t;
};

using cell3_t = gensimcell::Cell<
	gensimcell::Optional_Transfer,
	test_variable3
>;


using cell4_t = gensimcell::Cell<
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

	cell1_t c1;
	cell2_t c2;
	cell3_t c3;
	cell4_t c4;

	// check default properties
	CHECK_TRUE(not c4.is_transferred(v1))
	CHECK_TRUE(not c4.is_transferred(v2))
	CHECK_TRUE(not c4.is_transferred(v3))
	CHECK_TRUE(not c4[v2].is_transferred(v1))
	CHECK_TRUE(not c4[v3].is_transferred(v1))


	// check that static cell1_t logic affects all its users
	CHECK_TRUE(not c1.is_transferred(v1))
	CHECK_TRUE(not c2[v2].is_transferred(v1))
	CHECK_TRUE(not c3[v3].is_transferred(v1))
	c1.set_transfer_all(true, v1);
	CHECK_TRUE(c1.is_transferred(v1))
	CHECK_TRUE(c2[v2].is_transferred(v1))
	CHECK_TRUE(c3[v3].is_transferred(v1))
	CHECK_TRUE(c4[v2].is_transferred(v1))
	CHECK_TRUE(c4[v3].is_transferred(v1))

	// check cell/variable specific logic
	c1.set_transfer_all(boost::logic::indeterminate, v1);
	c2[v2].set_transfer(true, v1);
	CHECK_TRUE(c2[v2].is_transferred(v1))
	CHECK_TRUE(not c3[v3].is_transferred(v1))
	CHECK_TRUE(not c4[v2].is_transferred(v1))
	CHECK_TRUE(not c4[v3].is_transferred(v1))

	c4[v2].set_transfer(true, v1);
	CHECK_TRUE(c4[v2].is_transferred(v1))
	CHECK_TRUE(not c4[v3].is_transferred(v1))

	c1.set_transfer_all(false, v1);
	CHECK_TRUE(not c1.is_transferred(v1))
	CHECK_TRUE(not c2[v2].is_transferred(v1))
	CHECK_TRUE(not c3[v3].is_transferred(v1))
	CHECK_TRUE(not c4[v2].is_transferred(v1))
	CHECK_TRUE(not c4[v3].is_transferred(v1))

	c4.set_transfer_all(true, v1, v2, v3);
	CHECK_TRUE(c4.is_transferred(v1))
	CHECK_TRUE(c4.is_transferred(v2))
	CHECK_TRUE(c4.is_transferred(v3))

	c4[v3].set_transfer_all(true, v1, v1, v1);
	CHECK_TRUE(c4[v3].is_transferred(v1))


	return EXIT_SUCCESS;
}
