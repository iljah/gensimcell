#include "array"
#include "cstdlib"
#include "tuple"
#include "vector"

#include "gensimcell.hpp"

struct test_variable1 {
	typedef int data_type;
};

struct test_variable2 {
	typedef double data_type;
};

struct test_variable3 {
	typedef float data_type;
};

struct test_variable4 {
	typedef uint64_t data_type;
};

struct test_variable5 {
	typedef std::array<double, 3> data_type;
};

struct test_variable6 {
	typedef std::tuple<int, float, test_variable5> data_type;
};

struct test_variable7 {
	typedef std::vector<int> data_type;
};


int main(int argc, char* argv[])
{
	gensimcell::Cell<
		test_variable1,
		test_variable2,
		test_variable3,
		test_variable4,
		test_variable5,
		test_variable6,
		test_variable7
	> cell;

	return 0;
}
