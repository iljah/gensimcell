#include "cstdlib"

#include "check_true.hpp"
#include "gensimcell.hpp"


struct test_variable1 {
	typedef int data_type;
};

typedef gensimcell::Cell<test_variable1> cell1_t;


int main(int, char**)
{
	const test_variable1 v1;
	cell1_t c1;

	// check default properties
	CHECK_TRUE(not cell1_t::get_transfer_all(v1))
	CHECK_TRUE(not c1.get_transfer_all(v1))
	CHECK_TRUE(not c1.get_transfer(v1))
	CHECK_TRUE(not c1.is_transferred(v1))

	// test logic between static and instance specific transfers
	c1.set_transfer(true, v1);
	CHECK_TRUE(not c1.is_transferred(v1))

	c1.set_transfer_all(boost::logic::indeterminate, v1);
	CHECK_TRUE(c1.is_transferred(v1))

	c1.set_transfer_all(true, v1);
	c1.set_transfer(false, v1);
	CHECK_TRUE(c1.is_transferred(v1))

	// check variadic version
	c1.set_transfer_all(false, v1, v1);
	CHECK_TRUE(not c1.is_transferred(v1))

	c1.set_transfer_all(boost::logic::indeterminate, v1, v1, v1);
	c1.set_transfer(true, v1, v1, v1, v1);
	CHECK_TRUE(c1.is_transferred(v1))

	c1.set_transfer(false, v1, v1, v1);
	CHECK_TRUE(not c1.is_transferred(v1))

	return EXIT_SUCCESS;
}
