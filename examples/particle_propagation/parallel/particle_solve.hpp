/*
Solver for parallel particle propagator.

Copyright (c) 2013, 2014, Ilja Honkonen
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

#ifndef PARTICLE_SOLVE_HPP
#define PARTICLE_SOLVE_HPP


#include "cstdlib"
#include "iostream"
#include "vector"

#include "dccrg.hpp"
#include "dccrg_cartesian_geometry.hpp"

#include "gensimcell.hpp"


//! see ../serial.cpp for the basics


namespace particle {


/*!
Allocates space in each copy of a remote neighbor
in given grid to hold as many particles as given by
the copy's Number_Of_Particles_T variable.
*/
template<
	class Cell_T,
	class Number_Of_Particles_T,
	class External_Particles_T
> void resize_receiving_containers(
	dccrg::Dccrg<Cell_T, dccrg::Cartesian_Geometry>& grid
) {
	const auto remote_cells = grid.get_remote_cells_on_process_boundary();
	for (auto cell_id: remote_cells) {
		Cell_T* data = grid[cell_id];
		if (data == NULL) {
			std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
			abort();
		}

		(*data)[External_Particles_T()]
			.resize((*data)[Number_Of_Particles_T()]);
	}
}


/*!
Propagates particles in given cells for a given amount of time.

Returns the longest allowed time step for given cells
and their neighbors. Particles which propagate outside of the
cell in which they are stored are moved to the External_Particles_T
list of their previous cell and added to Particle_Destinations_T
information.
*/
template<
	class Cell_T,
	class Number_Of_Particles_T,
	class Particle_Destinations_T,
	class Velocity_T,
	class Internal_Particles_T,
	class External_Particles_T
> double solve(
	const double dt,
	const std::vector<uint64_t>& cell_ids,
	dccrg::Dccrg<Cell_T, dccrg::Cartesian_Geometry>& grid
) {
	double max_time_step = std::numeric_limits<double>::max();

	const auto
		grid_start = grid.geometry.get_start(),
		grid_end = grid.geometry.get_end();

	// propagate particles and move maybe from internal to external list
	for (auto cell_id: cell_ids) {

		Cell_T* cell_data = grid[cell_id];
		if (cell_data == NULL) {
			std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
			abort();
		}

		// shorthand notation
		const auto& v = (*cell_data)[Velocity_T()];
		auto& destinations = (*cell_data)[Particle_Destinations_T()];
		auto
			&int_coords = (*cell_data)[Internal_Particles_T()].coordinates,
			&ext_coords = (*cell_data)[External_Particles_T()].coordinates;

		// propagate particles
		const auto
			cell_min = grid.geometry.get_min(cell_id),
			cell_max = grid.geometry.get_max(cell_id);

		for (size_t i = 0; i < int_coords.size(); i++) {
			auto& coordinate = int_coords[i];

			coordinate[0] += v[0] * dt;
			coordinate[1] += v[1] * dt;

			// handle periodic grid
			coordinate = grid.geometry.get_real_coordinate(coordinate);

			if (
				coordinate[0] < cell_min[0]
				or coordinate[0] > cell_max[0]
				or coordinate[1] < cell_min[1]
				or coordinate[1] > cell_max[1]
				or coordinate[2] < cell_min[2]
				or coordinate[2] > cell_max[2]
			) {
				ext_coords.push_back(coordinate);
				int_coords.erase(int_coords.begin() + i);
				i--;
			}
		}

		/*
		Designate particles to a neighbor cell. Particles must
		be listed contiguously for each destination so sort
		them first by destination cell.
		*/
		std::unordered_map<
			uint64_t,
			typename External_Particles_T::data_type::storage_type
		> sorted_destinations;

		const auto* const neighbors = grid.get_neighbors_of(cell_id);
		if (neighbors == NULL) {
			std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
			abort();
		}

		for (const auto& ext_coord: ext_coords) {
			bool destination_found = false;

			for (auto neighbor_id: *neighbors) {
				if (neighbor_id == dccrg::error_cell) {
					continue;
				}

				const auto
					neighbor_min = grid.geometry.get_min(neighbor_id),
					neighbor_max = grid.geometry.get_max(neighbor_id);

				if (
					ext_coord[0] >= neighbor_min[0]
					and ext_coord[0] <= neighbor_max[0]
					and ext_coord[1] >= neighbor_min[1]
					and ext_coord[1] <= neighbor_max[1]
					and ext_coord[2] >= neighbor_min[2]
					and ext_coord[2] <= neighbor_max[2]
				) {
					destination_found = true;
					sorted_destinations[neighbor_id].push_back(ext_coord);
					break;
				}
			}

			if (not destination_found) {
				// don't lose any particles
				if (
					ext_coord[0] >= grid_start[0]
					and ext_coord[0] <= grid_end[0]
					and ext_coord[1] >= grid_start[1]
					and ext_coord[1] <= grid_end[1]
					and ext_coord[2] >= grid_start[2]
					and ext_coord[2] <= grid_end[2]
				) {
					int_coords.push_back(ext_coord);
				}
			}
		}


		// create the final external particle list and destination info
		ext_coords.clear();

		size_t destination_i = 0;
		for (const auto& item: sorted_destinations) {

			destinations[destination_i] = item.first;
			destinations[destination_i + 1] = item.second.size();
			destination_i += 2;

			ext_coords.insert(
				ext_coords.end(),
				item.second.cbegin(),
				item.second.cend()
			);
		}
		(*cell_data)[Number_Of_Particles_T()] = ext_coords.size();

		// check time step
		const auto length = grid.geometry.get_length(cell_id);
		max_time_step =
			std::min(max_time_step,
			std::min(
				fabs(length[0] / v[0]),
				fabs(length[1] / v[1])
			));
	}

	return max_time_step;
}


/*!
Copies particles from the external particle lists of neighbors
of given cells into internal particle lists of given cells.
*/
template<
	class Cell_T,
	class Particle_Destinations_T,
	class Internal_Particles_T,
	class External_Particles_T
> void incorporate_external_particles(
	const std::vector<uint64_t>& cell_ids,
	dccrg::Dccrg<Cell_T, dccrg::Cartesian_Geometry>& grid
) {
	for (auto cell_id: cell_ids) {

		Cell_T* cell_data = grid[cell_id];
		if (cell_data == NULL) {
			std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
			abort();
		}

		auto& int_coords = (*cell_data)[Internal_Particles_T()].coordinates;

		// assign some particles from neighbors' external list to this cell
		const std::vector<uint64_t>* const neighbors = grid.get_neighbors_of(cell_id);
		if (neighbors == NULL) {
			std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
			abort();
		}

		for (auto neighbor_id: *neighbors) {
			if (neighbor_id == dccrg::error_cell) {
				continue;
			}

			Cell_T* neighbor_data = grid[neighbor_id];
			if (neighbor_data == NULL) {
				std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
				abort();
			}

			const auto& destinations = (*neighbor_data)[Particle_Destinations_T()];
			const auto& ext_coords = (*neighbor_data)[External_Particles_T()].coordinates;

			for (size_t
				dest_i = 0, coord_i = 0;
				dest_i < destinations.size();
				dest_i += 2
			) {

				const uint64_t
					destination_cell = destinations[dest_i],
					nr_of_particles = destinations[dest_i + 1];

				if (destination_cell == cell_id) {
					int_coords.insert(
						int_coords.end(),
						ext_coords.begin() + coord_i,
						ext_coords.begin() + coord_i + nr_of_particles
					);
				}

				coord_i += nr_of_particles;
			}

		}
	}
}


/*!
Removes particles from the external particle list of given cells.

Also clears particle destinations of given cells and updates number_of_particles.
*/
template<
	class Cell_T,
	class Number_Of_Particles_T,
	class Particle_Destinations_T,
	class External_Particles_T
> void remove_external_particles(
	const std::vector<uint64_t>& cell_ids,
	dccrg::Dccrg<Cell_T, dccrg::Cartesian_Geometry>& grid
) {
	for (auto cell_id: cell_ids) {

		Cell_T* cell_data = grid[cell_id];
		if (cell_data == NULL) {
			std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
			abort();
		}

		(*cell_data)[Number_Of_Particles_T()] = 0;
		(*cell_data)[Particle_Destinations_T()].fill(dccrg::error_cell);
		(*cell_data)[External_Particles_T()].coordinates.clear();
	}
}


} // namespace

#endif // ifndef PARTICLE_SOLVE_HPP
