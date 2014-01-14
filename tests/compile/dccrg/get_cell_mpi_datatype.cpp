#include "cstdint"
#include "mpi.h"
#include "tuple"

#include "dccrg.hpp"
#include "gensimcell.hpp"

struct variable_t {
	typedef int data_type;
};

typedef gensimcell::Cell<variable_t> cell_t;

int main(int, char**)
{
	cell_t cell;

	void* address;
	int count;
	MPI_Datatype datatype;

	std::tie(
		address,
		count,
		datatype
	) = dccrg::detail::get_cell_mpi_datatype(
		cell,
		0,
		0,
		0,
		false,
		0
	);

	return 0;
}
