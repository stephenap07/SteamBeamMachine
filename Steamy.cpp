#include <cmath>
#include <iostream>
#include <vector>
#include "Steamy.hpp"
#include "Controller.hpp"

template <typename T> int sign(T val) {
        return (T(0) < val) - (val < T(0));
}

struct KeyboardController : Controller {

    KeyboardController() :isFixedTimeStep(true), playRecording(false)
    {
        sf::Time timer;
    }

	virtual void update(sf::Time timeDelta, Agent *agent)
	{
        timer += timeDelta;
        PhysicalProperties *phys = &(agent->phys);
        phys->jumpAcceleration = 0.0f;

        Event event = {
            {},
            timer
        };

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            playRecording = true;
            timer = sf::seconds(0);
            agent->eventManager.startReplay();
            agent->animatedSprite->setPosition(0, 0);
        }

        if (!playRecording) {
            bool noKeyWasPressed = true;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                noKeyWasPressed = false;
                agent->eventManager.pushCommand(event, command_e::WALK_LEFT);
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                noKeyWasPressed = false;
                agent->eventManager.pushCommand(event, command_e::WALK_RIGHT);
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) /*&& phys->isOnGround*/) {
                noKeyWasPressed = false;
                agent->eventManager.pushCommand(event, command_e::JUMP);
            }

            if (noKeyWasPressed) {
                agent->eventManager.pushCommand(event, command_e::STOP);
            } else
                noKeyWasPressed = true;

            agent->eventManager.pushEvent(event);
        } else {
            if (!agent->eventManager.nextEvent(timer)) {
                playRecording = false;
            }
        }
	}

    bool isFixedTimeStep;
    bool playRecording;
    sf::Time timer;
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
    currentDir = direction::RIGHT;

    // Mario - 15 frames per 0.5s
    // Animation - 4 frames, 0.5s / (15/4) = 0.13
    animatedSprite = std::unique_ptr<AnimatedSprite>(new AnimatedSprite(sf::seconds(0.13f), true, false));
    animatedSprite->setPosition(sf::Vector2f(0, 0));

    auto controller = std::shared_ptr<Controller>(new KeyboardController);
    controllers.push_back(controller);

    phys.isOnGround = false;
    phys.terminalVelocity = 120.0f;
    phys.jumpAcceleration = 0.0f;
    phys.landAcceleration = 50.0f;
    phys.skyAcceleration = 20.0f;

    animatedSprite->play(animations[animIndex]);
}

void Steamy::fixedUpdate(sf::Time timeDelta)
{
    animatedSprite->play(animations[animIndex]);

    for (auto controller : controllers) {
        if (controller->isFixedTimeStep)
            controller->update(timeDelta, this);
    }

    if (currentDir == direction::LEFT) {
        animIndex = anim_type::WALK_LEFT;
    } else if (currentDir == direction::RIGHT) {
        animIndex = anim_type::WALK_RIGHT;
    }

    animatedSprite->update(timeDelta);
}

void Steamy::update(sf::Time timeDelta)
{
    for (auto controller : controllers) {
        if (!controller->isFixedTimeStep)
            controller->update(timeDelta, this);
    }
}
