/*
Parallel program playing Conway's Game of Life that requires only MPI.

Copyright 2015 Ilja Honkonen
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

* Neither the name of copyright holders nor the names of their contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


To keep it simple use 1d grid and one cell / process.
*/

#include "cstdlib"
#include "iostream"
#include "tuple"
#include "vector"

#include "mpi.h" // must be included before gensimcell.hpp
#include "gensimcell.hpp"

using namespace std;


struct Is_Alive{ using data_type = bool; };

struct Live_Neighbors { using data_type = int; };

using Cell_T = gensimcell::Cell<
	gensimcell::Optional_Transfer,
	Is_Alive,
	Live_Neighbors
>;


/*
MPI rank 0 prints state of all cells to standard output.

Takes as input cell data of the cell owned by current rank.
*/
void print_game(const Cell_T& cell, const int rank, const int comm_size)
{
	// vector<bool> is special and cannot be used for MPI gather
	vector<int> alives(comm_size);
	int alive = 0;

	if (cell[Is_Alive()]) {
		alive = 1;
	}

	MPI_Gather(
		&alive,
		1,
		MPI_INT,
		alives.data(),
		1,
		MPI_INT,
		0,
		MPI_COMM_WORLD
	);

	if (rank == 0) {
		for (auto alive: alives) {
			if (alive > 0) {
				cout << "0";
			} else {
				cout << ".";
			}
		}
		cout << endl;
	}
}


int main(int argc, char* argv[])
{
	const Is_Alive is_alive{};
	const Live_Neighbors live_neighbors{};

	if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
		cerr << "Coudln't initialize MPI." << endl;
		abort();
	}

	MPI_Comm comm = MPI_COMM_WORLD;

	int rank = 0, comm_size = 0;
	MPI_Comm_rank(comm, &rank);
	MPI_Comm_size(comm, &comm_size);

	Cell_T
		// each process has one cell
		cell,
		// copies of above cell's neighbors' data in both directions
		neg_neigh, pos_neigh;

	// tranfer only is_alive, all cells return precommitted MPI_Datatype
	Cell_T::set_transfer_all(true, is_alive);

	// initial condition
	cell[is_alive] = true;
	cell[live_neighbors] = 0;
	if (rank == 0) {
		cell[is_alive] = false;
	}

	print_game(cell, rank, comm_size);

	constexpr size_t max_turns = 10;
	for (size_t turn = 0; turn < max_turns; turn++) {

		// MPI transfer info of cell and its neighbors
		tuple<void*, int, MPI_Datatype>
			cell_info, neg_info, pos_info;

		cell_info = cell.get_mpi_datatype();
		neg_info = neg_neigh.get_mpi_datatype();
		pos_info = pos_neigh.get_mpi_datatype();

		// update variables between neighboring cells
		MPI_Request neg_send, pos_send, neg_recv, pos_recv;
		MPI_Irecv(
			get<0>(neg_info), get<1>(neg_info), get<2>(neg_info),
			int(unsigned(rank + comm_size - 1) % comm_size),
			int(unsigned(rank + comm_size - 1) % comm_size),
			MPI_COMM_WORLD,
			&neg_recv
		);
		MPI_Irecv(
			get<0>(pos_info), get<1>(pos_info), get<2>(pos_info),
			int(unsigned(rank + 1) % comm_size),
			int(unsigned(rank + 1) % comm_size),
			MPI_COMM_WORLD,
			&pos_recv
		);
		MPI_Isend(
			get<0>(cell_info), get<1>(cell_info), get<2>(cell_info),
			int(unsigned(rank + comm_size - 1) % comm_size),
			rank,
			MPI_COMM_WORLD,
			&neg_send
		);
		MPI_Isend(
			get<0>(cell_info), get<1>(cell_info), get<2>(cell_info),
			int(unsigned(rank + 1) % comm_size),
			rank,
			MPI_COMM_WORLD,
			&pos_send
		);

		MPI_Wait(&neg_recv, MPI_STATUS_IGNORE);
		MPI_Wait(&pos_recv, MPI_STATUS_IGNORE);

		if (neg_neigh[is_alive]) cell[live_neighbors]++;
		if (pos_neigh[is_alive]) cell[live_neighbors]++;

		MPI_Wait(&neg_send, MPI_STATUS_IGNORE);
		MPI_Wait(&pos_send, MPI_STATUS_IGNORE);

		if (cell[live_neighbors] == 2) {
			cell[is_alive] = true;
		} else {
			cell[is_alive] = false;
		}
		cell[live_neighbors] = 0;

		print_game(cell, rank, comm_size);
	}

	MPI_Finalize();

	return EXIT_SUCCESS;
}
