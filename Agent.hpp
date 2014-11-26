#ifndef AGENT_H_
#define AGENT_H_

#include <memory>
#include <SFML/Graphics.hpp>
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
    enum direction {
        LEFT=0,
        RIGHT,
        DOWN,
        UP
    };

    virtual void update(sf::Time deltaTime) {};

	int animIndex;
    direction currentDir;
	std::unique_ptr<AnimatedSprite> animatedSprite;
    std::vector<std::shared_ptr<Controller>> controllers;
    PhysicalProperties phys;
};

#endif // AGENT_H_
