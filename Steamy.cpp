#include <cmath>
#include <iostream>
#include <vector>
#include "Steamy.hpp"
#include "Controller.hpp"
#include "KeyboardController.hpp"

template <typename T> int sign(T val) { return (T(0) < val) - (val < T(0)); }

Steamy::Steamy() {
  texture.loadFromFile("agent.png");

  Animation &walkingAnimationRight = animations[anim_type::WALK_RIGHT];
  walkingAnimationRight.setSpriteSheet(texture);
  walkingAnimationRight.addFrame(sf::IntRect(0, 0, 15, 20));
  walkingAnimationRight.addFrame(sf::IntRect(15, 0, 15, 20));
  walkingAnimationRight.addFrame(sf::IntRect(30, 0, 15, 20));
  walkingAnimationRight.addFrame(sf::IntRect(0, 20, 15, 20));

  Animation &walkingAnimationLeft = animations[anim_type::WALK_LEFT];
  walkingAnimationLeft.setSpriteSheet(texture);
  walkingAnimationLeft.addFrame(sf::IntRect(15, 20, 15, 20));
  walkingAnimationLeft.addFrame(sf::IntRect(30, 20, 15, 20));
  walkingAnimationLeft.addFrame(sf::IntRect(45, 20, 15, 20));
  walkingAnimationLeft.addFrame(sf::IntRect(0, 40, 15, 20));

  Animation &slideAnimationLeft = animations[anim_type::SLIDE_LEFT];
  slideAnimationLeft.setSpriteSheet(texture);
  slideAnimationLeft.addFrame(sf::IntRect(15, 40, 15, 20));

  Animation slideAnimationRight = animations[anim_type::SLIDE_RIGHT];
  slideAnimationRight.setSpriteSheet(texture);
  slideAnimationRight.addFrame(sf::IntRect(30, 40, 15, 20));

  animIndex = anim_type::WALK_RIGHT;
  currentDir = direction::RIGHT;

  // Mario - 15 frames per 0.5s
  // Animation - 4 frames, 0.5s / (15/4) = 0.13
  animatedSprite = std::unique_ptr<AnimatedSprite>(
      new AnimatedSprite(sf::seconds(0.13f), true, false));
  animatedSprite->setPosition(sf::Vector2f(0, 0));

  inputController = std::shared_ptr<Controller>(new KeyboardController);

  phys.isOnGround = false;
  phys.terminalVelocity = 120.0f;
  phys.jumpAcceleration = 0.0f;
  phys.landAcceleration = 50.0f;
  phys.skyAcceleration = 20.0f;

  animatedSprite->play(animations[animIndex]);
}

void Steamy::fixedUpdate(sf::Time timeDelta) {
  animatedSprite->play(animations[animIndex]);

  collisionController->update(timeDelta, this);

  for (auto cmd : eventManager.currentEvent->commands) {
    if (cmd == command_e::RESTART) {
      eventManager.events.pop_back();
      eventManager.startReplay();
    }
    if (cmd == command_e::STOP) {
      animatedSprite->stop();
    }
  }

  if (currentDir == direction::LEFT) {
    animIndex = anim_type::WALK_LEFT;
  } else if (currentDir == direction::RIGHT) {
    animIndex = anim_type::WALK_RIGHT;
  }

  animatedSprite->update(timeDelta);
}

void Steamy::update(sf::Time timeDelta) {
  inputController->update(timeDelta, this);
}
