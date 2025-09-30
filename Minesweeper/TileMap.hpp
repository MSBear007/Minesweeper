#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Tile.hpp"


//in tiles
const int TILES_TEXTURE_WIDTH = 3;

//in tiles
const int TILES_TEXTURE_HEIGHT = 4;

class TileMap : public sf::Drawable, public sf::Transformable {

	sf::VertexArray m_vertices;

	sf::Texture m_tileset;

	int m_width;
	int m_height;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
public:

	TileMap(int width, int height, sf::Texture &tileset);
	void load(const std::vector<std::vector<Tile>> &tilemap, sf::Vector2i tilesize);

	void setTile(sf::Vector2i coords, Tile tile, sf::Vector2i tilesize);
};