#include "TileMap.hpp"
#include <exception>

void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    states.texture = &m_tileset;

    // draw the vertex array
    target.draw(m_vertices, states);
}

TileMap::TileMap(int width, int height, sf::Texture &tileset) {

    m_vertices = sf::VertexArray(sf::PrimitiveType::Quads);
    m_vertices.resize(width * height * 4);

    m_tileset = tileset;

    m_width = width;
    m_height = height;
}

// loads inner cells, excluding the rims of the field
void TileMap::load(const std::vector<std::vector<Tile>>& tilemap, sf::Vector2i tilesize) 
{
    // TODO throw exception if vector size is invalid

    int tilesize_x = tilesize.x;
    int tilesize_y = tilesize.y;
    for (int i = 0; i < tilemap.size() - 2; i++) {
        for (int j = 0; j < tilemap[i].size() - 2; j++) {
            
            //coordinates on screen
            m_vertices[(i * m_width + j) * 4].position = sf::Vector2f(tilesize_x * j, tilesize_y * i);
            m_vertices[(i * m_width + j) * 4 + 1].position = sf::Vector2f(tilesize_x * j + tilesize_x, tilesize_y * i);
            m_vertices[(i * m_width + j) * 4 + 2].position = sf::Vector2f(tilesize_x * j + tilesize_x, tilesize_y * i + tilesize_y);
            m_vertices[(i * m_width + j) * 4 + 3].position = sf::Vector2f(tilesize_x * j, tilesize_y * i + tilesize_y);

            //texture coordinates
            m_vertices[(i * m_width + j) * 4].texCoords.x = ((int)tilemap[i+1][j+1] % TILES_TEXTURE_WIDTH) * tilesize_x;
            m_vertices[(i * m_width + j) * 4].texCoords.y = ((int)tilemap[i+1][j+1] / TILES_TEXTURE_WIDTH) * tilesize_y;
            m_vertices[(i * m_width + j) * 4 + 1].texCoords.x = ((int)tilemap[i+1][j+1] % TILES_TEXTURE_WIDTH) * tilesize_x + tilesize_x;
            m_vertices[(i * m_width + j) * 4 + 1].texCoords.y = ((int)tilemap[i+1][j+1] / TILES_TEXTURE_WIDTH) * tilesize_y;
            m_vertices[(i * m_width + j) * 4 + 2].texCoords.x = ((int)tilemap[i+1][j+1] % TILES_TEXTURE_WIDTH) * tilesize_x + tilesize_x;
            m_vertices[(i * m_width + j) * 4 + 2].texCoords.y = ((int)tilemap[i+1][j+1] / TILES_TEXTURE_WIDTH) * tilesize_y + tilesize_y;
            m_vertices[(i * m_width + j) * 4 + 3].texCoords.x = ((int)tilemap[i+1][j+1] % TILES_TEXTURE_WIDTH) * tilesize_x;
            m_vertices[(i * m_width + j) * 4 + 3].texCoords.y = ((int)tilemap[i+1][j+1] / TILES_TEXTURE_WIDTH) * tilesize_y + tilesize_y;
        }
        tilesize_x = tilesize_x;
    }

}

void TileMap::setTile(sf::Vector2i coords, Tile tile, sf::Vector2i tilesize) {

    m_vertices[(coords.y * m_width + coords.x) * 4].texCoords.x = ((int)tile % TILES_TEXTURE_WIDTH) * tilesize.x;
    m_vertices[(coords.y * m_width + coords.x) * 4].texCoords.y = ((int)tile / TILES_TEXTURE_WIDTH) * tilesize.y;
    m_vertices[(coords.y * m_width + coords.x) * 4 + 1].texCoords.x = ((int)tile % TILES_TEXTURE_WIDTH) * tilesize.x + tilesize.x;
    m_vertices[(coords.y * m_width + coords.x) * 4 + 1].texCoords.y = ((int)tile / TILES_TEXTURE_WIDTH) * tilesize.y;
    m_vertices[(coords.y * m_width + coords.x) * 4 + 2].texCoords.x = ((int)tile % TILES_TEXTURE_WIDTH) * tilesize.x + tilesize.x;
    m_vertices[(coords.y * m_width + coords.x) * 4 + 2].texCoords.y = ((int)tile / TILES_TEXTURE_WIDTH) * tilesize.y + tilesize.y;
    m_vertices[(coords.y * m_width + coords.x) * 4 + 3].texCoords.x = ((int)tile % TILES_TEXTURE_WIDTH) * tilesize.x;
    m_vertices[(coords.y * m_width + coords.x) * 4 + 3].texCoords.y = ((int)tile / TILES_TEXTURE_WIDTH) * tilesize.x + tilesize.y;
}
