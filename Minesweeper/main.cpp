#include <SFML/Graphics.hpp>
#include "TileMap.hpp"
#include "MineField.hpp"
#include "MineSolver.hpp"
#include <iostream>

#define TILESIZE 32
#define BOMBS 99
#define FIELDSIZE 22


int main() {

	sf::RenderWindow window(sf::VideoMode(FIELDSIZE*TILESIZE, FIELDSIZE * TILESIZE), "Minesweeper", sf::Style::Default);
	window.setVerticalSyncEnabled(true);

	sf::Texture tile_texture;
	if (!tile_texture.loadFromFile("..\\tilemap.png")) {
		std::cerr << "Unable to load the texture..." << std::endl;
		system("pause");
		return -1;
	}

	sf::Font font;
	if (!font.loadFromFile("..\\AmaticSC-Regular.ttf")) {
		std::cerr << "Unable to load the font..." << std::endl;
		system("pause");
		return -1;
	}

	sf::Text text("You lost!", font);
	text.setCharacterSize(96);
	text.setFillColor(sf::Color::Red);
	text.setPosition(FIELDSIZE * TILESIZE / 2 - 100, FIELDSIZE * TILESIZE / 2 - 50);
	text.setStyle(sf::Text::Bold);

	sf::CircleShape circle(150, 60);
	circle.setFillColor(sf::Color::White);
	circle.setOrigin(140-FIELDSIZE * TILESIZE / 2, 140-FIELDSIZE * TILESIZE / 2);


	TileMap tilemap(FIELDSIZE, FIELDSIZE, tile_texture);

	std::vector<std::vector<Tile>> tiles;

	for (int i = 0; i < FIELDSIZE + 2; i++) {
		tiles.push_back(std::vector<Tile>());
		for (int j = 0; j < FIELDSIZE + 2; j++) {
			tiles[i].push_back(Tile::HIDDEN);
		}
	}

	tilemap.load(tiles, sf::Vector2i(TILESIZE, TILESIZE));

	sf::Vector2i mouseCoords;
	sf::Vector2i tileCoords;

	bool started = false;
	bool game_not_lost = true;
	bool game_started = false;
	Minefield mines;

	sf::Event event;
	while (window.isOpen()) {

		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}

			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Left) {

				mouseCoords = sf::Mouse::getPosition(window);
				tileCoords.x = mouseCoords.x / TILESIZE;
				tileCoords.y = mouseCoords.y / TILESIZE;

				if (tileCoords.x < FIELDSIZE && tileCoords.y < FIELDSIZE) {
					
					if (!started) {
						mines.initRandom(FIELDSIZE, FIELDSIZE, BOMBS, tileCoords.x, tileCoords.y);
						started = true;
					}

					game_not_lost = mines.reveal(tileCoords.x, tileCoords.y);
					tilemap.load(mines.getField(), sf::Vector2i(TILESIZE, TILESIZE));
				}

				game_started = true;
			}

			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Right) {

				mouseCoords = sf::Mouse::getPosition(window);
				tileCoords.x = mouseCoords.x / TILESIZE;
				tileCoords.y = mouseCoords.y / TILESIZE;


				if (mines.getAt(tileCoords.x, tileCoords.y) == Tile::HIDDEN) {
					mines.mark(tileCoords.x, tileCoords.y);
				} else if (mines.getAt(tileCoords.x, tileCoords.y) == Tile::FLAG) {
					mines.unmark(tileCoords.x, tileCoords.y);
				}
				tilemap.load(mines.getField(), sf::Vector2i(TILESIZE, TILESIZE));
			}

			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::S && game_started) { // solve
				MineSolver solver(mines, 3);
				solver.solve();
				mines = solver.getMinefield();
				tilemap.load(mines.getField(), sf::Vector2i(TILESIZE, TILESIZE));
			}
		}

		window.draw(tilemap);
		if (!game_not_lost) {
			window.draw(circle);
			window.draw(text);
		}
		window.display();
	}

	return 0;
}