/*
Solver for parallel reference particle propagator.

Copyright 2013, 2014, 2015, 2016 Ilja Honkonen
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

#ifndef REFERENCE_SOLVE_HPP
#define REFERENCE_SOLVE_HPP


#include "cstdlib"
#include "iostream"
#include "vector"

#include "dccrg.hpp"
#include "dccrg_cartesian_geometry.hpp"

#include "reference_cell.hpp"

namespace particle {


void reference_resize(
	dccrg::Dccrg<Reference_Cell, dccrg::Cartesian_Geometry>& grid
) {
	const auto remote_cells = grid.get_remote_cells_on_process_boundary();
	for (auto cell_id: remote_cells) {
		auto* const data = grid[cell_id];
		if (data == NULL) {
			std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
			abort();
		}

		data->external_particles.resize(data->number_of_external_particles);
	}
}


double reference_solve(
	const double dt,
	const std::vector<uint64_t>& cell_ids,
	dccrg::Dccrg<Reference_Cell, dccrg::Cartesian_Geometry>& grid
) {
	double max_time_step = std::numeric_limits<double>::max();

	// propagate particles and maybe move from internal to external list
	for (auto cell_id: cell_ids) {

		const auto
			cell_min = grid.geometry.get_min(cell_id),
			cell_max = grid.geometry.get_max(cell_id);

		auto* const cell_data = grid[cell_id];
		if (cell_data == NULL) {
			std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
			abort();
		}

		// shorthand notation
		const auto& vel = cell_data->velocity;
		auto& int_particles = cell_data->internal_particles;

		for (size_t i = 0; i < int_particles.size(); i++) {
			auto& coordinate = int_particles[i];

			coordinate[0] += vel[0] * dt;
			coordinate[1] += vel[1] * dt;

			// handle periodic grid
			coordinate = grid.geometry.get_real_coordinate(coordinate);

			// move to ext list if particle outside of current cell
			if (
				coordinate[0] < cell_min[0]
				or coordinate[0] > cell_max[0]
				or coordinate[1] < cell_min[1]
				or coordinate[1] > cell_max[1]
				or coordinate[2] < cell_min[2]
				or coordinate[2] > cell_max[2]
			) {
				const auto* const neighbors = grid.get_neighbors_of(cell_id);
				if (neighbors == NULL) {
					std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
					abort();
				}

				uint64_t destination = dccrg::error_cell;

				for (const auto neighbor_id: *neighbors) {
					if (neighbor_id == dccrg::error_cell) {
						continue;
					}

					const auto
						neighbor_min = grid.geometry.get_min(neighbor_id),
						neighbor_max = grid.geometry.get_max(neighbor_id);

					if (
						coordinate[0] >= neighbor_min[0]
						and coordinate[0] <= neighbor_max[0]
						and coordinate[1] >= neighbor_min[1]
						and coordinate[1] <= neighbor_max[1]
						and coordinate[2] >= neighbor_min[2]
						and coordinate[2] <= neighbor_max[2]
					) {
						destination = neighbor_id;
						break;
					}
				}

				if (destination != dccrg::error_cell) {
					cell_data->external_particles
						.emplace_back(coordinate, destination);

					int_particles.erase(int_particles.begin() + i);
					i--;
				}
			}
		}

		cell_data->number_of_internal_particles = int_particles.size();
		cell_data->number_of_external_particles
			= cell_data->external_particles.size();

		// check time step
		const auto length = grid.geometry.get_length(cell_id);
		max_time_step =
			std::min(max_time_step,
			std::min(
				fabs(length[0] / vel[0]),
				fabs(length[1] / vel[1])
			));
	}

	return max_time_step;
}


void reference_incorporate_external(
	const std::vector<uint64_t>& cell_ids,
	dccrg::Dccrg<Reference_Cell, dccrg::Cartesian_Geometry>& grid
) {
	for (auto cell_id: cell_ids) {

		auto* const cell_data = grid[cell_id];
		if (cell_data == NULL) {
			std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
			abort();
		}

		auto& int_particles = cell_data->internal_particles;

		// assign some particles from neighbors' external list to this cell
		const auto* const neighbors = grid.get_neighbors_of(cell_id);
		if (neighbors == NULL) {
			std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
			abort();
		}

		for (const auto neighbor_id: *neighbors) {
			if (neighbor_id == dccrg::error_cell) {
				continue;
			}

			const auto* const neighbor_data = grid[neighbor_id];
			if (neighbor_data == NULL) {
				std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
				abort();
			}

			const auto& neigh_ext_particles
				= neighbor_data->external_particles;

			for (const auto& neigh_ext_particle: neigh_ext_particles) {
				if (neigh_ext_particle.second == dccrg::error_cell) {
					std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
					abort();
				}
				if (neigh_ext_particle.second == cell_id) {
					int_particles.emplace_back(neigh_ext_particle.first);
				}
			}
		}

		cell_data->number_of_internal_particles = int_particles.size();
	}
}


void reference_remove_external(
	const std::vector<uint64_t>& cell_ids,
	dccrg::Dccrg<Reference_Cell, dccrg::Cartesian_Geometry>& grid
) {
	for (auto cell_id: cell_ids) {

		auto* const cell_data = grid[cell_id];
		if (cell_data == NULL) {
			std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
			abort();
		}

		cell_data->number_of_external_particles = 0;
		cell_data->external_particles.clear();
	}
}


} // namespace

#endif // ifndef REFERENCE_SOLVE_HPP
