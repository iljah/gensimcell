/*
Tests whether a cell with one variable complies.

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

#ifdef HAVE_MPI
#include "mpi.h"
#endif

#include "gensimcell.hpp"

struct test_variable1 {
	using data_type = int;
};

#ifdef HAVE_MPI
using cell1_t = gensimcell::Cell<gensimcell::Optional_Transfer, test_variable1>;
#else
using cell1_t = gensimcell::Cell<gensimcell::Never_Transfer, test_variable1>;
#endif

struct test_variable2 {
	using data_type = cell1_t;
};

#ifdef HAVE_MPI
using cell2_t = gensimcell::Cell<gensimcell::Always_Transfer, test_variable2>;
#else
using cell2_t = gensimcell::Cell<gensimcell::Never_Transfer, test_variable2>;
#endif

template<class Variable_T> bool test_recursive_ret_type(const cell2_t& cell)
{
	const typename Variable_T::data_type& data = cell[Variable_T()];
	return data[test_variable1()] == cell[Variable_T()][test_variable1()];
}

int main(int, char**)
{
	cell2_t c1, c2;
	c1 = c2;

	test_recursive_ret_type<test_variable2>(c1);

	return 0;
}
