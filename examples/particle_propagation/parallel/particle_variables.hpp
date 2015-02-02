/*
Variables for parallel particle propagator.

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

#ifndef PARTICLE_VARIABLES_HPP
#define PARTICLE_VARIABLES_HPP


#include "array"
#include "mpi.h"
#include "tuple"
#include "vector"

#include "gensimcell.hpp"


//! see ../serial.cpp for the basics

/*!
Avoid collisions between the names of
variables of different simulations by
using a unique namespace for each one.
*/
namespace particle {


/*!
Particles whose coordinates are inside of
the cell in which they are stored.
*/
struct Internal_Particles
{
	using data_type = std::vector<std::array<double, 3>>;
};



/*!
Particles that have propagated out of the
cell in which they are stored and should be
assigned to the storage of a different cell.
*/
struct External_Particles
{
	using data_type
		= std::vector<
			std::pair<
				std::array<double, 3>, // coordinate
				unsigned long long int // destination cell
			>
		>;
};


/*!
The number of internal particles in a cell.

MPI requires that the max size of transferred
data be known prior to the transfer.
*/
struct Number_Of_Internal_Particles
{
	using data_type = unsigned long long int;
};


/*!
The number of external particles in a cell.

MPI requires that the max size of transferred
data be known prior to the transfer.
*/
struct Number_Of_External_Particles
{
	using data_type = unsigned long long int;
};


struct Velocity
{
	using data_type = std::array<double, 2>;
};


/*!
Cell definition for the particle propagation example.

Fixed size variables are ordered first so that they
are saved at a known position in the file by dccrg.
The cell class puts the variables in an MPI datatype
in the same order as they are given here, which
dccrg will use to save the file.
*/
using Cell = gensimcell::Cell<
	gensimcell::Optional_Transfer,
	Number_Of_Internal_Particles,
	Number_Of_External_Particles,
	Velocity,
	Internal_Particles,
	External_Particles
>;


} // namespace

#endif // ifndef PARTICLE_VARIABLES_HPP
