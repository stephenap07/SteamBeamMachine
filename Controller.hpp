#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "Agent.hpp"

struct Controller {
	virtual void update(sf::Time timeDelta, Agent *agent) {};
    bool isFixedTimeStep;
};

#endif // CONTROLLER_H_
