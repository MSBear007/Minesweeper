#pragma once
#include "Tile.hpp"
#include "MineField.hpp"
#include <vector>
#include <utility>
#include <Eigen/Dense>

class MineSolver {
private:
	// Level is the max size of the window in which we check for bombs, + 3. (Minumum level of 0 corresponds to window size of 3x3)
	int level;
	Minefield mines;

	bool checkSurroundings(int x_coord, int y_coord, const std::vector<std::pair<int, int>> &surrounding);

	void solveLevelZero(int width, int height);
	bool solveLevelN(int width, int height, int lvl);

	std::vector<std::vector<int>> formSLESystem(int x_coord, int y_coord, int window_size);
	bool borderCondition(int x, int y);
	bool applySolution(const std::vector<int>& solution, int x_coord, int y_coord, int window_size);

public:

	MineSolver(Minefield mines, int lvl);

	void setLevel(int lvl);

	int getLevel();

	Minefield getMinefield();

	void solve();
};