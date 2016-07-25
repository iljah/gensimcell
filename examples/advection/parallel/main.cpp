/*
Parallel program solving advection equation in a prescribed velocity field.

Copyright 2014, 2015, 2016 Ilja Honkonen
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

//! see ../serial.cpp and ../game_of_life/parallel/* for basics

#include "array"
#include "boost/lexical_cast.hpp"
#include "cmath"
#include "cstdlib"
#include "iostream"
#include "mpi.h"

#include "dccrg.hpp"
#include "dccrg_cartesian_geometry.hpp"
#include "gensimcell.hpp"

#include "advection_initialize.hpp"
#include "advection_save.hpp"
#include "advection_solve.hpp"
#include "advection_variables.hpp"

int main(int argc, char* argv[])
{
	// the cell type used by this program
	using Cell = advection::Cell;

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


	// intialize Zoltan
	float zoltan_version;
	if (Zoltan_Initialize(argc, argv, &zoltan_version) != ZOLTAN_OK) {
		std::cerr << "Zoltan_Initialize failed." << std::endl;
		abort();
	}


	/*
	Set up the grid in which the simulation will run
	*/
	dccrg::Dccrg<Cell, dccrg::Cartesian_Geometry> grid;

	// initialize the grid
	std::array<uint64_t, 3> grid_length = {{20, 20, 1}};
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
	advection::initialize<
		Cell,
		advection::Density,
		advection::Density_Flux,
		advection::Velocity
	>(grid);

	const std::vector<uint64_t>
		inner_cells = grid.get_local_cells_not_on_process_boundary(),
		outer_cells = grid.get_local_cells_on_process_boundary();

	const double advection_save_interval = 0.1;

	double advection_next_save = 0;

	double
		simulation_time = 0,
		time_step = 0;
	while (simulation_time <= M_PI) {

		double next_time_step = std::numeric_limits<double>::max();

		/*
		Save simulation to disk
		*/
		Cell::set_transfer_all(
			false,
			advection::Density(),
			advection::Density_Flux(),
			advection::Velocity()
		);

		if (advection_next_save <= simulation_time) {
			advection_next_save += advection_save_interval;

			advection::save<
				Cell,
				advection::Density,
				advection::Velocity
			>(grid, simulation_time);
		}


		if (simulation_time >= M_PI) {
			break;
		}


		/*
		Solve
		*/
		Cell::set_transfer_all(
			true,
			advection::Density(),
			advection::Velocity()
		);
		grid.start_remote_neighbor_copy_updates();

		next_time_step
			= std::min(
				next_time_step,
				advection::solve<
					Cell,
					advection::Density,
					advection::Density_Flux,
					advection::Velocity
				>(time_step, inner_cells, grid)
			);

		grid.wait_remote_neighbor_copy_update_receives();

		next_time_step
			= std::min(
				next_time_step,
				advection::solve<
					Cell,
					advection::Density,
					advection::Density_Flux,
					advection::Velocity
				>(time_step, outer_cells, grid)
			);

		/*
		Apply solution
		*/
		advection::apply_solution<
			Cell,
			advection::Density,
			advection::Density_Flux
		>(inner_cells, grid);

		grid.wait_remote_neighbor_copy_update_sends();
		Cell::set_transfer_all(
			false,
			advection::Density(),
			advection::Velocity()
		);

		advection::apply_solution<
			Cell,
			advection::Density,
			advection::Density_Flux
		>(outer_cells, grid);

		simulation_time += time_step;

		MPI_Allreduce(&next_time_step, &time_step, 1, MPI_DOUBLE, MPI_MIN, comm);
		const double CFL = 0.5;
		time_step *= CFL;
	}

	MPI_Finalize();

	return EXIT_SUCCESS;
}
