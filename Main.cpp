#include <SFML/Graphics.hpp>

struct Node {
   sf::Sprite sprite;
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(640, 640), "SFML works!");
    sf::CircleShape shape(100.0f);
    shape.setFillColor(sf::Color::Green);

    sf::Texture tex;
    tex.loadFromFile("smw_ground.png");

    Node n;
    n.sprite.setTexture(tex);
    n.sprite.setTextureRect(sf::Rect<int>(569, 99, 16, 16));

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(n.sprite);
        window.display();
    }

    return 0;
}
