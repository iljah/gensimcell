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
	typedef char data_type;
};


int main(int, char**)
{
	gensimcell::Cell<test_variable1> cell;
	cell[test_variable1()] = 3;
	CHECK_TRUE(cell[test_variable1()] == 3)


	array<gensimcell::Cell<test_variable1>, 3> cell_array;
	cell_array[0][test_variable1()] = -1;
	cell_array[1][test_variable1()] = -2;
	cell_array[2][test_variable1()] = -3;
	CHECK_TRUE(cell_array[0][test_variable1()] == -1)
	CHECK_TRUE(cell_array[1][test_variable1()] == -2)
	CHECK_TRUE(cell_array[2][test_variable1()] == -3)


	tuple<
		gensimcell::Cell<test_variable1>,
		gensimcell::Cell<test_variable2>,
		gensimcell::Cell<test_variable3>
	> cell_tuple;
	get<0>(cell_tuple)[test_variable1()] = -10;
	get<1>(cell_tuple)[test_variable2()] = -0.5;
	get<2>(cell_tuple)[test_variable3()] = '3';
	CHECK_TRUE(get<0>(cell_tuple)[test_variable1()] == -10)
	CHECK_TRUE(get<1>(cell_tuple)[test_variable2()] == -0.5)
	CHECK_TRUE(get<2>(cell_tuple)[test_variable3()] == '3')


	vector<gensimcell::Cell<test_variable1>> cell_vector(3);
	cell_vector[0][test_variable1()] = 100.1;
	cell_vector[1][test_variable1()] = 200.3;
	cell_vector[2][test_variable1()] = 300.6;
	CHECK_TRUE(cell_vector[0][test_variable1()] == 100)
	CHECK_TRUE(cell_vector[1][test_variable1()] == 200)
	CHECK_TRUE(cell_vector[2][test_variable1()] == 300)


	return EXIT_SUCCESS;
}
