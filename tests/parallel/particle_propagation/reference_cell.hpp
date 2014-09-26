/*
Variables for reference parallel particle propagator.

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

#ifndef REFERENCE_CELL_HPP
#define REFERENCE_CELL_HPP


#include "array"
#include "mpi.h"
#include "tuple"
#include "vector"

namespace particle {

class Reference_Cell
{
public:
	enum transfer_types {
		nr_of_int_particles = (1 << 0),
		nr_of_ext_particles = (1 << 1),
		vel                 = (1 << 2),
		int_particles       = (1 << 3),
		ext_particles       = (1 << 4)
	};

	static int transfers;

	std::vector<uint64_t> destinations;
	std::vector<std::array<double, 3>> external_coordinates, internal_coordinates;
	std::array<double, 2> velocity;
	unsigned long int number_of_external_particles, number_of_internal_particles;

	// returns transfer info for each variable with bit set in transfers
	std::tuple<
		void*,
		int,
		MPI_Datatype
	> get_mpi_datatype() const
	{
		MPI_Datatype final_datatype = MPI_DATATYPE_NULL;

		std::array<int, 6> counts{{0, 0, 0, 0, 0, 0}};
		std::array<MPI_Aint, 6> displacements{{0, 0, 0, 0, 0, 0}};
		std::array<MPI_Datatype, 6> datatypes{{
			MPI_DATATYPE_NULL,
			MPI_DATATYPE_NULL,
			MPI_DATATYPE_NULL,
			MPI_DATATYPE_NULL,
			MPI_DATATYPE_NULL,
			MPI_DATATYPE_NULL
		}};

		void* starting_address = 0;
		size_t nr_vars_to_transfer = 0;

		if ((transfers & transfer_types::nr_of_int_particles) > 0) {
			if (nr_vars_to_transfer == 0) {
				starting_address = (void*) &(this->number_of_internal_particles);
			}
			counts[nr_vars_to_transfer] = 1;
			displacements[nr_vars_to_transfer]
				= reinterpret_cast<const char* const>(&(this->number_of_internal_particles))
				- reinterpret_cast<const char* const>(starting_address);
			datatypes[nr_vars_to_transfer] = MPI_UNSIGNED_LONG;
			nr_vars_to_transfer++;
		}

		if ((transfers & transfer_types::nr_of_ext_particles) > 0) {
			if (nr_vars_to_transfer == 0) {
				starting_address = (void*) &(this->number_of_external_particles);
			}
			counts[nr_vars_to_transfer] = 1;
			displacements[nr_vars_to_transfer]
				= reinterpret_cast<const char* const>(&(this->number_of_external_particles))
				- reinterpret_cast<const char* const>(starting_address);
			datatypes[nr_vars_to_transfer] = MPI_UNSIGNED_LONG;
			nr_vars_to_transfer++;
		}

		if ((transfers & transfer_types::vel) > 0) {
			if (nr_vars_to_transfer == 0) {
				starting_address = (void*) this->velocity.data();
			}
			counts[nr_vars_to_transfer] = 2;
			displacements[nr_vars_to_transfer]
				= reinterpret_cast<const char* const>(this->velocity.data())
				- reinterpret_cast<const char* const>(starting_address);
			datatypes[nr_vars_to_transfer] = MPI_DOUBLE;
			nr_vars_to_transfer++;
		}

		if ((transfers & transfer_types::int_particles) > 0) {
			if (nr_vars_to_transfer == 0) {
				starting_address = (void*) this->internal_coordinates.data();
			}
			counts[nr_vars_to_transfer] = 3 * this->internal_coordinates.size();
			displacements[nr_vars_to_transfer]
				= reinterpret_cast<const char* const>(this->internal_coordinates.data())
				- reinterpret_cast<const char* const>(starting_address);
			datatypes[nr_vars_to_transfer] = MPI_DOUBLE;
			nr_vars_to_transfer++;
		}

		if ((transfers & transfer_types::ext_particles) > 0) {
			if (nr_vars_to_transfer == 0) {
				starting_address = (void*) this->external_coordinates.data();
			}
			counts[nr_vars_to_transfer] = 3 * this->external_coordinates.size();
			displacements[nr_vars_to_transfer]
				= reinterpret_cast<const char* const>(this->external_coordinates.data())
				- reinterpret_cast<const char* const>(starting_address);
			datatypes[nr_vars_to_transfer] = MPI_DOUBLE;
			nr_vars_to_transfer++;

			counts[nr_vars_to_transfer] = this->destinations.size();
			displacements[nr_vars_to_transfer]
				= reinterpret_cast<const char* const>(this->destinations.data())
				- reinterpret_cast<const char* const>(starting_address);
			datatypes[nr_vars_to_transfer] = MPI_UINT64_T;
			nr_vars_to_transfer++;
		}

		if (
			MPI_Type_create_struct(
				nr_vars_to_transfer,
				counts.data(),
				displacements.data(),
				datatypes.data(),
				&final_datatype
			) != MPI_SUCCESS
		) {
			return std::make_tuple((void*) NULL, -1, MPI_DATATYPE_NULL);
		}

		return std::make_tuple(
			(void*) starting_address,
			1,
			final_datatype
		);
	}

	// resizes internal storage to fit new_size particles
	void resize(const size_t new_size)
	{
		this->external_coordinates.resize(new_size);
		this->destinations.resize(new_size);
	}
};

int Reference_Cell::transfers = 0;

} // namespace

#endif // ifndef REFERENCE_CELL_HPP
