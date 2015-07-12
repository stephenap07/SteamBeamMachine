#ifndef TILE_MAP_H_
#define TILE_MAP_H_

#include <SFML/Graphics.hpp>

class TileMap : public sf::Drawable, public sf::Transformable {
public:
  TileMap() : m_tiles(nullptr) {}

  virtual ~TileMap() {
    if (m_tiles)
      delete[] m_tiles;
  }

  bool load(const std::string &tileset, sf::Vector2u tileSize, const int *tiles,
            unsigned int width, unsigned int height) {
    // load the tileset texture
    if (!m_tileset.loadFromFile(tileset))
      return false;

    initTiles(tileSize, tiles, width, height);

    return true;
  }

  void initTiles(sf::Vector2u tileSize, const int *tiles, unsigned int width,
                 unsigned int height) {
    if (m_tiles)
      delete[] m_tiles;
    m_tiles = new int[width * height];

    for (size_t i = 0; i < width * height; i++) {
      m_tiles[i] = tiles[i];
    }

    m_width = width;
    m_height = height;

    // resize the vertex array to fit the level size
    m_vertices.setPrimitiveType(sf::Quads);
    m_vertices.resize(width * height * 4);

    // populate the vertex array, with one quad per tile
    for (unsigned int i = 0; i < width; ++i)
      for (unsigned int j = 0; j < height; ++j) {
        // get the current tile number
        int tile_number = tiles[i + j * width];

        // find its position in the tileset texture
        int tu = tile_number % (m_tileset.getSize().x / tileSize.x);
        int tv = tile_number / (m_tileset.getSize().x / tileSize.x);

        // get a pointer to the current tile's quad
        sf::Vertex *quad = &m_vertices[(i + j * width) * 4];

        // define its 4 corners
        quad[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
        quad[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
        quad[2].position =
            sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);
        quad[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);

        // define its 4 texture coordinates
        quad[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
        quad[1].texCoords =
            sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
        quad[2].texCoords =
            sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
        quad[3].texCoords =
            sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
      }
  }

  int getWidth() const { return m_width; }

  int getHeight() const { return m_height; }

  int getTileSize() const { return m_tileSize; }

  int isBlocked(int x, int y) const {
    int index = y * m_width + x;
    if (index >= 0 && index < (m_width * m_height))
      return m_tiles[index] > 0;
    else
      return true;
  }

  const int *getTiles() const { return m_tiles; }

private:
  virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const {
    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    states.texture = &m_tileset;

    // draw the vertex array
    target.draw(m_vertices, states);
  }

  sf::VertexArray m_vertices;
  sf::Texture m_tileset;
  int *m_tiles;
  int m_width;
  int m_height;
  int m_tileSize;
};

#endif // TILE_MAP_H_
