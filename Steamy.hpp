#ifndef STEAMY_H_
#define STEAMY_H_

#include "Agent.hpp"

struct Steamy : public Agent {
	Steamy();
	virtual void update(sf::Time timeDelta);
    virtual void fixedUpdate(sf::Time deltaTime);

	enum anim_type {
		WALK_RIGHT = 0,
		WALK_LEFT,
		SLIDE_RIGHT,
		SLIDE_LEFT
	};

	Animation animations[4];
	sf::Texture texture;
};

#endif
