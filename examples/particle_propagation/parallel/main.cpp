/*
Copyright (c) 2014, Ilja Honkonen
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

* Neither the name of NASA nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

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

//! see ../serial.cpp and ../advection/parallel/* for basics

#include "array"
#include "boost/lexical_cast.hpp"
#include "cmath"
#include "cstdlib"
#include "iostream"
#include "mpi.h"

#include "dccrg.hpp"
#include "dccrg_cartesian_geometry.hpp"
#include "gensimcell.hpp"

#include "particle_initialize.hpp"
#include "particle_save.hpp"
#include "particle_solve.hpp"
#include "particle_variables.hpp"

int main(int argc, char* argv[])
{
	using Cell = particle::Cell;

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

	particle::initialize<
		Cell,
		particle::Number_Of_Particles,
		particle::Particle_Destinations,
		particle::Velocity,
		particle::Internal_Particles
	>(grid);

	const std::vector<uint64_t>
		inner_cells = grid.get_local_cells_not_on_process_boundary(),
		outer_cells = grid.get_local_cells_on_process_boundary();

	const double particle_save_interval = 0.1;

	double
		simulation_time = 0,
		time_step = 0,
		particle_next_save = 0;
	while (simulation_time <= M_PI) {

		double next_time_step = std::numeric_limits<double>::max();

		/*
		Save simulation to disk
		*/
		if (particle_next_save <= simulation_time) {
			particle_next_save += particle_save_interval;

			particle::transfer_none<
				Cell,
				particle::Number_Of_Particles,
				particle::Particle_Destinations,
				particle::Velocity,
				particle::Internal_Particles,
				particle::External_Particles
			>();

			particle::save<
				Cell,
				particle::Number_Of_Particles,
				particle::Velocity,
				particle::Internal_Particles
			>(grid, simulation_time);

			particle::transfer_all<
				Cell,
				particle::Number_Of_Particles,
				particle::Particle_Destinations,
				particle::Velocity,
				particle::External_Particles
			>();
		}

		if (simulation_time >= M_PI) {
			break;
		}

		/*
		Solve
		*/

		next_time_step
			= std::min(
				next_time_step,
				particle::solve<
					Cell,
					particle::Number_Of_Particles,
					particle::Particle_Destinations,
					particle::Velocity,
					particle::Internal_Particles,
					particle::External_Particles
				>(time_step, inner_cells, grid)
			);

		next_time_step
			= std::min(
				next_time_step,
				particle::solve<
					Cell,
					particle::Number_Of_Particles,
					particle::Particle_Destinations,
					particle::Velocity,
					particle::Internal_Particles,
					particle::External_Particles
				>(time_step, outer_cells, grid)
			);

		// update particle counts so space can be allocated by receiving cells
		particle::transfer_none<
			Cell,
			particle::Number_Of_Particles,
			particle::Particle_Destinations,
			particle::Velocity,
			particle::Internal_Particles,
			particle::External_Particles
		>();
		Cell::set_transfer_all(particle::Number_Of_Particles(), true);
		grid.start_remote_neighbor_copy_updates();
		// wait for particle count transfers to complete and allocate
		grid.wait_remote_neighbor_copy_updates();
		particle::resize_receiving_containers<
			Cell,
			particle::Number_Of_Particles,
			particle::External_Particles
		>(grid);

		// start updating particle coordinates
		particle::transfer_all<
			Cell,
			particle::Number_Of_Particles,
			particle::Particle_Destinations,
			particle::Velocity,
			particle::External_Particles
		>();
		Cell::set_transfer_all(particle::Number_Of_Particles(), false);
		grid.start_remote_neighbor_copy_updates();
		grid.wait_remote_neighbor_copy_update_receives();
		grid.wait_remote_neighbor_copy_update_sends();

		/*
		Apply solution
		*/
		particle::move_particles<
			Cell,
			particle::Particle_Destinations,
			particle::Internal_Particles,
			particle::External_Particles
		>(inner_cells, grid);

		particle::move_particles<
			Cell,
			particle::Particle_Destinations,
			particle::Internal_Particles,
			particle::External_Particles
		>(outer_cells, grid);

		particle::remove_external_particles<
			Cell,
			particle::Number_Of_Particles,
			particle::Particle_Destinations,
			particle::External_Particles
		>(inner_cells, grid);

		particle::remove_external_particles<
			Cell,
			particle::Number_Of_Particles,
			particle::Particle_Destinations,
			particle::External_Particles
		>(outer_cells, grid);

		simulation_time += time_step;

		MPI_Allreduce(&next_time_step, &time_step, 1, MPI_DOUBLE, MPI_MIN, comm);
		const double CFL = 0.5;
		time_step *= CFL;
	}

	MPI_Finalize();

	return EXIT_SUCCESS;
}
