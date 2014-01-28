#include "boost/logic/tribool.hpp"
#include "cstdlib"
#include "iostream"

#include "check_true.hpp"
#include "gensimcell.hpp"


struct test_variable1 {
	typedef int data_type;
};

typedef gensimcell::Cell<test_variable1> cell1_t;


struct test_variable2 {
	typedef gensimcell::Cell<test_variable1> data_type;
};

typedef gensimcell::Cell<test_variable2> cell2_t;


struct test_variable3 {
	typedef cell1_t data_type;
};

typedef gensimcell::Cell<test_variable3> cell3_t;


typedef gensimcell::Cell<
	test_variable1,
	test_variable2,
	test_variable3
> cell4_t;


int main(int, char**)
{
	const test_variable1 v1;
	const test_variable2 v2;
	const test_variable3 v3;

	cell1_t c1;
	cell2_t c2;
	cell3_t c3;
	cell4_t c4;

	// check default properties
	CHECK_TRUE(not c4.is_transferred(v1))
	CHECK_TRUE(not c4.is_transferred(v2))
	CHECK_TRUE(not c4.is_transferred(v3))
	CHECK_TRUE(not c4(v2).is_transferred(v1))
	CHECK_TRUE(not c4(v3).is_transferred(v1))


	// check that static cell1_t logic affects all its users
	CHECK_TRUE(not c1.is_transferred(v1))
	CHECK_TRUE(not c2(v2).is_transferred(v1))
	CHECK_TRUE(not c3(v3).is_transferred(v1))
	c1.set_transfer_all(true, v1);
	CHECK_TRUE(c1.is_transferred(v1))
	CHECK_TRUE(c2(v2).is_transferred(v1))
	CHECK_TRUE(c3(v3).is_transferred(v1))
	CHECK_TRUE(c4(v2).is_transferred(v1))
	CHECK_TRUE(c4(v3).is_transferred(v1))

	// check cell/variable specific logic
	c1.set_transfer_all(boost::logic::indeterminate, v1);
	c2(v2).set_transfer(true, v1);
	CHECK_TRUE(c2(v2).is_transferred(v1))
	CHECK_TRUE(not c3(v3).is_transferred(v1))
	CHECK_TRUE(not c4(v2).is_transferred(v1))
	CHECK_TRUE(not c4(v3).is_transferred(v1))

	c4(v2).set_transfer(true, v1);
	CHECK_TRUE(c4(v2).is_transferred(v1))
	CHECK_TRUE(not c4(v3).is_transferred(v1))

	c1.set_transfer_all(false, v1);
	CHECK_TRUE(not c1.is_transferred(v1))
	CHECK_TRUE(not c2(v2).is_transferred(v1))
	CHECK_TRUE(not c3(v3).is_transferred(v1))
	CHECK_TRUE(not c4(v2).is_transferred(v1))
	CHECK_TRUE(not c4(v3).is_transferred(v1))

	c4.set_transfer_all(true, v1, v2, v3);
	CHECK_TRUE(c4.is_transferred(v1))
	CHECK_TRUE(c4.is_transferred(v2))
	CHECK_TRUE(c4.is_transferred(v3))

	c4(v3).set_transfer_all(true, v1, v1, v1);
	CHECK_TRUE(c4(v3).is_transferred(v1))


	return EXIT_SUCCESS;
}
