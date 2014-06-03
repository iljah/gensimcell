/*
Variables for parallel particle propagator.

Copyright 2014 Ilja Honkonen
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
Stores an arbitrary number of 3d particle coordinates and
provides the corresponding MPI transfer information.
*/
struct Internal_Particle_Storage
{
	std::vector<std::array<double, 3>> coordinates;

	std::tuple<
		void*,
		int,
		MPI_Datatype
	> get_mpi_datatype() const
	{
		if (3 * this->coordinates.size() > std::numeric_limits<int>::max()) {
			return std::make_tuple((void*) NULL, -1, MPI_DATATYPE_NULL);
		}

		return std::make_tuple(
			(void*) this->coordinates.data(),
			3 * this->coordinates.size(),
			MPI_DOUBLE
		);
	}

	// resizes internal storage to fit new_size particles
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
	using data_type = Internal_Particle_Storage;
};



/*!
Stores an arbitrary number of 3d particle coordinates,
the particles' destination cells and provides the
corresponding MPI transfer information.
*/
struct External_Particle_Storage
{
	std::vector<std::array<double, 3>> coordinates;
	std::vector<uint64_t> destinations;

	/*!
	Returns the MPI transfer information corresponding to
	particle coordinates and their destination cells.

	1st value is starting address, 2nd number of items, 3rd the datatype.
	In case of error returns NULL, N and MPI_DATATYPE_NULL, where N is:
		- -1 if the number of doubles in coordinates > maximum int
		- -2 if MPI_Type_create_struct failed to create the final type
	*/
	std::tuple<
		void*,
		int,
		MPI_Datatype
	> get_mpi_datatype() const
	{
		if (3 * this->coordinates.size() > std::numeric_limits<int>::max()) {
			return std::make_tuple((void*) NULL, -1, MPI_DATATYPE_NULL);
		}

		std::array<int, 2> counts{
			int(3 * this->coordinates.size()),
			int(1 * this->destinations.size())
		};

		std::array<MPI_Aint, 2> displacements{
			0,
			  reinterpret_cast<const char* const>(this->destinations.data())
			- reinterpret_cast<const char* const>(this->coordinates.data())
		};

		std::array<MPI_Datatype, 2> datatypes{
			MPI_DOUBLE,
			MPI_UINT64_T
		};


		MPI_Datatype final_datatype = MPI_DATATYPE_NULL;
		if (
			MPI_Type_create_struct(
				2,
				counts.data(),
				displacements.data(),
				datatypes.data(),
				&final_datatype
			) != MPI_SUCCESS
		) {
			return std::make_tuple((void*) NULL, -2, MPI_DATATYPE_NULL);
		}


		return std::make_tuple(
			(void*) this->coordinates.data(),
			1,
			final_datatype
		);
	}

	// resizes internal storage to fit new_size particles
	void resize(const size_t new_size)
	{
		this->coordinates.resize(new_size);
		this->destinations.resize(new_size);
	}
};


/*!
Particles that have propagated out of the
cell in which they are stored and should be
assigned to the storage of a different cell.
*/
struct External_Particles
{
	using data_type = External_Particle_Storage;
};


/*!
The number of internal particles in a cell.

MPI requires that the max size of transferred
data be known prior to the transfer.
*/
struct Number_Of_Internal_Particles
{
	using data_type = unsigned long int;
};


/*!
The number of external particles in a cell.

MPI requires that the max size of transferred
data be known prior to the transfer.
*/
struct Number_Of_External_Particles
{
	using data_type = unsigned long int;
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
	Number_Of_Internal_Particles,
	Number_Of_External_Particles,
	Velocity,
	Internal_Particles,
	External_Particles
>;


} // namespace

#endif // ifndef PARTICLE_VARIABLES_HPP
