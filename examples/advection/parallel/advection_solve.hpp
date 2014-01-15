/*
Copyright (c) 2013, 2014, Ilja Honkonen
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

#ifndef ADVECTION_SOLVE_HPP
#define ADVECTION_SOLVE_HPP

#include "cstdlib"
#include "iostream"
#include "vector"

#include "dccrg.hpp"
#include "dccrg_cartesian_geometry.hpp"

#include "gensimcell.hpp"

//! see ../serial.cpp for the basics

namespace advection {

/*!
Calculates the flux of advected density into (+) and
out of (-) each given cell in given grid over given time.

Returns the longest allowed time step for given cells
and their neighbors.
*/
template<
	class Cell_T,
	class Density_T,
	class Density_Flux_T,
	class Velocity_T
> double solve(
	const double dt,
	const std::vector<uint64_t>& cell_ids,
	dccrg::Dccrg<Cell_T, dccrg::Cartesian_Geometry>& grid
) {
	double max_time_step = std::numeric_limits<double>::max();

	for (auto cell_id: cell_ids) {

		/*
		Unoptimized version that only changes the data of the current
		cell, i.e. the flux through each face is always solved twice
		even if the neighboring cell is on the same process
		*/

		Cell_T* data = grid[cell_id];
		if (data == NULL) {
			std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
			abort();
		}

		// shorthand notation
		const auto& n = (*data)(Density_T());
		const auto& v = (*data)(Velocity_T());
		auto& flux = (*data)(Density_Flux_T());

		// substract density flowing out of this cell
		const auto length = grid.geometry.get_length(cell_id);

		// advection out of current cell in x and y directions
		flux -= fabs(n * v[0] * dt / length[0]);
		flux -= fabs(n * v[1] * dt / length[1]);

		// check time step
		max_time_step =
			std::min(max_time_step,
			std::min(
				fabs(length[0] / v[0]),
				fabs(length[1] / v[1])
			));

		// advection into current cell from neighbors
		const auto face_neighbors = grid.get_face_neighbors_of(cell_id);
		for (const auto& item: face_neighbors) {

			const uint64_t neighbor_id = item.first;
			// direction of this neighbor from the cell
			const int dir = item.second;
			// index into multidimensional arrays
			const size_t dim = size_t(abs(dir) - 1);

			Cell_T* neighbor_data = grid[neighbor_id];
			if (neighbor_data == NULL) {
				std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
				abort();
			}

			const auto& neigh_n = (*neighbor_data)(Density_T());
			const auto& neigh_v = (*neighbor_data)(Velocity_T());

			if (
				(dir < 0 and neigh_v[dim] < 0)
				or (dir > 0 and neigh_v[dim] > 0)
			) {
				// nothing flows into the cell from this neighbor
				continue;
			}

			const auto neigh_length = grid.geometry.get_length(neighbor_id);

			flux += fabs(neigh_n * neigh_v[dim] * dt / neigh_length[dim]);

			max_time_step
				= std::min(
					max_time_step,
					fabs(neigh_length[dim] / neigh_v[dim])
				);
		}
	}

	return max_time_step;
}


/*!
Applies the density fluxes in given cells.
*/
template<
	class Cell_T,
	class Density_T,
	class Density_Flux_T
> void apply_solution(
	const std::vector<uint64_t>& cell_ids,
	dccrg::Dccrg<Cell_T, dccrg::Cartesian_Geometry>& grid
) {
	for (auto cell_id: cell_ids) {
		Cell_T* data = grid[cell_id];
		if (data == NULL) {
			std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
			abort();
		}
		(*data)(Density_T()) += (*data)(Density_Flux_T());
		(*data)(Density_Flux_T()) = 0;
	}
}


} // namespace

#endif // ifndef ADVECTION_SOLVE_HPP
