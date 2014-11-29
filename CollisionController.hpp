#ifndef COLLISION_CONTROLLER_H_
#define COLLISION_CONTROLLER_H_

#include <memory>
#include <vector>
#include <Box2D/Box2D.h>
#include "Controller.hpp"


struct CollisionManager : public Controller {
    bool isFixedTimeStep;
    int *map;
    int tileSize;
    int mapWidth;
    int mapHeight;
    int mapX;
    int mapY;

    b2Vec2 gravity;
    std::unique_ptr<b2World> world;
    b2Body *agentBody;
    std::vector<b2BodyDef> bodyDefs;

    CollisionManager();
    virtual void update(sf::Time timeDelta, Agent *agent);
    void initPhysics();
    void initPhysics(Agent *agent);
    void draw();
    bool isPassable(int tileX, int tileY);
};


#endif
