#include "MineField.hpp"
#include <random>

void Minefield::initRandom(int width, int height, int total_bombs, int x_clickedOn, int y_clickedOn) {

    showField.clear();
    bombs.clear();

    // one additional tile on each side to not do border and corner checks
    for (int i = 0; i < height + 2; i++) {
        showField.push_back(std::vector<Tile>());
        bombs.push_back(std::vector<bool>());
        for (int j = 0; j < width + 2; j++) {
            showField[i].push_back(Tile::HIDDEN);
            bombs[i].push_back(false);
        }
    }

    for (int i = 0; i < height + 2; i++) {
        showField[i][0] = Tile::EMPTY;
        showField[i][width + 1] = Tile::EMPTY;
    }

    for (int j = 0; j < width + 2; j++) {
        showField[0][j] = Tile::EMPTY;
        showField[height + 1][j] = Tile::EMPTY;
    }

    std::random_device rnd;
    std::mt19937 gen(rnd());

    std::uniform_int_distribution<int> x_distr(1, width);
    std::uniform_int_distribution<int> y_distr(1, height);

    int bombs_placed = 0;
    int x_next, y_next;
    int total_iterations = 0; // to prevent infinite loops we cap at 5000 iterations

    bool checkSurroundings = false;

    x_clickedOn += 1;
    y_clickedOn += 1;

    while (bombs_placed < total_bombs && total_iterations < 5000) {
        x_next = x_distr(gen);
        y_next = y_distr(gen);

        checkSurroundings =
            (x_clickedOn == x_next - 1 && y_clickedOn == y_next - 1) ||
            (x_clickedOn == x_next - 1 && y_clickedOn == y_next) ||
            (x_clickedOn == x_next - 1 && y_clickedOn == y_next + 1) ||
            (x_clickedOn == x_next && y_clickedOn == y_next - 1) ||
            (x_clickedOn == x_next && y_clickedOn == y_next) ||
            (x_clickedOn == x_next && y_clickedOn == y_next + 1) ||
            (x_clickedOn == x_next + 1 && y_clickedOn == y_next - 1) ||
            (x_clickedOn == x_next + 1 && y_clickedOn == y_next) ||
            (x_clickedOn == x_next + 1 && y_clickedOn == y_next + 1);

        if (checkSurroundings || bombs[y_next][x_next]) {
            total_iterations++;
            continue;
        }
        bombs[y_next][x_next] = true;
        bombs_placed++;
        total_iterations++;
    }

    field_width = width;
    field_height = height;
}

const std::vector<std::vector<Tile>>& Minefield::getField() const {
    return showField;
}

int getNumberOfSurroundingBombs(const std::vector<std::vector<bool>>& bombs, int x_coord, int y_coord) {

    int result = 0;

    result += (int)bombs[y_coord][x_coord - 1];
    result += (int)bombs[y_coord - 1][x_coord - 1];
    result += (int)bombs[y_coord + 1][x_coord - 1];
    result += (int)bombs[y_coord][x_coord + 1];
    result += (int)bombs[y_coord - 1][x_coord + 1];
    result += (int)bombs[y_coord + 1][x_coord + 1];
    result += (int)bombs[y_coord - 1][x_coord];
    result += (int)bombs[y_coord + 1][x_coord];

    return result;
}

void recursiveReveal(std::vector<std::vector<Tile>>& tiles, const std::vector<std::vector<bool>>& bombs, int x_coord, int y_coord) {
    // out of bounds check
    if (x_coord < 1 || y_coord < 1 || y_coord >= bombs.size() - 1 || x_coord >= bombs[y_coord].size() - 1) {
        return;
    }

    // if it was revealed before
    if (tiles[y_coord][x_coord] != Tile::HIDDEN && tiles[y_coord][x_coord] != Tile::FLAG && tiles[y_coord][x_coord] != Tile::BOMB) {
        return;
    }

    if (!bombs[y_coord][x_coord]) {
        int numberOfSurroundingBombs = getNumberOfSurroundingBombs(bombs, x_coord, y_coord);
        tiles[y_coord][x_coord] = (Tile)numberOfSurroundingBombs;

        if (numberOfSurroundingBombs == 0) {
            recursiveReveal(tiles, bombs, x_coord - 1, y_coord);
            recursiveReveal(tiles, bombs, x_coord + 1, y_coord);
            recursiveReveal(tiles, bombs, x_coord - 1, y_coord - 1);
            recursiveReveal(tiles, bombs, x_coord, y_coord - 1);
            recursiveReveal(tiles, bombs, x_coord + 1, y_coord - 1);
            recursiveReveal(tiles, bombs, x_coord - 1, y_coord + 1);
            recursiveReveal(tiles, bombs, x_coord, y_coord + 1);
            recursiveReveal(tiles, bombs, x_coord + 1, y_coord + 1);
        }

    }
    else {
        tiles[y_coord][x_coord] = Tile::BOMB;
    }
}

void revealAll(std::vector<std::vector<Tile>>& tiles, const std::vector<std::vector<bool>>& bombs) {
    int numberOfSurroundingBombs = 0;

    for (int i = 1; i < tiles.size() - 1; i++) {
        for (int j = 1; j < tiles[i].size() - 1; j++) {
            if (bombs[i][j]) {
                tiles[i][j] = Tile::BOMB;
            }
            else {
                numberOfSurroundingBombs = getNumberOfSurroundingBombs(bombs, i, j);
                tiles[i][j] = (Tile)numberOfSurroundingBombs;
            }
        }
    }
}

bool Minefield::reveal(int x_coord, int y_coord) {
    // + 1 because rim is invisible

    if (bombs[y_coord + 1][x_coord + 1]) {
        revealAll(showField, bombs);
        return false;
    }

    Tile tile = showField[y_coord + 1][x_coord + 1];
    if (tile != Tile::FLAG) {
        recursiveReveal(showField, bombs, x_coord + 1, y_coord + 1);
        return true;
    }
}

void Minefield::mark(int x_coord, int y_coord) {
    
    if (showField[y_coord + 1][x_coord + 1] == Tile::HIDDEN) {
        showField[y_coord + 1][x_coord + 1] = Tile::FLAG;
    }
}

void Minefield::unmark(int x_coord, int y_coord) {
    if (showField[y_coord + 1][x_coord + 1] == Tile::FLAG) {
        showField[y_coord + 1][x_coord + 1] = Tile::HIDDEN;
    }
}

Tile Minefield::getAt(int x_coord, int y_coord)
{
    return showField[y_coord + 1][x_coord + 1];
}

int Minefield::getWidth()
{
    return field_width;
}

int Minefield::getHeight()
{
    return field_height;
}

