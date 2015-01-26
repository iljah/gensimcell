/*
Sets the initial condition of parallel advection program.

Copyright 2014, 2015 Ilja Honkonen
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

#ifndef ADVECTION_INITIALIZE_HPP
#define ADVECTION_INITIALIZE_HPP

#include "cstdlib"
#include "iostream"
#include "vector"

#include "dccrg.hpp"
#include "dccrg_cartesian_geometry.hpp"

#include "gensimcell.hpp"

//! see ../serial.cpp for the basics

namespace advection {

/*!
The initial condition is from chapter 20.8.2 in
R.J. LeVeque,
Finite Volume Methods for Hyperbolic Problems,
ISBN 978-0-521-00924-9.
*/
template<
	class Cell_T,
	class Density_T,
	class Density_Flux_T,
	class Velocity_T
> void initialize(
	dccrg::Dccrg<Cell_T, dccrg::Cartesian_Geometry>& grid
) {
	const std::vector<uint64_t> cell_ids = grid.get_cells();
	for (auto cell_id: cell_ids) {

		Cell_T* cell_data_temp = grid[cell_id];
		if (cell_data_temp == NULL) {
			std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
			abort();
		}
		// make the notation a bit easier on the eyes
		Cell_T& cell_data = *cell_data_temp;

		const auto cell_center = grid.geometry.get_center(cell_id);

		const double
			r = std::sqrt(
				std::pow(cell_center[0] + 0.45, 2)
				+ std::pow(cell_center[1], 2)
			);

		/*
		Initialize density
		*/
		cell_data[Density_T()]      =
		cell_data[Density_Flux_T()] = 0;

		// square
		if (
			cell_center[0] > 0.1
			and cell_center[0] < 0.6
			and cell_center[1] > -0.25
			and cell_center[1] < 0.25
		) {
			cell_data[Density_T()] = 1;

		// cone
		} else if (r < 0.35) {
			cell_data[Density_T()] = 1 - r / 0.35;
		}

		/*
		Initialize velocity
		*/
		cell_data[Velocity_T()] = {{
			+2 * cell_center[1],
			-2 * cell_center[0]
		}};
	}
}

} // namespace

#endif // ifndef ADVECTION_INITIALIZE_HPP
