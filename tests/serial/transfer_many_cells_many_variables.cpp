#include "boost/logic/tribool.hpp"
#include "cstdlib"

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
	test_variable2
> cell1_t;

typedef gensimcell::Cell<
	test_variable2,
	test_variable3
> cell2_t;

typedef gensimcell::Cell<
	test_variable1,
	test_variable2,
	test_variable3
> cell3_t;


int main(int, char**)
{
	const test_variable1 v1;
	const test_variable2 v2;
	const test_variable3 v3;

	cell1_t c1_1, c1_2;
	cell2_t c2_1, c2_2;
	cell3_t c3_1, c3_2;

	// check default properties
	CHECK_TRUE(not c3_1.get_transfer_all(v1))
	CHECK_TRUE(not c3_1.get_transfer(v1))
	CHECK_TRUE(not c3_1.is_transferred(v1))
	CHECK_TRUE(not c3_2.get_transfer_all(v1))
	CHECK_TRUE(not c3_2.get_transfer(v1))
	CHECK_TRUE(not c3_2.is_transferred(v1))
	CHECK_TRUE(not c3_1.get_transfer_all(v2))
	CHECK_TRUE(not c3_1.get_transfer(v2))
	CHECK_TRUE(not c3_1.is_transferred(v2))
	CHECK_TRUE(not c3_2.get_transfer_all(v2))
	CHECK_TRUE(not c3_2.get_transfer(v2))
	CHECK_TRUE(not c3_2.is_transferred(v2))
	CHECK_TRUE(not c3_1.get_transfer_all(v3))
	CHECK_TRUE(not c3_1.get_transfer(v3))
	CHECK_TRUE(not c3_1.is_transferred(v3))
	CHECK_TRUE(not c3_2.get_transfer_all(v3))
	CHECK_TRUE(not c3_2.get_transfer(v3))
	CHECK_TRUE(not c3_2.is_transferred(v3))

	// test static variable logic
	c1_1.set_transfer_all(true, v2);
	CHECK_TRUE(c1_1.is_transferred(v2))
	CHECK_TRUE(c1_2.is_transferred(v2))
	CHECK_TRUE(not c2_1.is_transferred(v2))
	CHECK_TRUE(not c2_2.is_transferred(v2))
	CHECK_TRUE(not c3_1.is_transferred(v2))
	CHECK_TRUE(not c3_2.is_transferred(v2))

	// test combinations of static and cell specific logic
	c3_1.set_transfer_all(boost::logic::indeterminate, v2);
	c3_1.set_transfer(true, v2);
	CHECK_TRUE(c3_1.is_transferred(v2))
	CHECK_TRUE(not c3_2.is_transferred(v2))

	c2_2.set_transfer(true, v3);
	c2_2.set_transfer_all(boost::logic::indeterminate, v3);
	c3_1.set_transfer_all(boost::logic::indeterminate, v3);
	c3_1.set_transfer(false, v3);
	c3_2.set_transfer(true, v3);
	CHECK_TRUE(not c2_1.is_transferred(v3))
	CHECK_TRUE(c2_2.is_transferred(v3))
	CHECK_TRUE(not c3_1.is_transferred(v3))
	CHECK_TRUE(c3_2.is_transferred(v3))


	return EXIT_SUCCESS;
}
