#ifndef KEYBOARD_CONTROLLER_H_
#define KEYBOARD_CONTROLLER_H_

#include "Controller.hpp"

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

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                playRecording = true;
                timer = sf::seconds(0);
                agent->animatedSprite->setPosition(0, 0);
                agent->eventManager.pushCommand(event, command_e::RESTART);
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

#endif
