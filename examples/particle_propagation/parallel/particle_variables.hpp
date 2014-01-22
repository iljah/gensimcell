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
Stores an arbitrary number of 2d particle coordinates and
provides the corresponding MPI transfer information.
*/
struct Particle_Storage
{
	typedef std::vector<std::array<double, 3>> storage_type;
	storage_type coordinates;

	std::tuple<
		void*,
		int,
		MPI_Datatype
	> get_mpi_datatype() const
	{
		return std::make_tuple(
			(void*) this->coordinates.data(),
			3 * this->coordinates.size(),
			MPI_DOUBLE
		);
	}

	// resizes coordinates to fit new_size particles
	void resize(const size_t new_size)
	{
		this->coordinates.resize(new_size);
	}
};


/*!
Particles whose coordinates are inside of
the cell in which they are stored.
*/
struct Internal_Particles
{
	typedef Particle_Storage data_type;
};


/*!
Particles that have propagated out of the
cell in which they are stored and should be
assigned to the storage of a different cell.
*/
struct External_Particles
{
	typedef Particle_Storage data_type;
};


/*!
Used for MPI which requires known max size of transferred data.

When updating copies of remote neighbor cells represents
the number of external particles, when saving the simulation
to a file represents the number of particles in a cell in
general assuming all particles are located in the internal
particle list at that time.
*/
struct Number_Of_Particles
{
	typedef unsigned long int data_type;
};


/*!
Records the destination cell of particles not
located in the cell in which they are stored.

Assumes a 2d system with a maximum of 8 destination cells.
*/
struct Particle_Destinations
{
	/*
	Format: 1st destination, number of particles to
	assign to 1st destination from the external particle
	list starting from the first particle, 2nd destination,
	number of particles to assign to 2nd destination
	starting from the next unassigned particle, etc.
	*/
	typedef std::array<unsigned long long int, 16> data_type;
};


struct Velocity
{
	typedef std::array<double, 2> data_type;
};


/*!
Cell definition for the particle propagation example.

Fixed size variables are ordered first in the
transfer information for MPI to allow reading
files saved by dccrg more easily.
*/
typedef gensimcell::Cell<
	Number_Of_Particles,
	Particle_Destinations,
	Velocity,
	Internal_Particles,
	External_Particles
> Cell;


} // namespace

#endif // ifndef PARTICLE_VARIABLES_HPP
