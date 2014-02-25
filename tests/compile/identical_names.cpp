/*
Tests if variables with identical names can be used
if the variables are defined in different namespaces
*/

#include "gensimcell.hpp"

namespace first {
	struct variable { using data_type = int; };
}

namespace second {
	struct variable { using data_type = double; };
}

int main(int, char**)
{
	gensimcell::Cell<first::variable, second::variable> cell;

	return 0;
}
