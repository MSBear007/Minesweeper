#include "MineSolver.hpp"
#include <stack>
#include <iostream>

const std::vector <std::pair<int, int>> innerCellSurrounding = { 
	std::pair<int, int>(0, 1),
	std::pair<int, int>(0, -1),
	std::pair<int, int>(-1, -1), 
	std::pair<int, int>(-1, 0), 
	std::pair<int, int>(-1, 1), 
	std::pair<int, int>(1, -1), 
	std::pair<int, int>(1, 0), 
	std::pair<int, int>(1, 1) 
};


MineSolver::MineSolver(Minefield minefield, int lvl) {
	level = lvl;
	mines = minefield;
}

void MineSolver::setLevel(int lvl) {
	level = lvl;
}

int MineSolver::getLevel() {
	return level;
}

Minefield MineSolver::getMinefield()
{
	return mines;
}

bool MineSolver::checkSurroundings(int x_coord, int y_coord, const std::vector<std::pair<int, int>>& surrounding) {

	bool result = false;

	int number_to_check = (int)mines.getAt(x_coord, y_coord);
	if (number_to_check == 0 || number_to_check > 8) {
		return false;
	}

	// checking surrounding cells. If we did put flag there before, substract 1 from the number in the cell.
	for (auto pair : surrounding) {
		number_to_check -= (int)(mines.getAt(x_coord + pair.second, y_coord + pair.first) == Tile::FLAG);
	}

	int number_of_hiddens = 0;
	// how many cells are left to reveal?
	for (auto pair : surrounding) {
		number_of_hiddens += (int)(mines.getAt(x_coord + pair.second, y_coord + pair.first) == Tile::HIDDEN);
	}

	if (number_to_check == number_of_hiddens && number_of_hiddens > 0) { // put flags
		result = true;
		for (auto pair : surrounding) {
			mines.mark(x_coord + pair.second, y_coord + pair.first);
		}
	}

	if (number_to_check == 0 && number_of_hiddens > 0) { // reveal
		result = true;
		for (auto pair: surrounding) {
			if (mines.getAt(x_coord + pair.second, y_coord + pair.first) != Tile::FLAG)
				mines.reveal(x_coord + pair.second, y_coord + pair.first);
		}
	}
	return result;
}

void MineSolver::solveLevelZero(int width, int height) {
	bool check_is_successful = true;
	int iterations = 0;
	do { // we check field on this "level" until there is nothing to do
		check_is_successful = false;

		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				check_is_successful = check_is_successful || checkSurroundings(j, i, innerCellSurrounding);
			}
		}

		iterations++;
	} while (check_is_successful && iterations < 1000);
}

// forms the system of linear equations to solve small chunk of the field (lvl 1 - 4x4, lvl 2 - 5x5, lvl 3 - 6x6 etc)
std::vector<std::vector<int>> MineSolver::formSLESystem(int x_coord, int y_coord, int window_size)
{
	// checking if there are both revealed cells and hidden cells in the window interior, otherwise there is no point of solving SLE
	int numbers_in_window = 0;
	int hiddens_in_window = 0;
	for (int i = y_coord + 1; i < y_coord + window_size - 1; i++) {
		for (int j = x_coord + 1; j < x_coord + window_size - 1; j++) {
			numbers_in_window += (int)tileIsNumber(mines.getAt(j, i));
		}
	}
	std::vector<std::pair<int, int>> hiddens; // needed to determine the order of variables
	for (int i = y_coord; i < y_coord + window_size; i++) {
		for (int j = x_coord; j < x_coord + window_size; j++) {
			if ((int)(mines.getAt(j, i) == Tile::HIDDEN) && !borderCondition(j, i)) {
				hiddens_in_window ++;
				hiddens.push_back(std::pair<int, int>(i, j));
			}
		}
	}

	
	if (numbers_in_window == 0 || hiddens_in_window == 0) {
		return std::vector<std::vector<int>>();
	}
	// else
	std::vector<std::vector<int>> coeff_matr(numbers_in_window);

	for (int i = 0; i < numbers_in_window; i++) {
		for (int j = 0; j < hiddens_in_window + 1; j++) {
			coeff_matr[i].push_back(0);
		}
	}

	int row = 0;
	int number = 0;
	for (int i = y_coord + 1; i < y_coord + window_size - 1; i++) {
		for (int j = x_coord + 1; j < x_coord + window_size - 1; j++) {

			if (tileIsNumber(mines.getAt(j, i))) {

				int number = (int)mines.getAt(j, i);

				number = number;
				//adding row to the matrix
				for (auto neighbor : innerCellSurrounding) {
					if (mines.getAt(j + neighbor.second, i + neighbor.first) == Tile::HIDDEN) {
						auto found = std::find(hiddens.begin(), hiddens.end(), std::pair<int, int>(i + neighbor.first, j + neighbor.second));
						coeff_matr[row][std::distance(hiddens.begin(), found)] = 1;
					}
					else if (mines.getAt(j + neighbor.second, i + neighbor.first) == Tile::FLAG) {
						number--;
					}
				}

				// adding element to the vector
				coeff_matr[row][hiddens_in_window] = number;
				row++;
			}
		}
	}

	return coeff_matr;

}

bool MineSolver::borderCondition(int x, int y) {
	return x == -1 || y == -1 || x == mines.getWidth() + 1 || y == mines.getHeight() + 1;
}

bool MineSolver::applySolution(const std::vector<int>& solution, int x_coord, int y_coord, int window_size) {

	std::vector<std::pair<int, int>> hiddens; // needed to determine the order of variables
	for (int i = y_coord; i < y_coord + window_size; i++) {
		for (int j = x_coord; j < x_coord + window_size; j++) {
			if ((int)(mines.getAt(j, i) == Tile::HIDDEN) && !borderCondition(j, i)) {
				hiddens.push_back(std::pair<int, int>(i, j));
			}
		}
	}

	bool solution_exists = false;

	for (int i = 0; i < solution.size(); i++) {
		if (solution[i] == 1) {
			mines.mark(hiddens[i].second, hiddens[i].first);
			solution_exists = true;
		}
		if (solution[i] == 0) {
			mines.reveal(hiddens[i].second, hiddens[i].first);
			solution_exists = true;
		}
	}
	return solution_exists;
}

// returns number of ones in binary representation of a number
// I.e 0001111001 returns 5, as there is 5 ones in the number
// proof: https://web.archive.org/web/20151229003112/http://blogs.msdn.com/b/jeuge/archive/2005/06/08/hakmem-bit-count.aspx
inline int bitCount(unsigned int u)
{
	unsigned int uCount;

	uCount = u - ((u >> 1) & 033333333333) - ((u >> 2) & 011111111111);
	return ((uCount + (uCount >> 3)) & 030707070707) % 63;
}

// first test
std::vector<int> solveSystem(std::vector<std::vector<int>>& system) {

	int rows = system.size();
	int columns = system[0].size();
 

	//-1 if we cant tell anything, 0 if there is no bomb, 1 if there is a bomb
	std::vector<int> solution(columns - 1);
	for (int i = 0; i < columns - 1; i++) {
		solution[i] = -1;
	}

	if (columns > 15) return solution; // too hard, skip it

	// we will generate all possible combinations and check if they satisfy our condition
	// to be improved... maybe :-)

	std::vector<int> row_masks = std::vector<int>();
	int current_mask;
	int all_masks_combined = 0; // OR all masks
	for (int i = 0; i < rows; i++) {
		current_mask = 0;
		for (int j = 0; j < columns - 1; j++) {
			current_mask <<= 1;
			current_mask += system[i][j];
		}
		all_masks_combined |= current_mask;
		row_masks.push_back(current_mask);
	}

	unsigned int bomb_positions = 0xffffffff;
	unsigned int safe_positions = 0;

	int total = 0;
	for (unsigned int i = 0; i < (1 << (columns - 1)); i++) { // iterate over each possible combination
		total++;
		for (int j = 0; j < row_masks.size(); j++) {
			if (bitCount(i & row_masks[j]) != system[j][columns - 1]) { // check the mask condition
				goto doesnt_satisfy; // skip calculations, go straight to the next iteration
			}
		}
		bomb_positions &= i & all_masks_combined; // if "1" stays after end of the loop, then there is a bomb necessary
		safe_positions |= i | ~all_masks_combined; // if "0" stays after end of the loop, then cell is safe to click necessary
		if (bomb_positions == 0 && safe_positions == 0xffffffff) break;
	doesnt_satisfy:;
	}

	for (int i = solution.size() - 1; i >= 0; i--) {
		if (safe_positions % 2 == 0) solution[i] = 0;
		if (bomb_positions % 2 == 1) solution[i] = 1;
		safe_positions >>= 1;
		bomb_positions >>= 1;
	}

	return solution;
}

// using systems of linear equations to solve small part of the board;
// we check larger and larger parts of the board with each level
bool MineSolver::solveLevelN(int width, int height, int lvl) {

	int window_size = lvl + 3;

	const std::vector<std::vector<Tile>>& field = mines.getField();

	std::vector<int> solution;
	bool solution_exists = false;
	for (int i = -1; i <= height - window_size + 1; i++) {
		for (int j = -1; j <= width - window_size + 1; j++) {
			
			auto system = formSLESystem(j, i, window_size);
			if (!system.size() == 0) {

				solution = solveSystem(system);
				solution_exists |= applySolution(solution, j, i, window_size);
			}
		}
	}
	return solution_exists;
}

void MineSolver::solve() {


	int height = mines.getHeight();
	int width = mines.getWidth();

	std::stack<int> levels;
	levels.push(0);

	bool check_is_successful = false;
	int cur_lvl = 0;
	// we solve at level N. If we didn't reveal any cells, we go to the level N + 1. Else return to the previous level.
	// if we're at the maximum level, we're done.
	while ((cur_lvl = levels.top()) <= level) {

		if (cur_lvl == 0) {
			// simplest case when the number of hidden cells around a number is equal to this number, covers majority of the work
			solveLevelZero(width, height);
			levels.push(cur_lvl + 1);
		}
		else {
			check_is_successful = solveLevelN(width, height, cur_lvl);
			if (check_is_successful) {
				levels.pop();
			}
			else {
				levels.push(cur_lvl + 1);
			}
		}
	}

}

