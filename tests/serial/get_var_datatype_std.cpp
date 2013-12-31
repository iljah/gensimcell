#include "complex"
#include "cstdlib"
#include "iostream"
#include "mpi.h"

#include "get_var_datatype.hpp"

using namespace std;

int main(int argc, char* argv[]) {

	void* address = NULL;
	int count = -1;
	MPI_Datatype datatype = MPI_DATATYPE_NULL;


	bool b;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_datatype(b);
	if (
		address != &b
		or count != 1
		or datatype != MPI_CXX_BOOL
	) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		abort();
	}


	char c;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_datatype(c);
	if (
		address != &c
		or count != 1
		or datatype != MPI_CHAR
	) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		abort();
	}


	signed long long int i;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_datatype(i);
	if (
		address != &i
		or count != 1
		or datatype != MPI_LONG_LONG_INT
	) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		abort();
	}


	std::complex<long double> d;
	std::tie(address, count, datatype)
		= gensimcell::detail::get_var_datatype(d);
	if (
		address != &d
		or count != 1
		or datatype != MPI_CXX_LONG_DOUBLE_COMPLEX
	) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		abort();
	}


	return EXIT_SUCCESS;
};
