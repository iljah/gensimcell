/*
Parallel program playing Conway's Game of Life.

Copyright (c) 2014, Ilja Honkonen
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
*/

#include "array"
#include "boost/lexical_cast.hpp"
#include "cmath"
#include "cstdlib"
#include "iostream"
#include "mpi.h"

#include "dccrg.hpp"
#include "dccrg_cartesian_geometry.hpp"
#include "gensimcell.hpp"

#include "gol_initialize.hpp"
#include "gol_save.hpp"
#include "gol_solve.hpp"
#include "gol_variables.hpp"

int main(int argc, char* argv[])
{
	// the cell type used by this program
	using Cell = gol::Cell;

	/*
	Set up MPI
	*/
	if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
		std::cerr << "Coudln't initialize MPI." << std::endl;
		abort();
	}

	MPI_Comm comm = MPI_COMM_WORLD;

	int rank = 0, comm_size = 0;
	MPI_Comm_rank(comm, &rank);
	MPI_Comm_size(comm, &comm_size);


	/*
	Set up the grid in which the game will run
	*/
	dccrg::Dccrg<Cell, dccrg::Cartesian_Geometry> grid;

	// initialize the grid
	std::array<uint64_t, 3> grid_length = {20, 20, 1};
	const unsigned int neighborhood_size = 1;
	if (not grid.initialize(
		grid_length,
		comm,
		"RANDOM",
		neighborhood_size,
		0,
		false, false, false
	)) {
		std::cerr << __FILE__ << ":" << __LINE__
			<< ": Couldn't initialize grid."
			<< std::endl;
		abort();
	}

	// set the grid's geometry
	dccrg::Cartesian_Geometry::Parameters geom_params;
	geom_params.start[0] =
	geom_params.start[1] = -1;
	geom_params.start[2] = -1.0 / grid_length[0];
	geom_params.level_0_cell_length[0] =
	geom_params.level_0_cell_length[1] =
	geom_params.level_0_cell_length[2] = 2.0 / grid_length[0];
	if (not grid.set_geometry(geom_params)) {
		std::cerr << __FILE__ << ":" << __LINE__
			<< ": Couldn't set grid geometry."
			<< std::endl;
		abort();
	}

	grid.balance_load();

	/*
	Simulate
	*/

	gol::initialize<
		Cell,
		gol::Is_Alive,
		gol::Live_Neighbors
	>(grid);

	const std::vector<uint64_t>
		inner_cells = grid.get_local_cells_not_on_process_boundary(),
		outer_cells = grid.get_local_cells_on_process_boundary();

	double
		simulation_time = 0,
		time_step = 0.1;
	while (simulation_time <= M_PI) {

		/*
		Save the simulation to disk.
		save() itself decides what to "transfer" to the file so
		switch off all transfers before and restore them after.
		*/
		Cell::set_transfer_all(
			false,
			gol::Is_Alive(),
			gol::Live_Neighbors()
		);

		gol::save<Cell, gol::Is_Alive>(grid, simulation_time);

		if (simulation_time >= M_PI) {
			// don't simulate an extra step, e.g. if only initial state needed 
			break;
		}

		// start updating data required by the solver between processes
		Cell::set_transfer_all(true, gol::Is_Alive());
		grid.start_remote_neighbor_copy_updates();

		/*
		While transfers are executing solve cells for
		which data is not required from other processes.
		*/
		gol::solve<
			Cell,
			gol::Is_Alive,
			gol::Live_Neighbors
		>(inner_cells, grid);

		// wait for the required data to arrive
		grid.wait_remote_neighbor_copy_update_receives();

		// solve the rest of local cells
		gol::solve<
			Cell,
			gol::Is_Alive,
			gol::Live_Neighbors
		>(outer_cells, grid);

		/*
		Set the new state of cells whose data wasn't
		required by other processes
		*/
		gol::apply_solution<
			Cell,
			gol::Is_Alive,
			gol::Live_Neighbors
		>(inner_cells, grid);

		/*
		Wait for required data to arrive to other
		processes before setting the new state to
		those local cells
		*/
		grid.wait_remote_neighbor_copy_update_sends();

		gol::apply_solution<
			Cell,
			gol::Is_Alive,
			gol::Live_Neighbors
		>(outer_cells, grid);

		simulation_time += time_step;
	}

	MPI_Finalize();

	return EXIT_SUCCESS;
}
