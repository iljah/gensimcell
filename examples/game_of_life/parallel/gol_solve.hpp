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

#ifndef GOL_SOLVE_HPP
#define GOL_SOLVE_HPP

#include "cstdlib"
#include "iostream"
#include "vector"

#include "dccrg.hpp"
#include "dccrg_cartesian_geometry.hpp"

#include "gensimcell.hpp"

//! see ../serial.cpp for the basics

namespace gol {

/*!
Calculates the number of live neighbors for given cells.

Uses Is_Alive to access the data corresponding to
the life state of a cell and Live_Neighbors to access
the data corresponding to the number of live neighbors.
*/
template<
	class Cell_T,
	class Is_Alive,
	class Live_Neighbors
> void solve(
	const std::vector<uint64_t>& cell_ids,
	dccrg::Dccrg<Cell_T, dccrg::Cartesian_Geometry>& game_grid
) {
	for (auto cell_id: cell_ids) {

		Cell_T* current_data = game_grid[cell_id];
		if (current_data == NULL) {
			std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
			abort();
		}

		const std::vector<uint64_t>* const neighbors
			= game_grid.get_neighbors_of(cell_id);

		for (auto neighbor_id: *neighbors) {

			if (neighbor_id == dccrg::error_cell) {
				continue;
			}

			Cell_T* neighbor_data = game_grid[neighbor_id];
			if (neighbor_data == NULL) {
				std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
				abort();
			}

			if ((*neighbor_data)(Is_Alive())) {
				(*current_data)(Live_Neighbors())++;
			}
		}
	}
}


/*!
Applies the rules of Conway's Game of Life to given cells.
*/
template<
	class Cell_T,
	class Is_Alive,
	class Live_Neighbors
> void apply_solution(
	const std::vector<uint64_t>& cell_ids,
	dccrg::Dccrg<Cell_T, dccrg::Cartesian_Geometry>& game_grid
) {
	for (auto cell_id: cell_ids) {

		Cell_T* data = game_grid[cell_id];
		if (data == NULL) {
			std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
			abort();
		}

		if ((*data)(Live_Neighbors()) == 3) {
			(*data)(Is_Alive()) = true;
		} else if ((*data)(Live_Neighbors()) != 2) {
			(*data)(Is_Alive()) = false;
		}
		(*data)(Live_Neighbors()) = 0;
	}
}


} // namespace

#endif // ifndef GOL_VARIABLES_HPP
