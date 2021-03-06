#include <iostream>
#include <list>
#include <array>
#include <sstream>
#include <cassert>
#include <iterator>
#include <SFML/Graphics.hpp>

#include "TileMap.hpp"
#include "AnimatedSprite.hpp"
#include "Steamy.hpp"
#include "CollisionController.hpp"
#include "SFMLDebugDraw.h"
#include "Pathfinding.hpp"

enum class CollisionType
{
  MID_BLOCK,
  RIGHT_LEDGE,
  LEFT_LEDGE,
  FEET,
  COUNT
};

static const int tileSize = 16;

template <size_t W, size_t H>
struct MapLevel
{
  std::array<int, W * H> level;
  void set(const int(&levelToCopy)[W * H])
  {
    std::copy(std::begin(levelToCopy), std::end(levelToCopy),
              std::begin(level));
  }
  int width() const { return W; }
  int height() const { return H; }
};

int
main()
{
  sf::Vector2i screenDimensions(32 * 16, 16 * 16);

  sf::RenderWindow window(sf::VideoMode(screenDimensions.x, screenDimensions.y),
                          "Steam Beam Machine v0.0.1");
  window.setFramerateLimit(60);

  // define the level with an array of tile indices
  const int ground_layer[] = {
    0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  40,
    41, 42, 0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  80, 81, 82, 0,  0,
    0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,   0,   0,   0,  80, 81, 82, 0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  120, 121, 122, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  40, 41, 42, 0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,   40,  42,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  41, 41, 41, 41,  41,  41,  41, 41, 41, 42, 0,  0,  0,  40, 41, 41,
    41, 41, 81, 81, 41, 41,  41,  41,  41, 41, 41, 41, 41, 41, 41, 41, 81, 81,
    81, 81, 81, 81, 81, 81,  81,  82,  0,  0,  0,  80, 81, 81, 81, 81, 81, 81,
    81, 81, 81, 81, 81, 81,  81,  81,  81, 81, 81, 81, 81, 81, 81, 81, 81, 81,
    81, 81, 81, 82, 0,  0,   0,   80,  81, 81, 81, 81, 81, 81, 81, 81, 81, 81,
    81, 81, 81, 81, 81, 81,  81,  81
  };

  const int sky_layer[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
  };

  const int objects_layer[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };

  // create the tilemap from the level definition
  TileMap mapLayerGround;
  TileMap mapLayerBackground;
  TileMap mapLayerObjects;

  MapLevel<32, 16> mapLevel;
  mapLevel.set(ground_layer);

  const auto tilePath = "tiles.png";

  if (!mapLayerGround.load(tilePath, sf::Vector2u(tileSize, tileSize),
                           ground_layer, 32, tileSize)) {
    return -1;
  }

  if (!mapLayerBackground.load(tilePath, sf::Vector2u(tileSize, tileSize),
                               sky_layer, 32, tileSize)) {
    return -1;
  }

  if (!mapLayerObjects.load(tilePath, sf::Vector2u(tileSize, tileSize),
                            objects_layer, 32, tileSize)) {
    return -1;
  }

  Steamy agent;

  auto collisionController =
    std::shared_ptr<CollisionManager>(new CollisionManager);
  int dest[32 * tileSize];
  int objDest[32 * tileSize];

  std::copy(std::begin(ground_layer), std::end(ground_layer), std::begin(dest));
  std::copy(std::begin(objects_layer), std::end(objects_layer),
            std::begin(objDest));

  collisionController->map = dest;
  collisionController->objMap = &mapLayerObjects;
  collisionController->tileObjArr = objDest;
  collisionController->tileSize = tileSize;
  collisionController->mapWidth = 32;
  collisionController->mapHeight = tileSize;
  collisionController->tileMap = &mapLayerGround;
  collisionController->agent = &agent;
  collisionController->initPhysics(&agent);

  /* Initialize SFML Debug Draw */
  SFMLDebugDraw debugDraw(window);
  collisionController->world->SetDebugDraw(&debugDraw);
  debugDraw.SetFlags(b2Draw::e_shapeBit);

  agent.collisionController = collisionController;

  // Initialize debug text
  std::stringstream sstream;
  sf::Font mainFont;
  sf::Text fpsCounter;
  sf::Text scoreCounter;

  if (!mainFont.loadFromFile("monotype.ttf")) {
    exit(1);
  }

  fpsCounter.setFont(mainFont);
  fpsCounter.setColor(sf::Color::Black);
  fpsCounter.setCharacterSize(tileSize);

  scoreCounter.setFont(mainFont);
  scoreCounter.setColor(sf::Color::Black);
  scoreCounter.setCharacterSize(tileSize);
  scoreCounter.setPosition(window.getView().getSize().x - 100, 0);

  sf::Time fpsTimer;
  bool timerInitialRun = true;

  sf::Clock frameClock;
  float accumulator = 0.0f;
  float dt = 1.0f / 60.0f;
  bool renderDebug = false;

  sf::Vector2i startTile(9, 11);
  sf::Vector2i targetTile(14, 11);

  std::vector<std::vector<sf::Vertex>>& paths = collisionController->paths;
  std::vector<sf::Vertex> jumpPath =
    collisionController->getJumpPath(sf::Vector2i(9, 11), sf::Vector2i(14, 12));

  while (window.isOpen()) {
    sf::Event event;

    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape)
          window.close();
        if (event.key.code == sf::Keyboard::D)
          renderDebug = !renderDebug;
      }
    }

    sf::Time frameTime = frameClock.restart();

    // Semi-fixed timestep
    {
      static const sf::Time physicsSeconds = sf::seconds(0.05f);

      if (frameTime > physicsSeconds) {
        frameTime = physicsSeconds;
      }

      accumulator += frameTime.asSeconds();

      if (accumulator >= dt) {
        while (accumulator >= dt) {
          agent.fixedUpdate(sf::seconds(dt));
          accumulator -= dt;
        }
        accumulator = 0.0f;
      }

      agent.update(sf::seconds(dt));
    }

    // Update FPS
    {
      fpsTimer = fpsTimer + frameTime;
      if (fpsTimer > sf::seconds(0.5f) || timerInitialRun) {
        sstream.precision(0);
        sstream << std::fixed << "FPS: " << 1.f / frameTime.asSeconds();
        fpsCounter.setString(sstream.str());
        sstream.str("");
        fpsTimer = sf::seconds(0);
        timerInitialRun = false;
      }
    }

    // Display Score
    {
      sstream.precision(0);
      sstream << std::fixed << "Score: " << collisionController->getPoints();
      scoreCounter.setString(sstream.str());
      sstream.str("");
    }

    window.clear();
    window.draw(mapLayerBackground);
    window.draw(mapLayerGround);
    window.draw(mapLayerObjects);
    window.draw(*agent.animatedSprite);

    if (renderDebug) {
      collisionController->draw();
    }

    if (renderDebug) {
      window.draw(fpsCounter);
      for (auto line : paths) {
        window.draw(&line[0], line.size(), sf::Lines);
      }
      window.draw(&jumpPath[0], jumpPath.size(), sf::Lines);
    }

    window.draw(scoreCounter);
    window.display();
  }

  return 0;
}
