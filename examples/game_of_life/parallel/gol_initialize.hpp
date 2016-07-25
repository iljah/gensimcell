/*
Sets the initial condition of parallel game of life program.

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

#ifndef GOL_INITIALIZE_HPP
#define GOL_INITIALIZE_HPP

#include "cstdlib"
#include "iostream"
#include "vector"

#include "dccrg.hpp"
#include "dccrg_cartesian_geometry.hpp"

#include "gensimcell.hpp"

//! see ../serial.cpp for the basics

namespace gol {

/*!
Initializes given grid with a row of live cells at the center.

The row is parallel to the first dimension.
Assumes maximum refinement level == 0.
*/
template<
	class Cell_T,
	class Is_Alive_T,
	class Live_Neighbors_T
> void initialize(
	dccrg::Dccrg<Cell_T, dccrg::Cartesian_Geometry>& grid
) {
	const auto
		grid_start = grid.geometry.get_start(),
		grid_end = grid.geometry.get_end();

	const double middle = (grid_end[1] + grid_start[1]) / 2;

	const std::vector<uint64_t> cell_ids = grid.get_cells();
	for (auto cell_id: cell_ids) {

		Cell_T* cell_data = grid[cell_id];
		if (cell_data == NULL) {
			std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
			abort();
		}

		const auto
			cell_min = grid.geometry.get_min(cell_id),
			cell_max = grid.geometry.get_max(cell_id);

		if (cell_max[1] > middle and cell_min[1] <= middle) {
			(*cell_data)[Is_Alive_T()] = true;
		} else {
			(*cell_data)[Is_Alive_T()] = false;
		}
		(*cell_data)[Live_Neighbors_T()] = 0;
	}
}

} // namespace

#endif // ifndef GOL_INITIALIZE_HPP
