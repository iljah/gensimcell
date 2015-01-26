/*
Game of Life example not implemented using cell-based storage.

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

#include "array"
#include "chrono"
#include "cstdlib"
#include "iostream"

using namespace std;
using namespace std::chrono;

struct Is_Alive {};
struct Live_Neighbors {};

/*!
Grid which stores each variable in a separate contiguous array.
*/
template <
	size_t Width,
	size_t Height
> class Grid {

private:
	array<array<bool, Width>, Height> is_alive;
	array<array<int, Width>, Height> live_neighbors;
	/*array<bool, Width * Height> is_alive;
	array<int, Width * Height> live_neighbors;*/

	// implements last operator [] in grid[row][col][var]
	struct cell_proxy {
		Grid& grid;
		const size_t row, col;

		cell_proxy(
			Grid& given_grid,
			const size_t given_row,
			const size_t given_col
		) :
			grid(given_grid),
			row(given_row),
			col(given_col)
		{};

		bool& operator[](const Is_Alive&) {
			return this->grid.is_alive[this->row][this->col];
			//return this->grid.is_alive[this->row * Height + this->col];
		}
		int& operator[](const Live_Neighbors&) {
			return this->grid.live_neighbors[this->row][this->col];
			//return this->grid.live_neighbors[this->row * Height + this->col];
		}
	};

	// const version of above
	struct cell_proxy_const {
		const Grid& grid;
		const size_t row, col;

		cell_proxy_const(
			const Grid& given_grid,
			const size_t given_row,
			const size_t given_col
		) :
			grid(given_grid),
			row(given_row),
			col(given_col)
		{};

		const bool& operator[](const Is_Alive&) const {
			return this->grid.is_alive[this->row][this->col];
			//return this->grid.is_alive[this->row * Height + this->col];
		}
		const int& operator[](const Live_Neighbors&) const {
			return this->grid.live_neighbors[this->row][this->col];
			//return this->grid.live_neighbors[this->row * Height + this->col];
		}
	};


	// implements middle operator [] in grid[row][col][var]
	struct row_proxy {
		Grid& grid;
		const size_t row;

		row_proxy(
			Grid& given_grid,
			const size_t given_row
		) :
			grid(given_grid),
			row(given_row)
		{};

		cell_proxy operator[](const size_t given_column) {
			return cell_proxy(this->grid, this->row, given_column);
		}
	};

	// const version of above
	struct row_proxy_const {
		const Grid& grid;
		const size_t row;

		row_proxy_const(
			const Grid& given_grid,
			const size_t given_row
		) :
			grid(given_grid),
			row(given_row)
		{};

		cell_proxy_const operator[](const size_t given_column) const {
			return cell_proxy_const(this->grid, this->row, given_column);
		}
	};


public:

	static constexpr size_t
		height = Height,
		width = Width;


	bool& operator()(
		const Is_Alive&,
		const size_t row,
		const size_t column
	) {
		return this->is_alive[row][column];
		//return this->is_alive[row * Height + column];
	}

	const bool& operator()(
		const Is_Alive&,
		const size_t row,
		const size_t column
	) const {
		return this->is_alive[row][column];
		//return this->is_alive[row * Height + column];
	}


	int& operator()(
		const Live_Neighbors&,
		const size_t row,
		const size_t column
	) {
		return this->live_neighbors[row][column];
		//return this->live_neighbors[row * Height + column];
	}

	const int& operator()(
		const Live_Neighbors&,
		const size_t row,
		const size_t column
	) const {
		return this->live_neighbors[row][column];
		//return this->live_neighbors[row * Height + column];
	}


	/*!
	Enables [] syntax for accessing grid data.

	Usage example: cout << grid[row][col][Is_Alive()];
	*/
	row_proxy operator[](const size_t row) {
		return row_proxy(*this, row);
	}

	// const version of above
	const row_proxy_const operator[](const size_t row) const {
		return row_proxy_const(*this, row);
	}
};


/*template<class Grid_T> void print_gol(const Grid_T& grid)
{
	for (size_t row = 0; row < grid.height; row++) {
	for (size_t cell = 0; cell < grid.width; cell++) {
			if (grid[row][cell][Is_Alive()]) {
				cout << "0";
			} else {
				cout << ".";
			}
		}
		cout << endl;
	}
	cout << endl;
}*/


int main(int, char**)
{
	constexpr size_t
		width = 100,
		height = 100;

	Grid<width, height> grid;

	for (size_t row = 0; row < grid.height; row++) {
	for (size_t cell = 0; cell < grid.width; cell++) {
		grid(Is_Alive(), row, cell) = false;
		grid(Live_Neighbors(), row, cell) = 0;
	}}
	grid[1][2][Is_Alive()] = true;
	grid[2][3][Is_Alive()] = true;
	grid[3][3][Is_Alive()] = true;
	grid[3][2][Is_Alive()] = true;
	grid[3][1][Is_Alive()] = true;

	const auto time_start = high_resolution_clock::now();

	constexpr size_t max_turns = 30000;
	for (size_t turn = 0; turn < max_turns; turn++) {

		// collect live neighbor counts, use periodic boundaries
		for (size_t row = 0; row < grid.height; row++)
		for (size_t cell = 0; cell < grid.width; cell++) {

			int& current_live_neighbors
				= grid[row][cell][Live_Neighbors()];
				//= grid(Live_Neighbors(), row, cell);

			for (auto row_offset: {size_t(1), size_t(0), width - 1}) 
			for (auto cell_offset: {size_t(1), size_t(0), height - 1}) {

				if (row_offset == 0 and cell_offset == 0) {
					continue;
				}

				const bool& neighbor_is_alive
					= grid[
						(row + row_offset) % height
					][
						(cell + cell_offset) % width
					][
						Is_Alive()
					];
					/*= grid(
						Is_Alive(),
						(row + row_offset) % height,
						(cell + cell_offset) % width
					);*/

				if (neighbor_is_alive) {
					current_live_neighbors++;
				}
			}
		}

		// set new state
		for (size_t row = 0; row < grid.height; row++)
		for (size_t cell = 0; cell < grid.width; cell++) {

			bool& is_alive = grid[row][cell][Is_Alive()];
			int& live_neighbors = grid[row][cell][Live_Neighbors()];
			/*bool& is_alive = grid(Is_Alive(), row, cell);
			int& live_neighbors = grid(Live_Neighbors(), row, cell);*/
			if (live_neighbors == 3) {
				is_alive = true;
			} else if (live_neighbors != 2) {
				is_alive = false;
			}
			live_neighbors = 0;
		}
	}

	const auto time_end = high_resolution_clock::now();

	size_t number_of_live_cells = 0;
	for (size_t row = 0; row < grid.height; row++) {
	for (size_t cell = 0; cell < grid.width; cell++) {
		if (grid[row][cell][Is_Alive()]) {
			number_of_live_cells++;
		}
	}}

	if (number_of_live_cells != 5) {
		std::cerr << __FILE__ << ":" << __LINE__ << " FAILED" << std::endl;
		abort();
	}

	cout << duration_cast<duration<double>>(time_end - time_start).count() << " s" << endl;

	return EXIT_SUCCESS;
}
