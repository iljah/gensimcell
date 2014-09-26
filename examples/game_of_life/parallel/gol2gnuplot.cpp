/*
Program for converting output of game of life program with gnuplot.

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

#include "cstdint"
#include "cstdlib"
#include "fstream"
#include "mpi.h"
#include "string"
#include "unordered_map"
#include "vector"

#include "dccrg_cartesian_geometry.hpp"
#include "dccrg_mapping.hpp"
#include "dccrg_topology.hpp"

#include "gol_variables.hpp"

using namespace std;
using namespace gol;

int main(int argc, char* argv[])
{
	if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
		cerr << "Coudln't initialize MPI." << endl;
		abort();
	}

	MPI_Comm comm = MPI_COMM_WORLD;

	int rank = 0, comm_size = 0;
	MPI_Comm_rank(comm, &rank);
	MPI_Comm_size(comm, &comm_size);


	dccrg::Mapping mapping;
	dccrg::Grid_Topology topology;
	dccrg::Cartesian_Geometry geometry(mapping.length, mapping, topology);

	for (int i = 1; i < argc; i++) {

		const string argv_string(argv[i]);

		if ((i - 1) % comm_size != rank) {
			continue;
		}

		MPI_File file;
		if (
			MPI_File_open(
				MPI_COMM_SELF,
				const_cast<char*>(argv_string.c_str()),
				MPI_MODE_RDONLY,
				MPI_INFO_NULL,
				&file
			) != MPI_SUCCESS
		) {
			cerr << "Process " << rank
				<< " couldn't open file " << argv_string
				<< endl;
			continue;
		}

		// skip unncessary data
		MPI_Offset offset = sizeof(uint64_t);

		mapping.read(file, offset);

		offset
			+= mapping.data_size()
			+ sizeof(unsigned int)
			+ topology.data_size()
			+ geometry.data_size();

		// read number of cells
		uint64_t total_cells = 0;
		MPI_File_read_at(
			file,
			offset,
			&total_cells,
			1,
			MPI_UINT64_T,
			MPI_STATUS_IGNORE
		);
		offset += sizeof(uint64_t);

		if (total_cells == 0) {
			MPI_File_close(&file);
			continue;
		}

		// read cell ids and file offsets of cell data
		vector<pair<uint64_t, uint64_t>> cells_offsets(total_cells);
		MPI_File_read_at(
			file,
			offset,
			cells_offsets.data(),
			2 * total_cells,
			MPI_UINT64_T,
			MPI_STATUS_IGNORE
		);

		/*
		Read cell data

		Assumes that only one variable of type MPI_CXX_BOOL was
		saved for each cell. For a more generic version which
		only assumes that the size of each cells' data is known
		before reading anything from the saved file see
		../../advection/parallel/advection2gnuplot.cpp
		*/
		unordered_map<
			uint64_t,
			Is_Alive::data_type
		> simulation_data;

		for (const auto& item: cells_offsets) {
			const uint64_t
				cell_id = item.first,
				offset = item.second;

			simulation_data[cell_id];

			MPI_File_read_at(
				file,
				(MPI_Offset) offset,
				&(simulation_data.at(cell_id)),
				1,
				MPI_CXX_BOOL,
				MPI_STATUS_IGNORE
			);
		}

		cells_offsets.clear();

		const string
			gnuplot_file_name(argv_string + ".dat"),
			plot_file_name(argv_string + ".png");

		ofstream gnuplot_file(gnuplot_file_name);

		gnuplot_file
			<< "set term png enhanced\nset output '"
			<< plot_file_name
			<< "'\nset size square\n"
			   "plot '-' matrix with image title ''\n";

		// plot assumes data is ordered from top to bottom left to right
		const uint64_t grid_width = mapping.length.get()[1]; 
		for (size_t y_i = grid_width - 1; /* handle this below */ true; y_i--) {
			for (size_t x_i = 0; x_i < grid_width; x_i++) {

				const auto cell_id
					= mapping.get_cell_from_indices({{x_i, y_i, 0}}, 0);

				if (simulation_data.at(cell_id)) {
					gnuplot_file << "1 ";
				} else {
					gnuplot_file << "0 ";
				}
			}

			gnuplot_file << "\n";

			if (y_i == 0) {
				break;
			}
		}
		gnuplot_file << "end\n";
		gnuplot_file.close();

		system(("gnuplot " + gnuplot_file_name).c_str());
	}

	MPI_Finalize();

	return EXIT_SUCCESS;
}
