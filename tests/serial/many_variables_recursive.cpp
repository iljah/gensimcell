#include "array"
#include "cstdlib"
#include "iostream"
#include "tuple"
#include "vector"

#include "check_true.hpp"
#include "gensimcell.hpp"


using namespace std;


struct test_variable1 {
	typedef int data_type;
};

struct test_variable2 {
	typedef float data_type;
};

struct test_variable3 {
	typedef std::array<char, 2> data_type;
};


struct test_variable4 {
	typedef gensimcell::Cell<
		test_variable1,
		test_variable2,
		test_variable3
	> data_type;
};

struct test_variable5 {
	typedef gensimcell::Cell<
		test_variable1,
		test_variable4
	> data_type;
};

struct test_variable6 {
	typedef gensimcell::Cell<
		test_variable4,
		test_variable5
	> data_type;
};


int main(int, char**)
{
	const test_variable1 v1;
	const test_variable2 v2;
	const test_variable3 v3;
	const test_variable4 v4;
	const test_variable5 v5;
	const test_variable6 v6;

	gensimcell::Cell<test_variable4> cell4;
	cell4(v4)(v1) = 3;
	cell4(v4)(v2) = 1.5;
	cell4(v4)(v3) = {'a', 'b'};
	CHECK_TRUE(cell4(v4)(v1) == 3)
	CHECK_TRUE(cell4(v4)(v2) == 1.5)
	CHECK_TRUE(cell4(v4)(v3)[0] == 'a')
	CHECK_TRUE(cell4(v4)(v3)[1] == 'b')


	gensimcell::Cell<test_variable5> cell5;
	cell5(v5)(v1) = 4;
	cell5(v5)(v4)(v1) = 5;
	cell5(v5)(v4)(v2) = 2.5;
	cell5(v5)(v4)(v3) = {'c', 'd'};
	CHECK_TRUE(cell5(v5)(v1) == 4)
	CHECK_TRUE(cell5(v5)(v4)(v1) == 5)
	CHECK_TRUE(cell5(v5)(v4)(v2) == 2.5)
	CHECK_TRUE(cell5(v5)(v4)(v3)[0] == 'c')
	CHECK_TRUE(cell5(v5)(v4)(v3)[1] == 'd')


	gensimcell::Cell<test_variable6> cell6;
	cell6(v6)(v4)(v1) = 6;
	cell6(v6)(v4)(v2) = 4.5;
	cell6(v6)(v4)(v3) = {'e', 'f'};
	cell6(v6)(v5)(v1) = 7;
	cell6(v6)(v5)(v4)(v1) = 8;
	cell6(v6)(v5)(v4)(v2) = 8.5;
	cell6(v6)(v5)(v4)(v3) = {'g', 'h'};
	CHECK_TRUE(cell6(v6)(v4)(v1) == 6)
	CHECK_TRUE(cell6(v6)(v4)(v2) == 4.5)
	CHECK_TRUE(cell6(v6)(v4)(v3)[0] == 'e')
	CHECK_TRUE(cell6(v6)(v4)(v3)[1] == 'f')
	CHECK_TRUE(cell6(v6)(v5)(v1) == 7)
	CHECK_TRUE(cell6(v6)(v5)(v4)(v1) == 8)
	CHECK_TRUE(cell6(v6)(v5)(v4)(v2) == 8.5)
	CHECK_TRUE(cell6(v6)(v5)(v4)(v3)[0] == 'g')
	CHECK_TRUE(cell6(v6)(v5)(v4)(v3)[1] == 'h')


	return EXIT_SUCCESS;
}
