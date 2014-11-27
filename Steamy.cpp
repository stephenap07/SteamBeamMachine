#include <cmath>
#include <iostream>
#include <vector>
#include "Steamy.hpp"
#include "Controller.hpp"

template <typename T> int sign(T val) {
        return (T(0) < val) - (val < T(0));
}

enum class command_e {
    WALK_RIGHT,
    WALK_LEFT,
    JUMP,
    STOP
};


struct Event {
    std::vector<command_e> commands;
    sf::Time eventTime;
};


class EventManager {

public:

    EventManager() :currentEvent(nullptr), m_currentEventIndex(0)
    {}

    void pushCommand(Event &event, command_e command)
    {
        if (m_lastCommand != command)
            event.commands.push_back(command);
        m_lastCommand = command;
    }

    void pushEvent(const Event &event)
    {
        if (event.commands.size()) {
            events.push_back(event);
            currentEvent = &events[events.size() - 1];
            m_currentEventIndex = events.size() - 1;
        }
    }

    void startReplay()
    {
        Event event = {
            {},
            sf::seconds(0) 
        };

        pushCommand(event, command_e::STOP);
        currentEvent = &events[0];
        m_currentEventIndex = 0;
    }

    bool nextEvent(const sf::Time &timer)
    {
        Event *nextEvent = nullptr;
        if (m_currentEventIndex < static_cast<int>(events.size()) - 1) {
            nextEvent = &events[m_currentEventIndex + 1];
            if (nextEvent->eventTime <= timer) {
                m_currentEventIndex = m_currentEventIndex + 1;
                currentEvent = nextEvent;
            }
        }
        if (m_currentEventIndex == (static_cast<int>(events.size()) - 1)) {
            return false;
        } else {
            return true;
        }
    }

    Event *currentEvent;
    std::vector<Event> events;
private:
    int m_currentEventIndex;
    command_e m_lastCommand;
};

struct KeyboardController : Controller {

    KeyboardController() :playRecording(false)
    {
        sf::Time timer;
        Event ev {{command_e::STOP}, timer};
        eventManager.pushEvent(ev);
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
            eventManager.startReplay();
            agent->animatedSprite->setPosition(0, 0);
        }

        if (!playRecording) {
            bool noKeyWasPressed = true;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                noKeyWasPressed = false;
                eventManager.pushCommand(event, command_e::WALK_LEFT);
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                noKeyWasPressed = false;
                eventManager.pushCommand(event, command_e::WALK_RIGHT);
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && phys->isOnGround) {
                noKeyWasPressed = false;
                eventManager.pushCommand(event, command_e::JUMP);
            }

            if (noKeyWasPressed) {
                eventManager.pushCommand(event, command_e::STOP);
            } else
                noKeyWasPressed = true;

            eventManager.pushEvent(event);
        } else {
            if (!eventManager.nextEvent(timer)) {
                playRecording = false;
            }
        }

        doPhysics(timeDelta, agent);
	}

    void doPhysics(sf::Time timeDelta, Agent *agent)
    {
        PhysicalProperties *phys = &(agent->phys);

        for (auto cmd : eventManager.currentEvent->commands) {
            if(cmd == command_e::WALK_LEFT) {
                agent->currentDir = Agent::direction::LEFT;
                phys->targetSpeed.x = -phys->terminalVelocity;
            }

            if (cmd == command_e::WALK_RIGHT) {
                agent->currentDir = Agent::direction::RIGHT;
                phys->targetSpeed.x = phys->terminalVelocity;
            }

            if (cmd == command_e::JUMP) {
                phys->targetSpeed.y = -350.0f;
                phys->jumpAcceleration = 1000.0f;
                phys->isOnGround = false;
            }
            
            if (cmd == command_e::STOP) {
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
    EventManager eventManager;
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
