#include <iostream>
#include "CollisionController.hpp"
#include "SFMLDebugDraw.h"

template<typename T >
b2Vec2 sfVecToB2Vec(sf::Vector2<T> vector)
{
    return b2Vec2(vector.x / sfdd::SCALE, vector.y / sfdd::SCALE);
}

CollisionManager::CollisionManager() 
    :isFixedTimeStep(false), mapX(0), mapY(0)
{
    gravity.Set(0.0f, 10.0f);
    world = std::unique_ptr<b2World>(new b2World(gravity));
}

void CollisionManager::update(sf::Time timeDelta, Agent *agent)
{
    for (auto cmd : agent->eventManager.currentEvent->commands) {
        if(cmd == command_e::WALK_LEFT) {
            agent->currentDir = Agent::direction::LEFT;
            agentBody->ApplyForce(b2Vec2(-5, 0), agentBody->GetWorldCenter(), true);
            //phys->targetSpeed.x = -phys->terminalVelocity;
        }

        if (cmd == command_e::WALK_RIGHT) {
            agent->currentDir = Agent::direction::RIGHT;
            agentBody->ApplyForce(b2Vec2(5, 0), agentBody->GetWorldCenter(), true);
            //phys->targetSpeed.x = phys->terminalVelocity;
        }

        if (cmd == command_e::JUMP) {
            //phys->targetSpeed.y = -450.0f;
            //phys->jumpAcceleration = 1000.0f;
            //phys->isOnGround = false;
            agentBody->ApplyForce(b2Vec2(0, -11), agentBody->GetWorldCenter(), true);
        }

        if (cmd == command_e::STOP) {
            agent->animatedSprite->stop();
            //phys->targetSpeed = sf::Vector2f(0.0f, 0.0f);
        }
    }

    const int velocityIterations = 6;
    const int positionIterations = 2;
    world->Step(1.0f / 60.0f, velocityIterations, positionIterations);

    // Set the position of our agent
    b2Vec2 bodyPos = agentBody->GetPosition();
    sf::FloatRect rect = agent->animatedSprite->getLocalBounds();
    agent->animatedSprite->setPosition(bodyPos.x * sfdd::SCALE - rect.width/2.0f, bodyPos.y * sfdd::SCALE - rect.height/2.0f);
} 

void CollisionManager::initPhysics(Agent *agent)
{
    std::vector<bool> tilesVisited(mapWidth*mapHeight, false);
    const float tileScale = tileSize / sfdd::SCALE;

    for (int in = 0; in < mapWidth; in++) {
        sf::Vector2f firstVec(-1.0f, -1.0f);

        bool visited = false;
        for (int x = 0; x < mapWidth; x++) {
            for (int y = 0; y < mapHeight; y++) {
                if (!isPassable(x, y) && !tilesVisited[y*mapWidth + x]) {
                    firstVec.x = x;
                    firstVec.y = y;
                    tilesVisited[y*mapWidth + x] = true;
                    visited = true;
                    break;
                }
            }
            if (visited)
                break;
        }

        if (!visited)
            continue;

        tilesVisited[firstVec.y*mapWidth + firstVec.x] = true;
        sf::Vector2f secondVec = firstVec;
        while ((secondVec.x+1) < mapWidth && !isPassable(secondVec.x+1, secondVec.y)) {
            tilesVisited[secondVec.y*mapWidth + secondVec.x] = true;
            secondVec.x++;
        }
        tilesVisited[secondVec.y*mapWidth + secondVec.x] = true;

        sf::FloatRect rect(firstVec, (secondVec-firstVec + sf::Vector2f(1, 1)));

        b2BodyDef groundBodyDef;
        groundBodyDef.position.Set((rect.left + rect.width) * tileScale - (rect.width *tileScale/2.0f), (rect.top + rect.height) * tileScale - (rect.height *tileScale/2.0f));
        b2Body *groundBody = world->CreateBody(&groundBodyDef);

        b2PolygonShape groundBox;
        groundBox.SetAsBox(rect.width * tileScale/2.0f, rect.height * tileScale/2.0f);
        groundBody->CreateFixture(&groundBox, 0.0f);
    }

    // Create agent body
    sf::FloatRect bounds = agent->animatedSprite->getGlobalBounds();
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set((bounds.left + bounds.width / 2.0f) / sfdd::SCALE, (bounds.top + bounds.height / 2.0f) / sfdd::SCALE);
    agentBody = world->CreateBody(&bodyDef);
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(bounds.width / 2.0f / sfdd::SCALE, bounds.height / 2.0f /  sfdd::SCALE);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    agentBody->CreateFixture(&fixtureDef);
}

void CollisionManager::draw()
{
    world->DrawDebugData();
}

bool CollisionManager::isPassable(int tileX, int tileY)
{
    return map[tileY*mapWidth + tileX] == 0;
}
