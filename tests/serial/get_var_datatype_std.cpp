#include "array"
#include "complex"
#include "cstdlib"
#include "iostream"
#include "mpi.h"

#include "check_true.hpp"
#include "get_var_datatype.hpp"

using namespace std;

int main(int argc, char* argv[]) {

	void* address = NULL;
	int count = -1;
	MPI_Datatype datatype = MPI_DATATYPE_NULL;


	bool b;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_datatype(b);
	CHECK_TRUE(
		address == &b
		and count == 1
		and datatype == MPI_CXX_BOOL
	)


	char c;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_datatype(c);
	CHECK_TRUE(
		address == &c
		and count == 1
		and datatype == MPI_CHAR
	)


	signed long long int i;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_datatype(i);
	CHECK_TRUE(
		address == &i
		and count == 1
		and datatype == MPI_LONG_LONG_INT
	)


	std::complex<long double> d;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_datatype(d);
	CHECK_TRUE(
		address == &d
		and count == 1
		and datatype == MPI_CXX_LONG_DOUBLE_COMPLEX
	)


	std::array<char, 5> e;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_datatype(e);
	CHECK_TRUE(
		address == e.data()
		and count == 5
		and datatype == MPI_CHAR
	)


	std::array<std::complex<double>, 3> f;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_datatype(f);
	CHECK_TRUE(
		address == f.data()
		and count == 3
		and datatype == MPI_CXX_DOUBLE_COMPLEX
	)


	return EXIT_SUCCESS;
};
