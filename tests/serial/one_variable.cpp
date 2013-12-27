#include "array"
#include "cstdlib"
#include "iostream"
#include "tuple"
#include "vector"

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
	gensimcell::Cell<test_variable1> cell;
	cell(test_variable1()) = 3;
	if (cell(test_variable1()) != 3) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		abort();
	}


	array<gensimcell::Cell<test_variable1>, 3> cell_array;
	cell_array[0](test_variable1()) = -1;
	cell_array[1](test_variable1()) = -2;
	cell_array[2](test_variable1()) = -3;
	if (cell_array[0](test_variable1()) != -1) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		abort();
	}
	if (cell_array[1](test_variable1()) != -2) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		abort();
	}
	if (cell_array[2](test_variable1()) != -3) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		abort();
	}


	tuple<
		gensimcell::Cell<test_variable1>,
		gensimcell::Cell<test_variable2>,
		gensimcell::Cell<test_variable3>
	> cell_tuple;
	get<0>(cell_tuple)(test_variable1()) = -10;
	get<1>(cell_tuple)(test_variable2()) = -0.5;
	get<2>(cell_tuple)(test_variable3()) = '3';
	if (get<0>(cell_tuple)(test_variable1()) != -10) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		abort();
	}
	if (get<1>(cell_tuple)(test_variable2()) != -0.5) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		abort();
	}
	if (get<2>(cell_tuple)(test_variable3()) != '3') {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		abort();
	}


	vector<gensimcell::Cell<test_variable1>> cell_vector(3);
	cell_vector[0](test_variable1()) = 100.1;
	cell_vector[1](test_variable1()) = 200.3;
	cell_vector[2](test_variable1()) = 300.6;
	if (cell_vector[0](test_variable1()) != 100) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		abort();
	}
	if (cell_vector[1](test_variable1()) != 200) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		abort();
	}
	if (cell_vector[2](test_variable1()) != 300) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		abort();
	}


	return EXIT_SUCCESS;
}
