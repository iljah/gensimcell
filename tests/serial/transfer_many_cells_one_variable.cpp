#include "boost/logic/tribool.hpp"
#include "cstdlib"

#include "check_true.hpp"
#include "gensimcell.hpp"


struct test_variable1 {
	typedef int data_type;
};

typedef gensimcell::Cell<test_variable1> cell1_t;


int main(int argc, char* argv[])
{
	const test_variable1 v1;
	cell1_t c1_1, c1_2, c1_3;

	// check default properties
	CHECK_TRUE(cell1_t::get_transfer_all(v1))
	CHECK_TRUE(c1_1.get_transfer(v1))
	CHECK_TRUE(c1_1.is_transferred(v1))
	CHECK_TRUE(c1_2.get_transfer(v1))
	CHECK_TRUE(c1_2.is_transferred(v1))
	CHECK_TRUE(c1_3.get_transfer(v1))
	CHECK_TRUE(c1_3.is_transferred(v1))

	// test logic between static and instance specific transfers
	c1_1.set_transfer_all(v1, false);
	CHECK_TRUE(not c1_2.get_transfer_all(v1))
	CHECK_TRUE(not c1_3.get_transfer_all(v1))
	CHECK_TRUE(not c1_2.is_transferred(v1))
	CHECK_TRUE(not c1_3.is_transferred(v1))

	c1_2.set_transfer_all(v1, boost::logic::indeterminate);
	c1_2.set_transfer(v1, false);
	CHECK_TRUE(c1_1.is_transferred(v1))
	CHECK_TRUE(not c1_2.is_transferred(v1))
	CHECK_TRUE(c1_3.is_transferred(v1))

	c1_3.set_transfer(v1, false);
	CHECK_TRUE(c1_1.is_transferred(v1))
	CHECK_TRUE(not c1_2.is_transferred(v1))
	CHECK_TRUE(not c1_3.is_transferred(v1))


	return EXIT_SUCCESS;
}
