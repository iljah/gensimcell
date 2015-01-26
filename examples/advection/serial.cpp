/*
Serial example program solving the advection equation.

Copyright 2013, 2014, 2015 Ilja Honkonen
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
#include "boost/range/adaptor/reversed.hpp"
#include "cmath"
#include "cstdlib"
#include "fstream"
#include "iomanip"
#include "iostream"
#include "limits"
#include "sstream"
#include "string"

#include "gensimcell.hpp"


using namespace std;


//! see ../game_of_life/serial.cpp for basics

struct Density
{
	using data_type = double;
};

struct Density_Flux
{
	using data_type = double;
};

struct Velocity
{
	using data_type = std::array<double, 2>;
};

using Cell_T = gensimcell::Cell<
	gensimcell::Never_Transfer,
	Density,
	Density_Flux,
	Velocity
>;


//! horizontal and vertical size of the grid
constexpr size_t
	width = 20,
	height = 20;

/*!
Use row wise storage so, for example, to access
the cell at horizontal (x) index = 2 and
vertical (y) index = 4 use grid[4][2]
(indices start from 0)
*/
using Grid_T = array<array<Cell_T, width>, height>;
Grid_T grid;


/*!
Returns the center of cell located at given indices in given grid.

First index is the cell's location in horizontal direction (x),
second in vertical (y). Indices increase in positive direction.
*/
array<double, 2> get_cell_center(
	const Grid_T& grid,
	const array<size_t, 2>& index
) {
	if (
		index[1] >= grid.size()
		or index[0] >= grid[index[1]].size()
	) {
		return {{
			std::numeric_limits<double>::quiet_NaN(),
			std::numeric_limits<double>::quiet_NaN()
		}};
	}

	return {{
		-1.0 + (0.5 + index[0]) * 2.0 / grid[index[1]].size(),
		-1.0 + (0.5 + index[1]) * 2.0 / grid.size()
	}};
}


/*!
Returns the size of given cell in x and y directions
at given x and y indices in the grid.
*/
array<double, 2> get_cell_size(
	const Grid_T& grid,
	const array<size_t, 2>& index
) {
	if (
		index[1] >= grid.size()
		or index[0] >= grid[index[1]].size()
	) {
		return {{
			std::numeric_limits<double>::quiet_NaN(),
			std::numeric_limits<double>::quiet_NaN()
		}};
	}

	return {{
		2.0 / grid.size(),
		2.0 / grid[index[1]].size()
	}};
}


/*!
Initializes given grid with a density of 0 except for a square and a circle.

The initial condition is from chapter 20.8.2 in
R.J. LeVeque,
Finite Volume Methods for Hyperbolic Problems,
ISBN 978-0-521-00924-9.
*/
void initialize(Grid_T& grid)
{
	for (size_t row_i = 0; row_i < grid.size(); row_i++)
	for (size_t cell_i = 0; cell_i < grid[row_i].size(); cell_i++) {

		const array<double, 2> center = get_cell_center(grid, {{cell_i, row_i}});
		const double r = sqrt(pow(center[0] + 0.45, 2) + pow(center[1], 2));

		auto& cell = grid[row_i][cell_i];

		/*
		Initialize density
		*/
		cell[Density()]      =
		cell[Density_Flux()] = 0;

		// square
		if (
			center[0] > 0.1
			and center[0] < 0.6
			and center[1] > -0.25
			and center[1] < 0.25
		) {
			cell[Density()] = 1;

		// cone
		} else if (r < 0.35) {
			cell[Density()] = 1 - r / 0.35;
		}

		/*
		Initialize velocity
		*/
		cell[Velocity()] = {{
			+2 * center[1],
			-2 * center[0]
		}};
	}
}


/*!
Returns the maximum allowed time step in given grid.
*/
double get_max_time_step(const Grid_T& grid)
{
	double ret_val = std::numeric_limits<double>::max();

	for (size_t y_i = 0; y_i < grid.size(); y_i++)
	for (size_t x_i = 0; x_i < grid[y_i].size(); x_i++) {

		const array<double, 2>
			cell_size = get_cell_size(grid, {{x_i, y_i}}),
			vel = grid[x_i][y_i][Velocity()];

		ret_val =
			min(ret_val,
			min(fabs(cell_size[0] / vel[0]),
			    fabs(cell_size[1] / vel[1])));
	}

	return ret_val;
}


/*!
Calculates the flux of advected density into (+) and
out of (-) each cell in given grid over given time.

Advection is calculated only into four edge neighbors of each cell.
Velocity does not change during the simulation.

The solver is probably simplest possible
so it has e.g. very large diffusion.
*/
void solve(Grid_T& grid, const double dt)
{
	static_assert(
		std::tuple_size<Grid_T>::value > 0,
		"Given grid must have at least one cell in first dimension"
	);
	static_assert(
		std::tuple_size<Grid_T::value_type>::value > 0,
		"Given grid must have at least one cell in second dimension"
	);

	for (size_t y_i = 0; y_i < height; y_i++)
	for (size_t x_i = 0; x_i < width; x_i++) {

		auto& cell = grid[y_i][x_i];

		const double density = cell[Density()];
		const array<double, 2>
			cell_size = get_cell_size(grid, {{x_i, y_i}}),
			velocity = cell[Velocity()];

		const double
			// advection out of current cell in x direction
			flux_x = density * (velocity[0] * dt / cell_size[0]),
			// advection out of current cell in y direction
			flux_y = density * (velocity[1] * dt / cell_size[1]);

		// save the flux out of the current cell
		cell[Density_Flux()] -= fabs(flux_x) + fabs(flux_y);

		// figure out where the density from current cell goes
		if (flux_x < 0) {
			// stuff flows into neighbor on the negative x side
			Cell_T& neighbor = grid[y_i][(x_i + (width - 1)) % width];
			neighbor[Density_Flux()] += fabs(flux_x);
		} else {
			// stuff flows into neighbor on the positive x side
			Cell_T& neighbor = grid[y_i][(x_i + 1) % width];
			neighbor[Density_Flux()] += flux_x;
		}

		if (flux_y < 0) {
			// stuff flows into neighbor on the negative y side
			Cell_T& neighbor = grid[(y_i + (height - 1)) % height][x_i];
			neighbor[Density_Flux()] += fabs(flux_y);
		} else {
			// stuff flows into neighbor on the positive y side
			Cell_T& neighbor = grid[(y_i + 1) % height][x_i];
			neighbor[Density_Flux()] += flux_y;
		}
	}
}


/*!
Sets the new density in each cell of given grid based on density flux.

Density flux is zeroed after applying it.
*/
void apply_solution(Grid_T& grid)
{
	for (auto& row: grid)
	for (auto& cell: row) {
		cell[Density()] += cell[Density_Flux()];
		cell[Density_Flux()] = 0;
	}
}


/*!
Writes given advection simulation to a file plottable with gnuplot.

Returns the name of the file written which
is derived from given simulation time.
*/
string save(
	const Grid_T& grid,
	const double simulation_time
) {
	ostringstream time_string;
	time_string
		<< std::setw(4)
		<< std::setfill('0')
		<< size_t(simulation_time * 1000);

	const string
		gnuplot_file_name(
			"advection_"
			+ time_string.str()
			+ ".dat"
		),
		plot_file_name(
			"advection_"
			+ time_string.str()
			+ ".png"
		);

	ofstream gnuplot_file(gnuplot_file_name);

	gnuplot_file
		<< "set term png enhanced\nset output '"
		<< plot_file_name
		<< "'\nset xlabel 'X ([-1, 1])'\n"
		   "set ylabel 'Y ([-1, 1])'\n"
		   "set size square\n"
		   "plot '-' matrix with image title ''\n";

	// plotting 'with image' requires row data from bottom to top
	for (size_t row_i = 0; row_i < grid.size(); row_i++) {
		for (size_t cell_i = 0; cell_i < grid[row_i].size(); cell_i++) {
			const auto& cell = grid[row_i][cell_i];
			gnuplot_file << cell[Density()] << " ";
		}
		gnuplot_file << "\n";
	}
	gnuplot_file << "end" << endl;

	return gnuplot_file_name;
}


int main(int, char**)
{
	// number of rotations around origin
	constexpr size_t rotations = 1;

	// save results after this amount of simulated time
	constexpr double save_interval = 0.1;

	initialize(grid);

	string gnuplot_file_name = save(grid, 0);
	// plot the result if gnuplot is available
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
