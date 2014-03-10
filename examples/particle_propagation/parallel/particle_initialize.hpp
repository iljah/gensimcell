/*
Sets the initial condition of parallel particle propagator program.

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

#ifndef PARTICLE_INITIALIZE_HPP
#define PARTICLE_INITIALIZE_HPP

#include "cstdlib"
#include "iostream"
#include "vector"

#include "dccrg.hpp"
#include "dccrg_cartesian_geometry.hpp"

#include "gensimcell.hpp"

//! see ../serial.cpp for the basics

namespace particle {

/*!
Inserts 3 particles / cell in the center of the given grid.
*/
template<
	class Cell_T,
	class Number_Of_Internal_Particles_T,
	class Number_Of_External_Particles_T,
	class Velocity_T,
	class Internal_Particles_T,
	class External_Particles_T
> void initialize(
	dccrg::Dccrg<Cell_T, dccrg::Cartesian_Geometry>& grid
) {
	const auto
		grid_start = grid.geometry.get_start(),
		grid_end = grid.geometry.get_end();

	auto
		particles_start = grid_start,
		particles_end = grid_end;

	// don't create particles too close to the edges
	for (size_t i = 0; i < grid_start.size(); i++) {
		particles_start[i] += (grid_end[i] - grid_start[i]) / 4;
		particles_end[i] -= (grid_end[i] - grid_start[i]) / 4;
	}


	const std::vector<uint64_t> cell_ids = grid.get_cells();
	for (auto cell_id: cell_ids) {

		const auto
			center = grid.geometry.get_center(cell_id),
			length = grid.geometry.get_length(cell_id);

		Cell_T* cell_data = grid[cell_id];
		if (cell_data == NULL) {
			std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
			abort();
		}

		(*cell_data)[Number_Of_Internal_Particles_T()] =
		(*cell_data)[Number_Of_External_Particles_T()] = 0;
		(*cell_data)[Velocity_T()] = {
			-2 * center[1],
			+2 * center[0]
		};

		if (
			center[0] < particles_start[0]
			or center[0] > particles_end[0]
			or center[1] < particles_start[1]
			or center[1] > particles_end[1]
		) {
			continue;
		}

		(*cell_data)[Number_Of_Internal_Particles_T()] = 3;
		(*cell_data)[Internal_Particles_T()].coordinates.push_back({
				center[0] - length[0] / 4,
				center[1] - length[1] / 4,
				center[2]
		});
		(*cell_data)[Internal_Particles_T()].coordinates.push_back({
				center[0],
				center[1] + length[1] / 4,
				center[2]
		});
		(*cell_data)[Internal_Particles_T()].coordinates.push_back({
				center[0] + length[0] / 4,
				center[1] - length[1] / 4,
				center[2]
		});
	}
}

} // namespace

#endif // ifndef PARTICLE_INITIALIZE_HPP
