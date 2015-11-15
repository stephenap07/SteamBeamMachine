#include <iostream>
#include "CollisionController.hpp"

template <typename T>
b2Vec2
sfVecToB2Vec(sf::Vector2<T> vector)
{
  return b2Vec2(vector.x / sfdd::SCALE, vector.y / sfdd::SCALE);
}

CollisionManager::CollisionManager()
  : mapX(0)
  , mapY(0)
  , points(0)
{
  gravity.Set(0.0f, 40.0f);
  world = std::unique_ptr<b2World>(new b2World(gravity));
  world->SetContactListener(&listener);
}

void
CollisionManager::update(sf::Time timeDelta, Agent* agent)
{
  sf::FloatRect rect = agent->animatedSprite->getLocalBounds();
  b2Vec2 vel = agentBody->GetLinearVelocity();
  float desiredVel = 0;

  for (auto cmd : agent->eventManager.currentEvent->commands) {
    if (cmd == command_e::WALK_LEFT) {
      agent->currentDir = Agent::direction::LEFT;
      desiredVel = -5.0f;
    }

    if (cmd == command_e::WALK_RIGHT) {
      agent->currentDir = Agent::direction::RIGHT;
      desiredVel = 5.0f;
    }

    if (cmd == command_e::STOP) {
      desiredVel = 0.0f;
    } else {
      updatePaths();
    }

    if (cmd == command_e::JUMP && agent->phys.isOnGround) {
      b2Vec2 vel = agentBody->GetLinearVelocity();
      vel.y = -10; // upwards - don't change x velocity
      agentBody->SetLinearVelocity(vel);
    }

    if (cmd == command_e::RESTART) {
      agentBody->SetTransform(b2Vec2(rect.width / 2.0f / sfdd::SCALE,
                                     rect.height / 2.0f / sfdd::SCALE),
                              0);
      agentBody->SetLinearVelocity(b2Vec2(0, 0));
      agentBody->SetAwake(true);
    }
  }

  float velChange = desiredVel - vel.x;
  float impulse = agentBody->GetMass() * velChange;
  agentBody->ApplyLinearImpulse(b2Vec2(impulse, 0), agentBody->GetWorldCenter(),
                                true);

  const int velocityIterations = 6;
  const int positionIterations = 2;
  world->Step(1.0f / 60.0f, velocityIterations, positionIterations);

  // Set the position of our agent
  b2Vec2 bodyPos = agentBody->GetPosition();
  agent->animatedSprite->setPosition(
    bodyPos.x * sfdd::SCALE - rect.width / 2.0f,
    bodyPos.y * sfdd::SCALE - rect.height / 2.0f);
  agent->phys.isOnGround = listener.isOnGround();

  for (auto body : bodiesToDelete) {
    world->DestroyBody(body);
    points++;
  }
  bodiesToDelete.clear();
}

/**
 * Create box2d bodies and insert into world
 */
void
CollisionManager::initPhysics(Agent* agent)
{
  pathFinder.setMap(tileMap);
  std::vector<bool> tilesVisited(mapWidth * mapHeight, false);
  const float tileScale = tileSize / sfdd::SCALE;

  for (int in = 0; in < mapWidth; in++) {
    sf::Vector2f firstVec(-1.0f, -1.0f);

    bool visited = false;
    for (int x = 0; x < mapWidth; x++) {
      for (int y = 0; y < mapHeight; y++) {
        if (!isPassable(x, y) && !tilesVisited[y * mapWidth + x]) {
          firstVec.x = x;
          firstVec.y = y;
          tilesVisited[y * mapWidth + x] = true;
          visited = true;
          break;
        }
      }
      if (visited)
        break;
    }

    if (!visited)
      continue;

    tilesVisited[firstVec.y * mapWidth + firstVec.x] = true;
    sf::Vector2f secondVec = firstVec;
    while ((secondVec.x + 1) < mapWidth &&
           !isPassable(secondVec.x + 1, secondVec.y)) {
      tilesVisited[secondVec.y * mapWidth + secondVec.x] = true;
      secondVec.x++;
    }
    tilesVisited[secondVec.y * mapWidth + secondVec.x] = true;

    sf::FloatRect rect(firstVec, (secondVec - firstVec + sf::Vector2f(1, 1)));

    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(
      (rect.left + rect.width) * tileScale - (rect.width * tileScale / 2.0f),
      (rect.top + rect.height) * tileScale - (rect.height * tileScale / 2.0f));
    b2Body* groundBody = world->CreateBody(&groundBodyDef);

    b2PolygonShape groundBox;
    groundBox.SetAsBox(rect.width * tileScale / 2.0f,
                       rect.height * tileScale / 2.0f);
    groundBody->CreateFixture(&groundBox, 0.0f);
  }

  // Create agent body
  sf::FloatRect bounds = agent->animatedSprite->getGlobalBounds();

  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;
  bodyDef.position.Set((bounds.left + bounds.width / 2.0f) / sfdd::SCALE,
                       (bounds.top + bounds.height / 2.0f) / sfdd::SCALE);
  bodyDef.fixedRotation = true;
  agentBody = world->CreateBody(&bodyDef);

  b2PolygonShape dynamicBox;
  dynamicBox.SetAsBox(bounds.width / 2.0f / sfdd::SCALE,
                      bounds.height / 2.0f / sfdd::SCALE);

  b2FixtureDef fixtureDef;
  fixtureDef.shape = &dynamicBox;
  fixtureDef.density = 1.0f;
  fixtureDef.friction = 0.3f;
  agentBody->CreateFixture(&fixtureDef);

  b2PolygonShape rightSideBox;
  rightSideBox.SetAsBox(
    bounds.width / 8.0f / sfdd::SCALE, bounds.height / 2.0f / sfdd::SCALE,
    b2Vec2(
      bounds.width / 2.0f / sfdd::SCALE - bounds.width / 8.0f / sfdd::SCALE, 0),
    0);

  b2FixtureDef rightSideDef;
  rightSideDef.shape = &rightSideBox;
  rightSideDef.density = 1.0f;
  rightSideDef.friction = 0.0f;
  agentBody->CreateFixture(&rightSideDef);

  b2PolygonShape leftSideBox;
  leftSideBox.SetAsBox(bounds.width / 8.0f / sfdd::SCALE,
                       bounds.height / 2.0f / sfdd::SCALE,
                       b2Vec2(-bounds.width / 2.0f / sfdd::SCALE, 0), 0);

  b2FixtureDef leftSideDef;
  leftSideDef.shape = &leftSideBox;
  leftSideDef.density = 1.0f;
  leftSideDef.friction = 0.0f;
  agentBody->CreateFixture(&leftSideDef);

  b2PolygonShape polygonShape;

  // fixture definition
  b2FixtureDef myFixtureDef;
  myFixtureDef.shape = &polygonShape;
  myFixtureDef.density = 1;

  // add foot sensor fixture
  polygonShape.SetAsBox(bounds.width / 4.0f / sfdd::SCALE,
                        bounds.height / 16.0f / sfdd::SCALE,
                        b2Vec2(0, bounds.height / 2.0f / sfdd::SCALE), 0);
  myFixtureDef.isSensor = true;
  b2Fixture* footSensorFixture = agentBody->CreateFixture(&myFixtureDef);
  footSensorFixture->SetUserData((void*)collisionType_e::FOOT);

  // add sensors for coins

  b2PolygonShape coinBox;
  coinBox.SetAsBox(tileSize / 3.0f / sfdd::SCALE,
                   tileSize / 3.0f / sfdd::SCALE);

  for (int y = 0; y < mapHeight; y++) {
    for (int x = 0; x < mapWidth; x++) {
      if (tileObjArr[y * mapWidth + x] == collisionType_e::COIN) {
        b2BodyDef coinDef;
        b2Body* coinBody;
        coinDef.type = b2_staticBody;
        coinDef.position.Set((x * tileSize + tileSize / 2.0f) / sfdd::SCALE,
                             (y * tileSize + tileSize / 2.0f) / sfdd::SCALE);
        coinBody = world->CreateBody(&coinDef);

        b2FixtureDef coinFixtureDef;
        coinFixtureDef.shape = &coinBox;
        coinFixtureDef.density = 1;

        coinFixtureDef.isSensor = true;
        b2Fixture* coinSensorFixture = coinBody->CreateFixture(&coinFixtureDef);
        coinSensorFixture->SetUserData((void*)collisionType_e::COIN);

        listener.tileArr = tileObjArr;
        listener.tileMap = objMap;
        listener.tileSize = tileSize;
        listener.mapWidth = mapWidth;
        listener.mapHeight = mapHeight;
        listener.controller = this;
      }
    }
  }
}

void
CollisionManager::draw()
{
  world->DrawDebugData();
}

bool
CollisionManager::isPassable(int tileX, int tileY)
{
  return map[tileY * mapWidth + tileX] == 0;
}

void
CollisionManager::scheduleDelete(b2Body* body)
{
  bodiesToDelete.insert(body);
}

int
CollisionManager::getPoints() const
{
  return points;
}

std::vector<sf::Vertex>
CollisionManager::getJumpPath(sf::Vector2i start, sf::Vector2i target)
{
  sf::Vector2f startPos((start.x) * tileSize + tileSize / 2.0f,
                        (start.y + 1) * tileSize + tileSize / 2.0f);
  sf::Vector2f targetPos((target.x) * tileSize + tileSize / 2.0f,
                         (target.y + 1) * tileSize + tileSize / 2.0f);

  const float gravity = 40.0f;
  const float vY = -10.0f;
  const float vX = 5.0f;

  float vertDist = (targetPos - startPos).y / sfdd::SCALE;
  float horzDist = (targetPos - startPos).x / sfdd::SCALE;
  float timeToTop = -vY / gravity;
  float jumpHeight = -(vY * timeToTop + gravity * timeToTop * timeToTop / 2.0f);

  std::vector<sf::Vertex> arcPoints;
  if (jumpHeight >= vertDist) {
    float fallDistance = jumpHeight - vertDist;
    float timeToFall = std::sqrt(2.0f * fallDistance / gravity);
    float jumpTime = timeToTop + timeToFall;
    float jumpLength = vX * jumpTime;

    bool arcSetFirst = false;
    float startX = startPos.x;
    float startY = startPos.y;

    for (float t = 0; t < jumpTime; t += 1.0f / 60.0f) {
      float x = vX * t;
      float y = vY * t + gravity * t * t / 2.0f;

      sf::Vertex vertex(
        sf::Vector2f(startX + x * sfdd::SCALE, startY + y * sfdd::SCALE),
        sf::Color::Red);
      if (arcSetFirst)
        arcPoints.push_back(vertex);

      arcPoints.push_back(vertex);
      arcSetFirst = true;
    }
  }

  return arcPoints;
}

bool
CollisionManager::canJumpBetween(sf::Vector2i start, sf::Vector2i target)
{
  sf::Vector2f startPos((start.x) * tileSize + tileSize / 2.0f,
                        (start.y + 1) * tileSize + tileSize / 2.0f);
  sf::Vector2f targetPos((target.x) * tileSize + tileSize / 2.0f,
                         (target.y + 1) * tileSize + tileSize / 2.0f);

  const float gravity = 40.0f;
  const float vY = -10.0f;
  const float vX = 5.0f;

  const sf::Vector2f distanceVec{targetPos - startPos};
  float vertDist = distanceVec.y / sfdd::SCALE;
  float horzDist = distanceVec.x / sfdd::SCALE;
  float timeToTop = -vY / gravity;
  float jumpHeight = -(vY * timeToTop + gravity * timeToTop * timeToTop / 2.0f);

  std::vector<sf::Vertex> arcPoints;
  if (jumpHeight >= vertDist) {
    float fallDistance = jumpHeight - vertDist;
    float timeToFall = std::sqrt(2.0f * fallDistance / gravity);
    float jumpTime = timeToTop + timeToFall;
    float jumpLength = vX * jumpTime;

    return true;
  }
  return false;
}

static bool
isValidPoint(sf::Vector2i point, int mapWidth, int mapHeight)
{
  if (point.x < 0 || point.y < 0 || point.x > mapWidth || point.y > mapHeight) {
    return false;
  }
}

std::vector<sf::Vertex>
CollisionManager::getSearchPath(sf::Vector2i start, sf::Vector2i target)
{
  if (!isValidPoint(start, mapWidth, mapHeight) ||
      !isValidPoint(target, mapWidth, mapHeight)) {
    return {};
  }

  std::vector<Node> path =
    pathFinder.getPath(Node(start.x, start.y), Node(target.x, target.y));

  std::vector<sf::Vertex> linePoints;
  bool setFirst = false;
  for (auto node : path) {
    sf::Vector2f point((float)node.x * tileSize + tileSize / 2.0f,
                       (float)node.y * tileSize + tileSize / 2.0f);
    sf::Vertex vertex(point, sf::Color::Blue);
    linePoints.push_back(sf::Vertex(vertex));
    if (setFirst)
      linePoints.push_back(vertex);
    else
      setFirst = true;
  }

  return linePoints;
}

void
CollisionManager::updatePaths()
{
  paths.clear();

  for (int y = 0; y < mapHeight; y++) {
    for (int x = 0; x < mapWidth; x++) {
      if (tileObjArr[y * mapWidth + x]) {
        sf::Vector2f pos = agent->animatedSprite->getPosition();
        sf::Vector2i start(pos.x / 16, pos.y / 16);
        sf::Vector2i end(x, y);

        std::vector<sf::Vertex> linePoints = getSearchPath(start, end);
        paths.push_back(linePoints);
      }
    }
  }

  // std::vector<sf::Vertex> arcPoints = getJumpPath(startTile, targetTile);
  // paths.push_back(arcPoints);
}
