#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "Agent.hpp"

struct Controller {
  virtual void update(sf::Time timeDelta, Agent *agent) = 0;
};

#endif // CONTROLLER_H_
