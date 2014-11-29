#include "CollisionController.hpp"

CollisionManager::CollisionManager() 
    :isFixedTimeStep(false), mapX(0), mapY(0)
{
    gravity.Set(0.0f, -10.0f);
    world = std::unique_ptr<b2World>(new b2World(gravity));
}

void CollisionManager::update(sf::Time timeDelta, Agent *agent)
{
    for (auto cmd : agent->eventManager.currentEvent->commands) {
        if(cmd == command_e::WALK_LEFT) {
            agent->currentDir = Agent::direction::LEFT;
            //phys->targetSpeed.x = -phys->terminalVelocity;
        }

        if (cmd == command_e::WALK_RIGHT) {
            agent->currentDir = Agent::direction::RIGHT;
            //phys->targetSpeed.x = phys->terminalVelocity;
        }

        if (cmd == command_e::JUMP) {
            //phys->targetSpeed.y = -450.0f;
            //phys->jumpAcceleration = 1000.0f;
            //phys->isOnGround = false;
        }

        if (cmd == command_e::STOP) {
            agent->animatedSprite->stop();
            //phys->targetSpeed = sf::Vector2f(0.0f, 0.0f);
        }
    }
} 

void CollisionManager::initPhysics()
{
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0.0f,-10.0f);
    b2Body *groundBody = world->CreateBody(&groundBodyDef);

    b2PolygonShape groundBox;
    groundBox.SetAsBox(50.0f, 10.0f);
}

void CollisionManager::draw()
{
    world->DrawDebugData();
}

bool CollisionManager::isPassable(int tileX, int tileY)
{
    return map[tileY*mapWidth + tileX] == 0;
}
