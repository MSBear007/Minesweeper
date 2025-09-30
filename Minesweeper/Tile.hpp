#pragma once

enum class Tile {
	EMPTY = 0,
	ONE = 1,
	TWO = 2,
	THREE = 3,
	FOUR = 4,
	FIVE = 5,
	SIX = 6,
	SEVEN = 7,
	EIGHT = 8,
	HIDDEN = 9,
	FLAG = 10,
	BOMB = 11
};

inline bool tileIsNumber(Tile t) {
	return (int)t > 0 && (int)t < 9;
}