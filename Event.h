#pragma once

#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

enum class command_e
{
  WALK_RIGHT,
  WALK_LEFT,
  JUMP,
  STOP,
  RESTART
};

struct Event
{
  sf::Time eventTime;
  std::vector<command_e> commands;
};

class EventManager
{
public:
  EventManager();

  void pushCommand(Event& event, command_e command);

  void pushEvent(const Event& event);

  bool nextEvent(const sf::Time& timer);

  void startReplay();

  Event* currentEvent;

  std::vector<Event> events;

private:
  std::size_t m_currentEventIndex;

  command_e m_lastCommand;
};
