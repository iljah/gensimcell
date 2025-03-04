/*
Tests whether a cell with multiple variables compiles.

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


Author(s): Ilja Honkonen
*/

#include "array"
#include "cstdint"
#include "cstdlib"
#include "tuple"
#include "vector"

#include "gensimcell.hpp"

struct test_variable1 {
	using data_type = int;
};

struct test_variable2 {
	using data_type = double;
};

struct test_variable3 {
	using data_type = float;
};

struct test_variable4 {
	using data_type = uint64_t;
};

struct test_variable5 {
	using data_type = std::array<double, 3>;
};

struct test_variable6 {
	using data_type = std::tuple<int, float, test_variable5>;
};

struct test_variable7 {
	using data_type = std::vector<int>;
};


int main(int, char**)
{
	gensimcell::Cell<
		gensimcell::Never_Transfer,
		test_variable1,
		test_variable2,
		test_variable3,
		test_variable4,
		test_variable5,
		test_variable6,
		test_variable7
	> cell1;


	#ifdef HAVE_MPI

	gensimcell::Cell<
		gensimcell::Always_Transfer,
		test_variable1,
		test_variable2,
		test_variable3,
		test_variable4,
		test_variable5,
		test_variable6,
		test_variable7
	> cell2;

	gensimcell::Cell<
		gensimcell::Optional_Transfer,
		test_variable1,
		test_variable2,
		test_variable3,
		test_variable4,
		test_variable5,
		test_variable6,
		test_variable7
	> cell3;

	#endif // ifdef HAVE_MPI

	return 0;
}
