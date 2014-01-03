#include "boost/logic/tribool.hpp"
#include "cstdlib"
#include "iostream"

#include "check_true.hpp"
#include "gensimcell.hpp"


struct test_variable1 {
	typedef int data_type;
};

struct test_variable2 {
	typedef float data_type;
};

struct test_variable3 {
	typedef char data_type;
};

typedef gensimcell::Cell<
	test_variable1,
	test_variable2,
	test_variable3
> cell1_t;


int main(int, char**)
{
	const test_variable1 v1;
	const test_variable2 v2;
	const test_variable3 v3;

	cell1_t c1;
	// check defaults of middle variable
	CHECK_TRUE(cell1_t::get_transfer_all(v2))
	CHECK_TRUE(c1.get_transfer_all(v2))
	CHECK_TRUE(c1.get_transfer(v2))
	CHECK_TRUE(c1.is_transferred(v2))

	// check that variables don't affect each other
	c1.set_transfer_all(v1, false);
	CHECK_TRUE(c1.is_transferred(v2))
	CHECK_TRUE(c1.is_transferred(v3))

	c1.set_transfer_all(v1, boost::logic::indeterminate);
	c1.set_transfer_all(v2, boost::logic::indeterminate);
	c1.set_transfer_all(v3, boost::logic::indeterminate);

	c1.set_transfer(v2, false);
	c1.set_transfer(v3, false);
	CHECK_TRUE(c1.is_transferred(v1))
	CHECK_TRUE(not c1.is_transferred(v2))
	CHECK_TRUE(not c1.is_transferred(v3))

	return EXIT_SUCCESS;
}
