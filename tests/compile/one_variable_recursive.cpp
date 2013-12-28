#include "gensimcell.hpp"

struct test_variable {
	typedef int data_type;
};


int main(int argc, char* argv[])
{
	gensimcell::Cell<
		gensimcell::Cell<
			test_variable
		>
	> cell;

	return 0;
}
