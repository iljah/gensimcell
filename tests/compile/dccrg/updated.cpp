#include "mpi.h"

#include "dccrg.hpp"
#include "gensimcell.hpp"

struct variable_t {
	typedef int data_type;
};

typedef gensimcell::Cell<variable_t> cell_t;

int main(int, char**)
{
	dccrg::Dccrg<cell_t> grid;
	MPI_Comm comm = MPI_COMM_WORLD;
	grid.initialize({1, 1, 1}, comm, "RANDOM", 0, 0, false, false, false);
	grid.update_copies_of_remote_neighbors();
	return 0;
}
