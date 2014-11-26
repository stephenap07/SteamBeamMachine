#ifndef AGENT_H_
#define AGENT_H_

#include <memory>
#include <SFML/Graphics.hpp>
#include "AnimatedSprite.hpp"


struct Agent {
	enum anim_type {
		WALK_RIGHT = 0,
		WALK_LEFT,
		SLIDE_RIGHT,
		SLIDE_LEFT
	};

	Agent()
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
		faceRight = true;

		// Mario - 15 frames per 0.5s
		// Animation - 4 frames, 0.5s / (15/4) = 0.13
		animatedSprite = std::unique_ptr<AnimatedSprite>(new AnimatedSprite(sf::seconds(0.13f), true, false));
		animatedSprite->setPosition(sf::Vector2f(0, 0));
		speed = 80.0f;
	}

	void update(sf::Time timeDelta)
	{
        animatedSprite->play(animations[animIndex]);
        animatedSprite->update(timeDelta);
	}

	bool faceRight;
	float speed;
	int animIndex;

	std::unique_ptr<AnimatedSprite> animatedSprite;

	Animation animations[4];
	sf::Texture texture;
};


struct KeyboardController {
	KeyboardController()
	{
		noKeyWasPressed = true;
	}

	void update(sf::Time timeDelta)
	{
        sf::Vector2f movement(0.f, 0.f);

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            //if (vec.x > 0.0f) {
                //currentAnimation = &slideAnimationRight;
            //} else {
            agent->animIndex = Agent::anim_type::WALK_LEFT;
            //}

            movement.x -= agent->speed;
            agent->faceRight = false;
            noKeyWasPressed = false;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            //if (vec.x < 0.0f) {
             //   currentAnimation = &slideAnimationLeft;
            //} else {
            agent->animIndex = Agent::anim_type::WALK_RIGHT;
            //}

            movement.x += agent->speed;
            agent->faceRight = true;
            noKeyWasPressed = false;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
            // jump
        }

        agent->animatedSprite->move(movement * timeDelta.asSeconds());

        if (noKeyWasPressed) {
            agent->animatedSprite->stop();
        }
        noKeyWasPressed = true;
	}

	bool noKeyWasPressed;
	Agent *agent;
};

#endif // AGENT_H_
