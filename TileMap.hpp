#ifndef TILE_MAP_H_
#define TILE_MAP_H_

#include <SFML/Graphics.hpp>

class TileMap : public sf::Drawable, public sf::Transformable {
public:

    bool load(const std::string& tileset, sf::Vector2u tile_size, const int* tiles, unsigned int width, unsigned int height)
    {
        // load the tileset texture
        if (!m_tileset.loadFromFile(tileset))
            return false;

        // resize the vertex array to fit the level size
        m_vertices.setPrimitiveType(sf::Quads);
        m_vertices.resize(width * height * 4);

        // populate the vertex array, with one quad per tile
        for (unsigned int i = 0; i < width; ++i)
            for (unsigned int j = 0; j < height; ++j) {
                // get the current tile number
                int tile_number = tiles[i + j * width];

                // find its position in the tileset texture
                int tu = tile_number % (m_tileset.getSize().x / tile_size.x);
                int tv = tile_number / (m_tileset.getSize().x / tile_size.x);

                // get a pointer to the current tile's quad
                sf::Vertex* quad = &m_vertices[(i + j * width) * 4];

                // define its 4 corners
                quad[0].position = sf::Vector2f(i * tile_size.x, j * tile_size.y);
                quad[1].position = sf::Vector2f((i + 1) * tile_size.x, j * tile_size.y);
                quad[2].position = sf::Vector2f((i + 1) * tile_size.x, (j + 1) * tile_size.y);
                quad[3].position = sf::Vector2f(i * tile_size.x, (j + 1) * tile_size.y);

                // define its 4 texture coordinates
                quad[0].texCoords = sf::Vector2f(tu * tile_size.x, tv * tile_size.y);
                quad[1].texCoords = sf::Vector2f((tu + 1) * tile_size.x, tv * tile_size.y);
                quad[2].texCoords = sf::Vector2f((tu + 1) * tile_size.x, (tv + 1) * tile_size.y);
                quad[3].texCoords = sf::Vector2f(tu * tile_size.x, (tv + 1) * tile_size.y);
            }

        return true;
    }

private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // apply the transform
        states.transform *= getTransform();

        // apply the tileset texture
        states.texture = &m_tileset;

        // draw the vertex array
        target.draw(m_vertices, states);
    }

    sf::VertexArray m_vertices;
    sf::Texture m_tileset;
};


#endif // TILE_MAP_H_
