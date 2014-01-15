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

#ifndef ADVECTION_SAVE_HPP
#define ADVECTION_SAVE_HPP

#include "cstdlib"
#include "iomanip"
#include "mpi.h"
#include "sstream"
#include "string"
#include "tuple"

#include "dccrg.hpp"
#include "dccrg_cartesian_geometry.hpp"

#include "gensimcell.hpp"

//! see ../serial.cpp for the basics

namespace advection {

/*!
Saves the simulation in given grid into a file with name derived from given time. 

The variables Density_T and Velocity_T are used to refer
to the particular data to be saved.
Assumes that the transfer of all variables
had been disabled before this function was called. 
*/
template<
	class Cell_T,
	class Density_T,
	class Velocity_T
> void save(
	dccrg::Dccrg<Cell_T, dccrg::Cartesian_Geometry>& grid,
	const double simulation_time
) {
	// save density and velocity
	Cell_T::set_transfer_all(Density_T(), true);
	Cell_T::set_transfer_all(Velocity_T(), true);

	// get the file name
	std::ostringstream time_string;
	time_string
		<< std::setw(4)
		<< std::setfill('0')
		<< size_t(simulation_time * 1000);

	// use an empty header with a sane address
	char dummy;
	std::tuple<void*, int, MPI_Datatype> header{(void*) &dummy, 0, MPI_BYTE};

	grid.save_grid_data(
		"advection_" + time_string.str() + ".dc",
		0,
		header
	);
}

} // namespace

#endif // ifndef ADVECTION_SAVE_HPP
