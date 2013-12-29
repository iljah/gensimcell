#include "cstdlib"
#include "iostream"

#include "gensimcell.hpp"

using namespace std;

struct test_variable1 {
	typedef int data_type;
};

struct test_variable2 {
	typedef gensimcell::Cell<test_variable1> data_type;
};

struct test_variable3 {
	typedef gensimcell::Cell<test_variable2> data_type;
};


int main(int argc, char* argv[])
{
	gensimcell::Cell<test_variable1> cell1;
	cell1(test_variable1()) = 3;
	if (cell1(test_variable1()) != 3) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		abort();
	}


	gensimcell::Cell<test_variable2> cell2;
	cell2(test_variable2())(test_variable1()) = 4;
	if (cell2(test_variable2())(test_variable1()) != 4) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		abort();
	}


	gensimcell::Cell<test_variable3> cell3;
	cell3(test_variable3())(test_variable2())(test_variable1()) = 5;
	if (cell3(test_variable3())(test_variable2())(test_variable1()) != 5) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		abort();
	}


	return EXIT_SUCCESS;
}
