#include "gensimcell.hpp"

struct test_variable {
	typedef int data_type;
};


int main(int, char**)
{
	gensimcell::Cell<test_variable> cell;

	return 0;
}
