/*
Tests that transfer logic of a cell with one variable works.

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

#include "cstdlib"
#include "mpi.h"

#include "check_true.hpp"
#include "gensimcell.hpp"


struct test_variable1 {
	using data_type = int;
};

using cell1_t = gensimcell::Cell<gensimcell::Optional_Transfer, test_variable1>;


int main(int, char**)
{
	const test_variable1 v1{};
	cell1_t c1;

	// check default properties
	CHECK_TRUE(not cell1_t::get_transfer_all(v1))
	CHECK_TRUE(not c1.get_transfer_all(v1))
	CHECK_TRUE(not c1.get_transfer(v1))
	CHECK_TRUE(not c1.is_transferred(v1))

	// test logic between static and instance specific transfers
	c1.set_transfer(true, v1);
	CHECK_TRUE(not c1.is_transferred(v1))

	c1.set_transfer_all(boost::logic::indeterminate, v1);
	CHECK_TRUE(c1.is_transferred(v1))

	c1.set_transfer_all(true, v1);
	c1.set_transfer(false, v1);
	CHECK_TRUE(c1.is_transferred(v1))

	// check variadic version
	c1.set_transfer_all(false, v1, v1);
	CHECK_TRUE(not c1.is_transferred(v1))

	c1.set_transfer_all(boost::logic::indeterminate, v1, v1, v1);
	c1.set_transfer(true, v1, v1, v1, v1);
	CHECK_TRUE(c1.is_transferred(v1))

	c1.set_transfer(false, v1, v1, v1);
	CHECK_TRUE(not c1.is_transferred(v1))

	return EXIT_SUCCESS;
}
