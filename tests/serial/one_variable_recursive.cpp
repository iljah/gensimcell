#include "cstdlib"
#include "iostream"

#include "check_true.hpp"
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
	CHECK_TRUE(cell1(test_variable1()) == 3)

	gensimcell::Cell<test_variable2> cell2;
	cell2(test_variable2())(test_variable1()) = 4;
	CHECK_TRUE(cell2(test_variable2())(test_variable1()) == 4)

	gensimcell::Cell<test_variable3> cell3;
	cell3(test_variable3())(test_variable2())(test_variable1()) = 5;
	CHECK_TRUE(cell3(test_variable3())(test_variable2())(test_variable1()) == 5)


	return EXIT_SUCCESS;
}
