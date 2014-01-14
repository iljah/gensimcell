#include "dccrg.hpp"
#include "gensimcell.hpp"

struct cell_t {
	typedef int data_type;
};

int main(int, char**)
{
	dccrg::Dccrg<cell_t> grid;
	return 0;
}
