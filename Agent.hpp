#ifndef AGENT_H_
#define AGENT_H_

#include <memory>
#include <SFML/Graphics.hpp>
#include "Event.h"
#include "AnimatedSprite.hpp"

struct PhysicalProperties {
  bool isOnGround;
  float terminalSpeed;
  float terminalVelocity;
  float jumpAcceleration;
  float landAcceleration;
  float skyAcceleration;
  sf::Vector2f currentSpeed;
  sf::Vector2f targetSpeed;
};

struct Controller;

struct Agent {
  enum direction { LEFT = 0, RIGHT, DOWN, UP };

  virtual void fixedUpdate(sf::Time deltaTime){};
  virtual void update(sf::Time deltaTime){};

  int animIndex;
  direction currentDir;
  std::unique_ptr<AnimatedSprite> animatedSprite;
  std::shared_ptr<Controller> collisionController;
  std::shared_ptr<Controller> inputController;
  PhysicalProperties phys;
  EventManager eventManager;
};

#endif // AGENT_H_
