#include <cmath>
#include <iostream>
#include <vector>
#include "Steamy.hpp"
#include "Controller.hpp"

template <typename T> int sign(T val) {
        return (T(0) < val) - (val < T(0));
}

enum class COMMAND {
    WALK_RIGHT,
    WALK_LEFT,
    JUMP,
    STOP
};


struct Event {
    std::vector<COMMAND> commands;
    sf::Time eventTime;
};


struct KeyboardController : Controller {
    KeyboardController() :playRecording(false), currentEventIndex(0) {
        sf::Time timer;
        Event ev {{COMMAND::STOP}, timer};
        events.push_back(ev);
    }

    void pushCommand(Event &event, COMMAND command)
    {
        if (lastCommand != command)
            event.commands.push_back(command);
        lastCommand = command;
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
            pushCommand(event, COMMAND::STOP);
            currentEvent = &events[0];
            currentEventIndex = 0;
            agent->animatedSprite->setPosition(0, 0);
            timer = sf::seconds(0);
        }

        if (!playRecording) {
            bool noKeyWasPressed = true;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                noKeyWasPressed = false;
                pushCommand(event, COMMAND::WALK_LEFT);
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                noKeyWasPressed = false;
                pushCommand(event, COMMAND::WALK_RIGHT);
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && phys->isOnGround) {
                noKeyWasPressed = false;
                pushCommand(event, COMMAND::JUMP);
            }

            if (noKeyWasPressed) {
                pushCommand(event, COMMAND::STOP);
            } else
                noKeyWasPressed = true;

            if (event.commands.size())
                events.push_back(event);
            
            currentEvent = &events[events.size() - 1];
            currentEventIndex = events.size() - 1;
        } else {
            Event *nextEvent = nullptr;
            if (currentEventIndex < events.size() - 1) {
                nextEvent = &events[currentEventIndex + 1];
                if (nextEvent->eventTime <= timer) {
                    currentEventIndex = currentEventIndex + 1;
                    currentEvent = nextEvent;
                }
            }
        }

        doPhysics(timeDelta, agent);
	}

    void doPhysics(sf::Time timeDelta, Agent *agent)
    {
        PhysicalProperties *phys = &(agent->phys);

        for (auto cmd : currentEvent->commands) {
            if(cmd == COMMAND::WALK_LEFT) {
                agent->currentDir = Agent::direction::LEFT;
                phys->targetSpeed.x = -phys->terminalVelocity;
            }

            if (cmd == COMMAND::WALK_RIGHT) {
                agent->currentDir = Agent::direction::RIGHT;
                phys->targetSpeed.x = phys->terminalVelocity;
            }

            if (cmd == COMMAND::JUMP) {
                phys->targetSpeed.y = -350.0f;
                phys->jumpAcceleration = 1000.0f;
                phys->isOnGround = false;
            }
            
            if (cmd == COMMAND::STOP) {
                agent->animatedSprite->stop();
                phys->targetSpeed = sf::Vector2f(0.0f, 0.0f);
            }
        }

        sf::Vector2f currentPosition = agent->animatedSprite->getPosition();

        if (std::fabs(phys->currentSpeed.x) < 0.0f)
            phys->currentSpeed.x = 0;
        if (std::fabs(phys->currentSpeed.y) < 0.0f)
            phys->currentSpeed.y = 0;

        float acceleration = phys->landAcceleration;

        if (!phys->isOnGround) {
            acceleration = phys->skyAcceleration;
        }

        // gravity
        phys->currentSpeed.y += 30.0f;
        float terminalGravity = 500.0f;

        if (phys->currentSpeed.y > terminalGravity) {
            phys->currentSpeed.y = terminalGravity;
        }

        sf::Vector2f direction = sf::Vector2f(sign(phys->targetSpeed.x - phys->currentSpeed.x), sign(phys->targetSpeed.y - phys->currentSpeed.y));
        phys->currentSpeed.x += acceleration * direction.x; 
        phys->currentSpeed.y += phys->jumpAcceleration * direction.y;

        if (sign(phys->targetSpeed.x - phys->currentSpeed.x) != direction.x)
            phys->currentSpeed.x = phys->targetSpeed.x;
        if (sign(phys->targetSpeed.y - phys->currentSpeed.y) != direction.y)
            phys->currentSpeed.y = phys->targetSpeed.y;

        agent->animatedSprite->move(phys->currentSpeed * timeDelta.asSeconds());
        currentPosition = agent->animatedSprite->getPosition();

        if (currentPosition.y > (16 * 16)-(4 * 16)) {
            agent->animatedSprite->setPosition(currentPosition.x, (16 * 16)-(4 * 16));
            phys->isOnGround = true;
        } else {
            phys->isOnGround = false;
        }
    }

    bool playRecording;
    int currentEventIndex;
    Event *currentEvent;
    COMMAND lastCommand;
    std::vector<Event> events;

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
