#include "gensimcell.hpp"

struct test_variable1 {
	typedef int data_type;
};

struct test_variable2 {
	typedef float data_type;
};

struct test_variable3 {
	typedef double data_type;
};

struct test_variable4 {
	typedef char data_type;
};

struct test_variable5 {
	typedef gensimcell::Cell<
		test_variable4,
		test_variable3,
		test_variable2,
		test_variable1
	> data_type;
};

struct test_variable6 {
	typedef gensimcell::Cell<test_variable5, test_variable4> data_type;
};


int main(int argc, char* argv[])
{
	gensimcell::Cell<test_variable5> cell1;
	gensimcell::Cell<test_variable6> cell2;

	gensimcell::Cell<
		test_variable2,
		gensimcell::Cell<
			test_variable1,
			test_variable5
		>,
		test_variable6
	> cell3;

	return 0;
}
