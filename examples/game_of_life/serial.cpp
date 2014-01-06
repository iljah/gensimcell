#include "array"
#include "cstdlib"
#include "iostream"

#include "gensimcell.hpp"

using namespace std;

/*
Variable that records whether
the cell is alive or not
*/
struct is_alive
{
	typedef bool data_type;
};

/*
Variable that records the
number of cell's live neighbors
*/
struct live_neighbors
{
	typedef int data_type;
};

/*
The cell type stored in the game grid
defined by the variables it holds 
*/
typedef gensimcell::Cell<
	is_alive,
	live_neighbors
> cell_t;


/*
Prints given game of life to standard output,
using 0 for live cells and . for dead cells.
*/
template<class Game_Grid> void print_game(const Game_Grid& grid)
{
	for (auto& row: grid) {
		for (auto& cell: row) {
			if (cell(is_alive())) {
				cout << "0";
			} else {
				cout << ".";
			}
		}
		cout << endl;
	}
	cout << endl;
}


int main(int, char**)
{
	// the game grid
	constexpr size_t
		width = 6,
		height = 6;

	array<array<cell_t, width>, height> game_grid;


	// initialize the game with a glider at upper left
	for (auto& row: game_grid) {
		for (auto& cell: row) {
			cell(is_alive()) = false;
			cell(live_neighbors()) = 0;
		}
	}
	game_grid[1][2](is_alive()) = true;
	game_grid[2][3](is_alive()) = true;
	game_grid[3][3](is_alive()) = true;
	game_grid[3][2](is_alive()) = true;
	game_grid[3][1](is_alive()) = true;


	constexpr size_t max_turns = 4;
	for (size_t turn = 0; turn < max_turns; turn++) {

		print_game(game_grid);

		// collect live neighbor counts, use periodic boundaries
		for (size_t row_i = 0; row_i < game_grid.size(); row_i++)
		for (size_t cell_i = 0; cell_i < game_grid[row_i].size(); cell_i++) {

			cell_t& current_cell = game_grid[row_i][cell_i];

			for (auto row_offset: {size_t(1), size_t(0), width - 1}) 
			for (auto cell_offset: {size_t(1), size_t(0), height - 1}) {

				if (row_offset == 0 and cell_offset == 0) {
					continue;
				}

				const cell_t& neighbor
					= game_grid[
						(row_i + row_offset) % height
					][
						(cell_i + cell_offset) % width
					];

				if (neighbor(is_alive())) {
					current_cell(live_neighbors())++;
				}
			}
		}

		// set new state
		for (size_t row_i = 0; row_i < game_grid.size(); row_i++)
		for (size_t cell_i = 0; cell_i < game_grid[row_i].size(); cell_i++) {

			cell_t& cell = game_grid[row_i][cell_i];
			if (cell(live_neighbors()) == 3) {
				cell(is_alive()) = true;
			} else if (cell(live_neighbors()) != 2) {
				cell(is_alive()) = false;
			}
			cell(live_neighbors()) = 0;
		}
	}

	print_game(game_grid);

	return EXIT_SUCCESS;
}
