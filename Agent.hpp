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

enum class command_e {
    WALK_RIGHT,
    WALK_LEFT,
    JUMP,
    STOP,
    RESTART
};


struct Event {
    std::vector<command_e> commands;
    sf::Time eventTime;
};


class EventManager {

public:

    EventManager() :currentEvent(nullptr), m_currentEventIndex(0)
    {
        Event ev {{command_e::STOP}, sf::seconds(0)};
        pushEvent(ev);
    }

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
        if (m_currentEventIndex == static_cast<int>(events.size()) - 1) {
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
    sf::Clock frameClock;
};


struct Agent {
    enum direction {
        LEFT=0,
        RIGHT,
        DOWN,
        UP
    };

    virtual void fixedUpdate(sf::Time deltaTime) {};
    virtual void update(sf::Time deltaTime) {};

	int animIndex;
    direction currentDir;
	std::unique_ptr<AnimatedSprite> animatedSprite;
    std::shared_ptr<Controller> collisionController;
    std::shared_ptr<Controller> inputController;
    PhysicalProperties phys;
    EventManager eventManager;
};

#endif // AGENT_H_
