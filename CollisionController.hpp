#ifndef COLLISION_CONTROLLER_H_
#define COLLISION_CONTROLLER_H_

#include <memory>
#include <vector>
#include <set>
#include <cstdint>
#include <cmath>
#include <inttypes.h>
#include <Box2D/Box2D.h>
#include "Controller.hpp"
#include "TileMap.hpp"
#include "SFMLDebugDraw.h"
#include "Pathfinding.hpp"
#include "Agent.hpp"


struct CollisionManager : public Controller {
    enum collisionType_e {
        COIN = 3,
        FOOT
    };

    struct FootContactListener : public b2ContactListener
    {
        FootContactListener() :numFootContacts(0) {}

        void BeginContact(b2Contact* contact) {
            //check if fixture A was the foot sensor
            int dataA = (long)reinterpret_cast<long*>(contact->GetFixtureA()->GetUserData());
            int dataB = (long)reinterpret_cast<long*>(contact->GetFixtureB()->GetUserData());

            if (dataA != collisionType_e::COIN && dataB != collisionType_e::COIN) {
                if (dataA == collisionType_e::FOOT || dataB == collisionType_e::FOOT) {
                    numFootContacts++;
                }
            } else {
                b2Body *body;
                if (dataA == collisionType_e::COIN) {
                    body = contact->GetFixtureA()->GetBody();
                } else {
                    body = contact->GetFixtureB()->GetBody();
                }

                b2Vec2 position = body->GetPosition();
                int tileX = (position.x * sfdd::SCALE) / tileSize;
                int tileY = (position.y * sfdd::SCALE) / tileSize;
                tileArr[tileY*mapWidth + tileX] = 0;
                tileMap->initTiles(sf::Vector2u(tileSize, tileSize), tileArr, mapWidth, mapHeight);
                controller->scheduleDelete(body);
            }
        }

        void EndContact(b2Contact* contact) {
            int dataA = (long)reinterpret_cast<int*>(contact->GetFixtureA()->GetUserData());
            int dataB = (long)reinterpret_cast<int*>(contact->GetFixtureB()->GetUserData());

            if (dataA != collisionType_e::COIN && dataB != collisionType_e::COIN) {
                if (dataA == collisionType_e::FOOT || dataB == collisionType_e::FOOT) {
                    numFootContacts--;
                }
            } else {
            }
        }

        bool isOnGround() {
            return numFootContacts > 0;
        }

        int numFootContacts;
        int *tileArr;
        TileMap *tileMap;
        int tileSize;
        int mapWidth;
        int mapHeight;
        CollisionManager *controller;
    };

    bool isFixedTimeStep;
    int *map;
    TileMap *objMap;
    TileMap *tileMap;
    int *tileObjArr;
    int tileSize;
    int mapWidth;
    int mapHeight;
    int mapX;
    int mapY;
    int points;
    FootContactListener listener;
    Agent *agent;

    b2Vec2 gravity;
    std::unique_ptr<b2World> world;
    b2Body *agentBody;
    std::vector<b2BodyDef> bodyDefs;
    std::set<b2Body*> bodiesToDelete;
    Pathfinder pathFinder;

    std::vector<std::vector<sf::Vertex>> paths;

    CollisionManager();
    virtual void update(sf::Time timeDelta, Agent *agent);
    void initPhysics();
    void initPhysics(Agent *agent);
    void draw();
    bool isPassable(int tileX, int tileY);
    void scheduleDelete(b2Body *body);
    int getPoints() const;
    std::vector<sf::Vertex> getJumpPath(sf::Vector2i start, sf::Vector2i target);
    std::vector<sf::Vertex> getSearchPath(sf::Vector2i start, sf::Vector2i target);
    bool canJumpBetween(sf::Vector2i start, sf::Vector2i target);
    void updatePaths();
};


#endif
