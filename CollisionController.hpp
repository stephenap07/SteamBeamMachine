#ifndef COLLISION_CONTROLLER_H_
#define COLLISION_CONTROLLER_H_

#include "Controller.hpp"


struct CollisionManager : public Controller {
    int *map;
    int tileSize;
    int mapWidth;
    int mapHeight;

    bool collisionHorzDown(int x, int y, int width, int & tileY)
    {
        int tilePixels = x - (x % tileSize);
        int testEnd = x + width;
        tileY = y/tileSize;
        int tileX = tilePixels/tileSize;

        while (tilePixels <= testEnd) {
            if (map[tileY*mapWidth + tileX])
                return true;
            tileX++;
            tilePixels += tileSize;
        }
        return false;
    }

    bool collisionHorzUp(int x, int y, int width, int & tileY)
    {
        int tilePixels = x-(x % tileSize);
        int testEnd = x + width;
        tileY = y/tileSize;
        int tileX = tilePixels/tileSize;

        while (tilePixels <= testEnd) {
            if (map[tileY*mapWidth + tileX])
                return true;
            tileX++;
            tilePixels += tileSize;
        }
        return false;
    }

    bool collisionVer(int x, int y, int height, int & tileX)
    {
        int tilePixels = y - ( y % tileSize );
        int testEnd = y + height;
        tileX = x/tileSize;
        int tileY = tilePixels/tileSize;

        while (tilePixels < testEnd) {
            if(map[tileY*mapWidth + tileX])
                return true;
            tileY++;
            tilePixels += tileSize;
        }
        return false;
    }

    // Must take global bounds
    static bool rectCollision(sf::FloatRect a, sf::FloatRect b)
    {
        return a.intersects(b);
    }

    virtual void update(sf::Time timeDelta, Agent *agent)
    {
        float x = agent->animatedSprite->getPosition().x;
        float y = agent->animatedSprite->getPosition().y;
        int tileCoord;

        int width = 15;
        int height = 20;

        if (agent->phys.currentSpeed.x > 0.0f) {
            if (collisionVer(x + width, y, height, tileCoord)) {
                agent->animatedSprite->setPosition(sf::Vector2f(tileCoord*tileSize - width, y));
            }
        } else if (agent->phys.currentSpeed.x < 0.0f) {
            if (collisionVer(x, y, height, tileCoord)) {
                agent->animatedSprite->setPosition( sf::Vector2f((tileCoord + 1)*tileSize, y));
            }
        }

        x = agent->animatedSprite->getPosition().x;
        y = agent->animatedSprite->getPosition().y;

        if (agent->phys.currentSpeed.y < 0) {
            if (collisionHorzUp(x, y, width, tileCoord)) {
                agent->animatedSprite->setPosition(sf::Vector2f(x, (tileCoord + 1) * tileSize));
                agent->phys.currentSpeed.y = 0.0f;
            }
        } else {
            if (collisionHorzDown(x, y + height, width, tileCoord)) { //on the ground
                agent->animatedSprite->setPosition(sf::Vector2f(x, tileCoord * tileSize - height));
                agent->phys.currentSpeed.y = 0;
                agent->phys.isOnGround = true;
            }
        }
    }
};


#endif
