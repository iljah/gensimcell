/*
Serial example program propagating particles in a const in time velocity field.

Copyright 2013, 2014 Ilja Honkonen
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

#include "algorithm"
#include "array"
#include "boost/lexical_cast.hpp"
#include "cmath"
#include "cstdlib"
#include "fstream"
#include "iomanip"
#include "iostream"
#include "limits"
#include "sstream"
#include "string"
#include "tuple"
#include "vector"

#include "gensimcell.hpp"


using namespace std;


//! see ../advection/serial.cpp for basics

struct Velocity
{
	using data_type = array<double, 2>;
};

/*!
Class storing an arbitrary number of 2-dimensional particle coordinates.
*/
struct Particles
{
	using data_type = vector<array<double, 2>>;
};


using cell_t = gensimcell::Cell<
	Velocity,
	Particles
>;


constexpr size_t
	width = 20,
	height = 20;

using grid_t = array<array<cell_t, width>, height>;
grid_t grid;


/*!
Writes given particle propagation simulation to a gnuplot file.

Returns the name of the file written which
is derived from given simulation time.
*/
string save(
	const grid_t& grid,
	const double simulation_time
) {
	ostringstream time_string;
	time_string
		<< std::setw(4)
		<< std::setfill('0')
		<< size_t(simulation_time * 1000);

	const string
		gnuplot_file_name(
			"particle_"
			+ time_string.str()
			+ ".dat"
		),
		plot_file_name(
			"particle_"
			+ time_string.str()
			+ ".png"
		);

	ofstream gnuplot_file(gnuplot_file_name);

	gnuplot_file
		<< "set term png enhanced\nset output '"
		<< plot_file_name
		<< "'\nset xlabel 'X'\n"
		   "set ylabel 'Y'\n"
		   "set size square\n"
		   "set xrange [-1 : 1]\n"
		   "set yrange [-1 : 1]\n"
		   "plot '-' using 1:2:3 linecolor palette pointtype 1 title ''\n";

	// color the particles for easier visualization
	size_t color = 0;

	for (const auto& row: grid)
	for (const auto& cell: row)
	for (const auto& coordinate: cell[Particles()]) {
		gnuplot_file
			<< coordinate[0] << " "
			<< coordinate[1] << " "
			<< color << "\n";
		color++;
	}
	gnuplot_file << "end" << endl;

	return gnuplot_file_name;
}


/*!
Returns the center of cell located at given indices in given grid.

First index is the cell's location in horizontal direction (x),
second in vertical (y).
*/
array<double, 2> get_cell_center(
	const grid_t& grid,
	const array<size_t, 2>& index
) {
	if (
		index[1] >= grid.size()
		or index[0] >= grid[index[1]].size()
	) {
		return {
			std::numeric_limits<double>::quiet_NaN(),
			std::numeric_limits<double>::quiet_NaN()
		};
	}

	return {
		-1.0 + (0.5 + index[0]) * 2.0 / grid[index[1]].size(),
		-1.0 + (0.5 + index[1]) * 2.0 / grid.size()
	};
}


/*!
Returns the size of given cell in x and y directions
at given x and y indices in the grid.
*/
array<double, 2> get_cell_size(
	const grid_t& grid,
	const array<size_t, 2>& index
) {
	if (
		index[1] >= grid.size()
		or index[0] >= grid[index[1]].size()
	) {
		return {
			std::numeric_limits<double>::quiet_NaN(),
			std::numeric_limits<double>::quiet_NaN()
		};
	}

	return {
		2.0 / grid.size(),
		2.0 / grid[index[1]].size()
	};
}


/*!
Initializes cells in given grid with 3 particles
*/
void initialize(grid_t& grid)
{
	for (size_t row_i = 0; row_i < height; row_i++)
	for (size_t cell_i = 0; cell_i < width; cell_i++) {

		const array<double, 2>
			center = get_cell_center(grid, {cell_i, row_i}),
			cell_size = get_cell_size(grid, {cell_i, row_i});

		cell_t& cell = grid[row_i][cell_i];
		cell[Velocity()][0] = -2 * center[1];
		cell[Velocity()][1] = +2 * center[0];

		// don't create particles too close to the edges
		if (
			row_i < height / 4
			or row_i >= height - height / 4
			or cell_i < width / 4
			or cell_i >= width - width / 4
		) {
			continue;
		}

		cell[Particles()].push_back({
				center[0] - cell_size[0] / 4,
				center[1] - cell_size[1] / 4
		});
		cell[Particles()].push_back({
				center[0],
				center[1] + cell_size[1] / 4
		});
		cell[Particles()].push_back({
				center[0] + cell_size[0] / 4,
				center[1] - cell_size[1] / 4
		});
	}
}


/*!
Returns the maximum allowed time step in given grid.
*/
double get_max_time_step(const grid_t& grid)
{
	double ret_val = std::numeric_limits<double>::max();

	for (size_t y_i = 0; y_i < grid.size(); y_i++)
	for (size_t x_i = 0; x_i < grid[y_i].size(); x_i++) {

		const array<double, 2>
			cell_size = get_cell_size(grid, {x_i, y_i}),
			vel = grid[x_i][y_i][Velocity()];

		ret_val =
			min(ret_val,
			min(fabs(cell_size[0] / vel[0]),
			    fabs(cell_size[1] / vel[1])));
	}

	return ret_val;
}


/*!
Propagates particles in each cell of given grid over given time.

Does not change the cell in which a particle is stored.
*/
void solve(grid_t& grid, const double dt)
{
	for (size_t y_i = 0; y_i < height; y_i++)
	for (size_t x_i = 0; x_i < width; x_i++) {

		cell_t& cell = grid[y_i][x_i];

		for (auto& particle: cell[Particles()]) {

			particle[0] += cell[Velocity()][0] * dt;
			particle[1] += cell[Velocity()][1] * dt;

			// make sure particles stay inside the grid
			for (size_t
				dimension = 0;
				dimension < particle.size();
				dimension++
			) {
				if (particle[dimension] < -1.0) {
					particle[dimension] += 2.0;
				}
				if (particle[dimension] > 1.0) {
					particle[dimension] -= 2.0;
				}
			}
		}
	}
}


/*!
Moves particles to the coordinate list of the cell in which
they are currently located at.

Assumes particles have propagated no further from their
previous cell than one of the cell's nearest neighbors.
*/
void apply_solution(grid_t& grid)
{
	for (size_t y_i = 0; y_i < height; y_i++)
	for (size_t x_i = 0; x_i < width; x_i++) {

		cell_t& cell = grid[y_i][x_i];
		const auto
			cell_center = get_cell_center(grid, {x_i, y_i}),
			cell_size = get_cell_size(grid, {x_i, y_i});

		// shorten the notation for current coordinate list
		vector<array<double, 2>>& coords = cell[Particles()];

		for (size_t particle_i = 0; particle_i < coords.size(); particle_i++) {

			// copy the coordinate so the original can be removed
			const auto coordinate = coords[particle_i];

			if (
				coordinate[0] >= cell_center[0] - cell_size[0] / 2
				and coordinate[0] <= cell_center[0] + cell_size[0] / 2
				and coordinate[1] >= cell_center[1] - cell_size[1] / 2
				and coordinate[1] <= cell_center[1] + cell_size[1] / 2
			) {
				// particle is still inside the same cell
				continue;
			} else {
				coords.erase(coords.begin() + particle_i);
			}

			// assign the particle to the closest neighbor
			size_t
				closest_x = numeric_limits<size_t>::max(),
				closest_y = numeric_limits<size_t>::max();
			double closest_distance = numeric_limits<double>::max();

			for (size_t y_offset: {size_t(1), size_t(0), height - 1})
			for (size_t x_offset: {size_t(1), size_t(0), width - 1}) {

				if (x_offset == 0 and y_offset == 0) {
					continue;
				}

				const size_t
					neighbor_x = (x_i + x_offset) % width,
					neighbor_y = (y_i + y_offset) % height;

				const auto neighbor_center = get_cell_center(
					grid,
					{neighbor_x, neighbor_y}
				);

				const double distance
					= sqrt(
						pow(coordinate[0] - neighbor_center[0], 2)
						+ pow(coordinate[1] - neighbor_center[1], 2)
					);

				if (closest_distance > distance) {
					closest_distance = distance;
					closest_x = neighbor_x;
					closest_y = neighbor_y;
				}
			}

			cell_t& neighbor = grid[closest_y][closest_x];
			neighbor[Particles()].push_back(coordinate);
		}
	}
}


int main(int, char**)
{
	constexpr size_t rotations = 1;

	constexpr double save_interval = 0.1;

	initialize(grid);

	string gnuplot_file_name = save(grid, 0);
	system(("gnuplot " + gnuplot_file_name).c_str());

	double
		simulation_time = 0,
		next_save = save_interval;
	while (simulation_time < rotations * M_PI) {

		// figure out the maximum time step to use
		const double
			max_dt = get_max_time_step(grid),
			CFL = 0.5,
			time_step = CFL * max_dt;

		solve(grid, time_step);
		apply_solution(grid);
		simulation_time += time_step;

		if (next_save <= simulation_time) {
			next_save += save_interval;

			gnuplot_file_name = save(grid, simulation_time);
			system(("gnuplot " + gnuplot_file_name).c_str());
		}
	}

	gnuplot_file_name = save(grid, simulation_time);
	system(("gnuplot " + gnuplot_file_name).c_str());

	return EXIT_SUCCESS;
}
