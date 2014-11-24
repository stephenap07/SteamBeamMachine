#include <iostream>
#include <sstream>
#include <cassert>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#include "TileMap.hpp"
#include "AnimatedSprite.hpp"
#include "SFMLDebugDraw.h"


class Agent {
};

int main()
{
    sf::Vector2i screenDimensions(32 * 16, 16 * 16);

    sf::RenderWindow window(sf::VideoMode(screenDimensions.x, screenDimensions.y), "Steam Beam Machine v0.0.1");
	window.setFramerateLimit(120);

    // define the level with an array of tile indices
    const int ground_layer[] = {
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 40, 42,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        41, 41, 41, 41, 41, 41, 41, 41, 41, 42,  0,  0,  0, 40, 41, 41, 41, 41, 81, 81, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,
        81, 81, 81, 81, 81, 81, 81, 81, 81, 81,  0,  0,  0, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81,
        81, 81, 81, 81, 81, 81, 81, 81, 81, 81,  0,  0,  0, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81,
        81, 81, 81, 81, 81, 81, 81, 81, 81, 81,  0,  0,  0, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81
    };

    const int sky_layer[] = {
        1,   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1
    };

    // create the tilemap from the level definition
    TileMap mapLayerGround;
    TileMap mapLayerBackground;

    if (!mapLayerGround.load("tiles.png", sf::Vector2u(16, 16), ground_layer, 32, 16))
        return -1;
    if (!mapLayerBackground.load("tiles.png", sf::Vector2u(16, 16), sky_layer, 32, 16))
        return -1;

    sf::Texture spriteTexture;
    spriteTexture.loadFromFile("agent.png");

    Animation walkingAnimationRight;
    walkingAnimationRight.setSpriteSheet(spriteTexture);
    walkingAnimationRight.addFrame(sf::IntRect(0, 0, 15, 20));
    walkingAnimationRight.addFrame(sf::IntRect(15, 0, 15, 20));
    walkingAnimationRight.addFrame(sf::IntRect(30, 0, 15, 20));
    walkingAnimationRight.addFrame(sf::IntRect(0, 20, 15, 20));

    Animation walkingAnimationLeft;
    walkingAnimationLeft.setSpriteSheet(spriteTexture);
    walkingAnimationLeft.addFrame(sf::IntRect(15, 20, 15, 20));
    walkingAnimationLeft.addFrame(sf::IntRect(30, 20, 15, 20));
    walkingAnimationLeft.addFrame(sf::IntRect(45, 20, 15, 20));
    walkingAnimationLeft.addFrame(sf::IntRect(0, 40, 15, 20));

    Animation* currentAnimation = &walkingAnimationRight;

    // Mario - 15 frames per 0.5s
    // Animation - 4 frames, 0.5s / (15/4) = 0.13
    AnimatedSprite animatedSprite(sf::seconds(0.13f), true, false);
    animatedSprite.setPosition(sf::Vector2f(0, 0));
    float speed = 80.0f;
    bool noKeyWasPressed = true;

    sf::Clock frameClock;

    //float xPos = (window.getSize().x / 2.f) / sfdd::SCALE;
	float yPos = (window.getSize().y) / sfdd::SCALE - 1.f;


    b2Vec2 gravity(0.0f, 25.0f);
    b2World world(gravity);
	SFMLDebugDraw debugDraw(window);
	world.SetDebugDraw(&debugDraw);
	debugDraw.SetFlags(b2Draw::e_shapeBit); //Only draw shapes

    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0.0f, 0.0f / sfdd::SCALE);
    b2Body *groundBody = world.CreateBody(&groundBodyDef);
    b2PolygonShape groundBox;

	yPos = (window.getSize().y) / sfdd::SCALE - 1.f;
    groundBox.SetAsBox((window.getSize().x) / sfdd::SCALE, 16.0f / sfdd::SCALE, b2Vec2(0.f, yPos), 0.f); 
    groundBody->CreateFixture(&groundBox, 0.0f);

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.fixedRotation = true;
    bodyDef.position.Set(10.0f / sfdd::SCALE, 4.0f / sfdd::SCALE);
    b2Body* body = world.CreateBody(&bodyDef);

    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(12.0f / 2.0f / sfdd::SCALE, 18.0f / 2.0f / sfdd::SCALE);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.1f;

    body->CreateFixture(&fixtureDef);

    //add foot sensor fixture
    dynamicBox.SetAsBox(3.0f / sfdd::SCALE, 2.0f / sfdd::SCALE, b2Vec2(0, 10.0f / sfdd::SCALE), 0);
    fixtureDef.isSensor = true;
    b2Fixture* footSensorFixture = body->CreateFixture(&fixtureDef);
    footSensorFixture->SetUserData( (void*)3 );

    float timeStep = 1.0f / 60.0f;
    int velocityIterations = 6;
    int positionIterations = 2;

	// Initialize debug text
    std::stringstream sstream;
	sf::Text fpsCounter;

	sf::Font mainFont;

	if(!mainFont.loadFromFile("SPFont.ttf")) {
        exit(1);
    }

	fpsCounter.setFont(mainFont);
	fpsCounter.setColor(sf::Color::Black);
    fpsCounter.setCharacterSize(16);

    // run the main loop
    while (window.isOpen())
    {
        // handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
            }
        }

        sf::Time frameTime = frameClock.restart();

        // if a key was pressed set the correct animation and move correctly
        sf::Vector2f movement(0.f, 0.f);

        //b2Vec2 vec = body->GetVelocity();

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            currentAnimation = &walkingAnimationLeft;
            movement.x -= speed;
            noKeyWasPressed = false;
            body->ApplyLinearImpulse(b2Vec2(-2.0f / sfdd::SCALE, 0), body->GetWorldCenter(), true);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            currentAnimation = &walkingAnimationRight;
            movement.x += speed;
            noKeyWasPressed = false;
            body->ApplyLinearImpulse(b2Vec2(2.0f / sfdd::SCALE, 0), body->GetWorldCenter(), true);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {

            body->ApplyLinearImpulse(b2Vec2(0, -0.85f * 25.0f), body->GetWorldCenter(), true);
        }

        animatedSprite.play(*currentAnimation);
        //animatedSprite.move(movement * frameTime.asSeconds());

        // if no key was pressed stop the animation
        if (noKeyWasPressed) {
            animatedSprite.stop();
        }
        noKeyWasPressed = true;

        // Box2D updating
        world.Step(timeStep, velocityIterations, positionIterations);
        animatedSprite.setPosition(body->GetPosition().x * sfdd::SCALE - 12.0f/2.0f, body->GetPosition().y * sfdd::SCALE - 18.0f/2.0f);

        // Agent updating
        animatedSprite.update(frameTime);

        window.clear();

        // Map Rendering
        window.draw(mapLayerBackground);
        window.draw(mapLayerGround);

        // Agent rendering
        window.draw(animatedSprite);
		world.DrawDebugData();

        // Display FPS
		{
			sstream.precision(0);
			sstream << std::fixed << "FPS: " << 1.f / frameTime.asSeconds();
			fpsCounter.setString(sstream.str());
			window.draw(fpsCounter);
			sstream.str("");
		}

        window.display();
    }

    return 0;
}
