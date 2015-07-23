#include "Event.h"

EventManager::EventManager() : currentEvent(nullptr), m_currentEventIndex(0) {
  Event ev{sf::seconds(0), {command_e::STOP}};
  pushEvent(ev);
}

void EventManager::pushCommand(Event &event, command_e command) {
  if (m_lastCommand != command)
    event.commands.push_back(command);
  m_lastCommand = command;
}

void EventManager::pushEvent(const Event &event) {
  if (event.commands.size()) {
    events.push_back(event);
    currentEvent = &events[events.size() - 1];
    m_currentEventIndex = events.size() - 1;
  }
}

void EventManager::startReplay() {
  Event event = {sf::seconds(0), {}};

  pushCommand(event, command_e::STOP);
  currentEvent = &events[0];
  m_currentEventIndex = 0;
}

bool EventManager::nextEvent(const sf::Time &timer) {
  Event *nextEvent = nullptr;
  if (m_currentEventIndex < static_cast<int>(events.size()) - 1) {
    nextEvent = &events[m_currentEventIndex + 1];
    if (nextEvent->eventTime <= timer) {
      m_currentEventIndex = m_currentEventIndex + 1;
      currentEvent = nextEvent;
    }
  }
  if (m_currentEventIndex == static_cast<int>(events.size()) - 1) {
    return false;
  } else {
    return true;
  }
}
