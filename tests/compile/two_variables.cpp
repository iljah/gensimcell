#include "gensimcell.hpp"

struct test_variable1 {
	typedef int data_type;
};

struct test_variable2 {
	typedef double data_type;
};


int main(int, char**)
{
	gensimcell::Cell<test_variable1, test_variable2> cell;

	return 0;
}
