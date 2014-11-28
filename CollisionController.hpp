#ifndef COLLISION_CONTROLLER_H_
#define COLLISION_CONTROLLER_H_

#include "Controller.hpp"


struct CollisionManager : public Controller {
    int *map;
    int tileSize;
    int mapWidth;
    int mapHeight;
    int mapX;
    int mapY;

    CollisionManager() :mapX(0), mapY(0)
    {
    }

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
        //agent->phys.isOnGround = false;
        agent->phys.isOnGround = false;
        for (int y = 0; y < mapHeight; y++) {
            for (int x = 0; x < mapWidth; x++) {
                if (!isNotPassable(x, y))
                    continue;

                sf::FloatRect tileRect(x*tileSize, y*tileSize, tileSize, tileSize);
                sf::FloatRect agentRect = agent->animatedSprite->getGlobalBounds();

                const float halfTileWidth = tileRect.width / 2.0f;
                const float halfAgentWidth = agentRect.width / 2.0f;
                const float halfTileHeight = tileRect.height / 2.0f;
                const float halfAgentHeight = agentRect.height / 2.0f;
                float distanceX = std::fabs(agent->animatedSprite->getPosition().x - tileRect.left);
                float distanceY = std::fabs(agent->animatedSprite->getPosition().y - tileRect.top);

                const bool movedLeft = agent->phys.currentSpeed.x < 0.0f;
                const bool movedRight = agent->phys.currentSpeed.x > 0.0f;
                const bool movedUp = agent->phys.currentSpeed.y < 0.0f;
                const bool movedDown = agent->phys.currentSpeed.y > 0.0f;

                if (agent->animatedSprite->getGlobalBounds().intersects(tileRect)) {
                    if (movedLeft || movedRight) {
                        if (!((movedUp || movedDown) && distanceY > distanceX)) {
                            if (agent->animatedSprite->getPosition().x >= tileRect.left) {
                                agent->animatedSprite->setPosition(tileRect.left + halfTileWidth + halfAgentWidth, agent->animatedSprite->getPosition().y);
                            }
                            else {
                                agent->animatedSprite->setPosition(tileRect.left - halfTileWidth - halfAgentWidth, agent->animatedSprite->getPosition().y);
                            }
                        }
                    }
                }

                if (agent->animatedSprite->getGlobalBounds().intersects(tileRect)) {
                    if (movedUp || movedDown) {
                        if (!((movedLeft || movedRight) && distanceX > distanceY)) {
                            if (agent->animatedSprite->getPosition().y >= tileRect.top) {
                                agent->animatedSprite->setPosition(agent->animatedSprite->getPosition().x, tileRect.top + halfTileHeight + halfAgentHeight);
                            } else {
                                agent->animatedSprite->setPosition(agent->animatedSprite->getPosition().x, tileRect.top - halfTileHeight - halfAgentHeight);
                                agent->phys.isOnGround = true;
                                agent->phys.currentSpeed.y = 0.0f;
                            }
                        }
                    }
                }
            }
        }
    }

    bool isPassable(int tileX, int tileY)
    {
        return map[tileY*mapWidth + tileX] == 0;
    }
};


#endif
