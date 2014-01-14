#include "array"
#include "mpi.h"

#include "dccrg.hpp"
#include "gensimcell.hpp"

struct cell_t {
	typedef int data_type;
};

int main(int, char**)
{
	dccrg::Dccrg<cell_t> grid;
	MPI_Comm comm = MPI_COMM_WORLD;
	grid.initialize({1, 1, 1}, comm, "RANDOM", 0, 0, false, false, false);
	return 0;
}
