#pragma once
#include <vector>
#include "Tile.hpp"

class Minefield {
private:
	int field_width;
	int field_height;
	std::vector<std::vector<Tile>> showField;
	std::vector<std::vector<bool>> bombs;
public:

	// x_clickedOn and y_clickedOn - coordinates of tile where you clicked first, so no bomb should be placed there
	void initRandom(int width, int height, int total_bombs, int x_clickedOn, int y_clickedOn);

	const std::vector<std::vector<Tile>> &getField() const;

	// returns true, if game is not lost;
	// false if you clicked on a bomb
	bool reveal(int x_coord, int y_coord);

	void mark(int x_coord, int y_coord);
	void unmark(int x_coord, int y_coord);

	Tile getAt(int x_coord, int y_coord);

	int getWidth();
	int getHeight();
};