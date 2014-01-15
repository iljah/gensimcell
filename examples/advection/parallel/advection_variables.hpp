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


#ifndef ADVECTION_VARIABLES_HPP
#define ADVECTION_VARIABLES_HPP

#include "gensimcell.hpp"

//! see ../serial.cpp for the basics

/*!
Avoid collisions between the names of
variables of different simulations by
using a unique namespace for each one.
*/
namespace advection {

// see ../serial.cpp for basics

struct Density
{
	typedef double data_type;
};

struct Density_Flux
{
	typedef double data_type;
};

struct Velocity
{
	typedef std::array<double, 2> data_type;
};

typedef gensimcell::Cell<
	Density,
	Density_Flux,
	Velocity
> Cell;


/*!
Stops the MPI transfer of all variables
used in a parallel advection simulation.

The actual variables used by a particular advection
simulation with the given cell type are given as
template parameters.
*/
template<
	class Cell_T,
	class Density_T,
	class Density_Flux_T,
	class Velocity_T
> void transfer_none()
{
	Cell_T::set_transfer_all(Density_T(), false);
	Cell_T::set_transfer_all(Density_Flux_T(), false);
	Cell_T::set_transfer_all(Velocity_T(), false);
}


/*!
Starts the MPI transfer of all variables
required in a parallel advection simulation.

The actual variables used by a particular advection
simulation with the given cell type are given as
template parameters.
*/
template<
	class Cell_T,
	class Density_T,
	class Velocity_T
> void transfer_all()
{
	Cell_T::set_transfer_all(Density_T(), true);
	Cell_T::set_transfer_all(Velocity_T(), true);
}


} // namespace

#endif // ifndef ADVECTION_VARIABLES_HPP
