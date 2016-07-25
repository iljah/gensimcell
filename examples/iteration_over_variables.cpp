/*
Example of iterating over cell's variables using boost::mpl.

Copyright 2015, 2016 Ilja Honkonen
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

#include "iostream"

#include "boost/mpl/for_each.hpp"
#include "boost/mpl/vector.hpp"
#include "gensimcell.hpp"

using namespace std;

struct variable1 {
	using data_type = int;
};

struct variable2 {
	using data_type = float;
};

struct variable3 {
	using data_type = char;
};

int main(int, char**)
{
	gensimcell::Cell<
		gensimcell::Never_Transfer,
		variable1,
		variable2,
		variable3
	> cell;
	cell[variable1()] = 3;
	cell[variable2()] = 1.5;
	cell[variable3()] = '3';

	// lambda with auto argument requires C++14,
	// see documentation of boost::mpl::for_each
	// on how to support older compilers
	boost::mpl::for_each<
		boost::mpl::vector<variable1, variable3>
	>(
		[&cell](auto variable){
			cout << cell[variable] << " ";
		}
	);
	cout << endl;
	// prints 3 3

	return 0;
}
