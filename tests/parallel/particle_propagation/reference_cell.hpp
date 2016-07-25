/*
Variables for reference parallel particle propagator.

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

#ifndef REFERENCE_CELL_HPP
#define REFERENCE_CELL_HPP


#include "array"
#include "tuple"
#include "vector"

#include "mpi.h"

namespace particle {

struct Reference_Cell
{
	enum transfer_types {
		nr_of_int_particles = (1 << 0),
		nr_of_ext_particles = (1 << 1),
		vel                 = (1 << 2),
		int_particles       = (1 << 3),
		ext_particles       = (1 << 4)
	};

	static int transfers;

	unsigned long long int
		number_of_internal_particles,
		number_of_external_particles;

	std::array<double, 2> velocity;
	std::vector<std::array<double, 3>> internal_particles;
	std::vector<
		std::pair<
			std::array<double, 3>,
			unsigned long long int
		>
	> external_particles;

	// returns transfer info for each variable with bit set in transfers
	std::tuple<
		void*,
		int,
		MPI_Datatype
	> get_mpi_datatype() const
	{
		MPI_Datatype final_datatype = MPI_DATATYPE_NULL;

		std::array<int, 5> counts{{0, 0, 0, 0, 0}};
		std::array<MPI_Aint, 5> displacements{{0, 0, 0, 0, 0}};
		std::array<MPI_Datatype, 5> datatypes{{
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
			datatypes[nr_vars_to_transfer] = MPI_UNSIGNED_LONG_LONG;
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
			datatypes[nr_vars_to_transfer] = MPI_UNSIGNED_LONG_LONG;
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

		if (
			(transfers & transfer_types::int_particles) > 0
			and this->internal_particles.size() > 0
		) {
			if (nr_vars_to_transfer == 0) {
				starting_address = (void*) this->internal_particles.data();
			}

			counts[nr_vars_to_transfer] = 3 * this->internal_particles.size();
			datatypes[nr_vars_to_transfer] = MPI_DOUBLE;
			displacements[nr_vars_to_transfer]
				= reinterpret_cast<const char* const>(this->internal_particles.data())
				- reinterpret_cast<const char* const>(starting_address);
			nr_vars_to_transfer++;
		}

		if (
			(transfers & transfer_types::ext_particles) > 0
			and this->external_particles.size() > 0
		) {
			if (nr_vars_to_transfer == 0) {
				starting_address = (void*) this->external_particles.data();
			}

			// datatype for one particle
			std::array<int, 2> temp_counts{{3, 1}};
			std::array<MPI_Aint, 2> temp_displacements{{
				0,
				reinterpret_cast<const char* const>(
					&(this->external_particles[0].second)
				)
				- reinterpret_cast<const char* const>(
					&(this->external_particles[0].first[0])
				)
			}};
			std::array<MPI_Datatype, 2> temp_datatypes{{
				MPI_DOUBLE,
				MPI_UNSIGNED_LONG_LONG
			}};
			MPI_Datatype temp_datatype;
			if (
				MPI_Type_create_struct(
					int(temp_counts.size()),
					temp_counts.data(),
					temp_displacements.data(),
					temp_datatypes.data(),
					&temp_datatype
				) != MPI_SUCCESS
			) {
				return std::make_tuple((void*) NULL, -1, MPI_DATATYPE_NULL);
			}

			// datatype for all particles
			const auto size = this->external_particles.size();
			std::vector<int> temp2_counts(size, 1);
			std::vector<MPI_Datatype> temp2_datatypes(size, temp_datatype);

			std::vector<MPI_Aint> temp2_displacements;
			temp2_displacements.reserve(size);
			for (const auto& ext_particle: this->external_particles) {
				temp2_displacements.emplace_back(
					reinterpret_cast<const char* const>(&(ext_particle.first[0]))
					- reinterpret_cast<const char* const>(this->external_particles.data())
				);
			}

			MPI_Datatype ext_particles_type;
			if (
				MPI_Type_create_struct(
					int(temp2_counts.size()),
					temp2_counts.data(),
					temp2_displacements.data(),
					temp2_datatypes.data(),
					&ext_particles_type
				) != MPI_SUCCESS
			) {
				return std::make_tuple((void*) NULL, -2, MPI_DATATYPE_NULL);
			}

			datatypes[nr_vars_to_transfer] = ext_particles_type;
			displacements[nr_vars_to_transfer]
				= reinterpret_cast<const char* const>(this->external_particles.data())
				- reinterpret_cast<const char* const>(starting_address);
			MPI_Type_free(&temp_datatype);

			counts[nr_vars_to_transfer] = 1;
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
			return std::make_tuple((void*) NULL, -3, MPI_DATATYPE_NULL);
		}

		// maybe free component types
		for (auto& datatype: datatypes) {
			if (datatype == MPI_DATATYPE_NULL) {
				continue;
			}
			int combiner = -1, tmp1 = -1, tmp2 = -1, tmp3 = -1;
			MPI_Type_get_envelope(datatype, &tmp1, &tmp2, &tmp3, &combiner);
			if (combiner != MPI_COMBINER_NAMED) {
				MPI_Type_free(&datatype);
			}
		}

		return std::make_tuple(
			(void*) starting_address,
			1,
			final_datatype
		);
	}
};

int Reference_Cell::transfers = 0;

} // namespace

#endif // ifndef REFERENCE_CELL_HPP
