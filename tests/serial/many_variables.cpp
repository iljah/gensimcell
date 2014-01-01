#include "cstdlib"
#include "iostream"
#include "tuple"

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
	typedef char data_type;
};


int main(int argc, char* argv[])
{
	gensimcell::Cell<
		test_variable1,
		test_variable2,
		test_variable3
	> cell;
	cell(test_variable1()) = 3;
	cell(test_variable2()) = 1.5;
	cell(test_variable3()) = '3';
	CHECK_TRUE(cell(test_variable1()) == 3)
	CHECK_TRUE(cell(test_variable2()) == 1.5)
	CHECK_TRUE(cell(test_variable3()) == '3')


	tuple<
		gensimcell::Cell<
			test_variable1,
			test_variable2
		>,
		gensimcell::Cell<
			test_variable2,
			test_variable3
		>,
		gensimcell::Cell<
			test_variable1,
			test_variable2,
			test_variable3
		>
	> cell_tuple;
	get<0>(cell_tuple)(test_variable1()) = -10;
	get<0>(cell_tuple)(test_variable2()) = -2.5;
	get<1>(cell_tuple)(test_variable2()) = 2.5;
	get<1>(cell_tuple)(test_variable3()) = '4';
	get<2>(cell_tuple)(test_variable1()) = -20;
	get<2>(cell_tuple)(test_variable2()) = 4.5;
	get<2>(cell_tuple)(test_variable3()) = '5';
	CHECK_TRUE(get<0>(cell_tuple)(test_variable1()) == -10)
	CHECK_TRUE(get<0>(cell_tuple)(test_variable2()) == -2.5)
	CHECK_TRUE(get<1>(cell_tuple)(test_variable2()) == 2.5)
	CHECK_TRUE(get<1>(cell_tuple)(test_variable3()) == '4')
	CHECK_TRUE(get<2>(cell_tuple)(test_variable1()) == -20)
	CHECK_TRUE(get<2>(cell_tuple)(test_variable2()) == 4.5)
	CHECK_TRUE(get<2>(cell_tuple)(test_variable3()) == '5')


	return EXIT_SUCCESS;
}
