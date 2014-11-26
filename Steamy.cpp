#include <cmath>
#include <iostream>
#include "Steamy.hpp"
#include "Controller.hpp"

template <typename T> int sign(T val) {
        return (T(0) < val) - (val < T(0));
}

struct PhysicalProperties {
    float terminalSpeed;
    sf::Vector2f currentSpeed;
    sf::Vector2f targetSpeed;
};

struct KeyboardController : Controller {
    KeyboardController() :isOnGround(false) {}

	virtual void update(sf::Time timeDelta, Agent *agent)
	{
        if (agent == NULL) {
            return;
        }

        bool noKeyWasPressed = true;
        float terminalVelocity = 120.0f;
        sf::Vector2f targetSpeed(0.0f, 200.0f);

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            agent->currentDir = Agent::direction::LEFT;
            targetSpeed.x = -terminalVelocity;
            noKeyWasPressed = false;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            agent->currentDir = Agent::direction::RIGHT;
            targetSpeed.x = terminalVelocity;
            noKeyWasPressed = false;
        }

        sf::Vector2f currentPosition = agent->animatedSprite->getPosition();

        float jumpAcceleration = 0.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && isOnGround) {
            targetSpeed.y = -350.0f;
            jumpAcceleration = 1000.0f;
        }

        if (std::fabs(currentSpeed.x) < 0.0f)
            currentSpeed.x = 0;
        if (std::fabs(currentSpeed.y) < 0.0f)
            currentSpeed.y = 0;

        float landAcceleration = 100.0f;

        sf::Vector2f direction = sf::Vector2f(sign(targetSpeed.x - currentSpeed.x), sign(targetSpeed.y - currentSpeed.y));
        currentSpeed.x += landAcceleration * direction.x; 
        currentSpeed.y += jumpAcceleration * direction.y;

        // gravity
        currentSpeed.y += 30.0f;

        if (sign(targetSpeed.x - currentSpeed.x) != direction.x)
            currentSpeed.x = targetSpeed.x;
        if (sign(targetSpeed.y - currentSpeed.y) != direction.y)
            currentSpeed.y = targetSpeed.y;

        agent->animatedSprite->move(currentSpeed * timeDelta.asSeconds());
        currentPosition = agent->animatedSprite->getPosition();
        if (currentPosition.y > (16 * 16)-(4 * 16)) {
            agent->animatedSprite->setPosition(currentPosition.x, (16 * 16)-(4 * 16));
            isOnGround = true;
        } else {
            isOnGround = false;
        }

        if (noKeyWasPressed)
            agent->animatedSprite->stop();
        else
            noKeyWasPressed = true;
	}

    sf::Vector2f currentSpeed;
    bool isOnGround;
};


Steamy::Steamy()
{
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

    // Mario - 15 frames per 0.5s
    // Animation - 4 frames, 0.5s / (15/4) = 0.13
    animatedSprite = std::unique_ptr<AnimatedSprite>(new AnimatedSprite(sf::seconds(0.13f), true, false));
    animatedSprite->setPosition(sf::Vector2f(0, 0));

    auto controller = std::shared_ptr<Controller>(new KeyboardController);
    controllers.push_back(controller);
}

void Steamy::update(sf::Time timeDelta)
{
    for (auto controller : controllers) {
        controller->update(timeDelta, this);
    }

    if (currentDir == direction::LEFT) {
        animIndex = anim_type::WALK_LEFT;
    } else if (currentDir == direction::RIGHT) {
        animIndex = anim_type::WALK_RIGHT;
    }

    animatedSprite->play(animations[animIndex]);
    animatedSprite->update(timeDelta);
}
