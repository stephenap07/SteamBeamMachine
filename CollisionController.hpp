#ifndef COLLISION_CONTROLLER_H_
#define COLLISION_CONTROLLER_H_

#include <memory>
#include <vector>
#include <Box2D/Box2D.h>
#include "Controller.hpp"


struct CollisionManager : public Controller {
    struct FootContactListener : public b2ContactListener
    {
        FootContactListener() :numFootContacts(0) {}

        void BeginContact(b2Contact* contact) {
            //check if fixture A was the foot sensor
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if ( (int)fixtureUserData == 3 )
                numFootContacts++;
            //check if fixture B was the foot sensor
            fixtureUserData = contact->GetFixtureB()->GetUserData();
            if ( (int)fixtureUserData == 3 )
                numFootContacts++;
        }

        void EndContact(b2Contact* contact) {
            //check if fixture A was the foot sensor
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if ( (int)fixtureUserData == 3 )
                numFootContacts--;
            //check if fixture B was the foot sensor
            fixtureUserData = contact->GetFixtureB()->GetUserData();
            if ( (int)fixtureUserData == 3 )
                numFootContacts--;
        }

        bool isOnGround() {
            return numFootContacts > 0;
        }

        int numFootContacts;
    };

    bool isFixedTimeStep;
    int *map;
    int tileSize;
    int mapWidth;
    int mapHeight;
    int mapX;
    int mapY;
    FootContactListener footListener;

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
