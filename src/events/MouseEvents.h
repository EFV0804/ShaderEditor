//
// Created by elise.vidal.
//

#ifndef SHADEREDITOR_MOUSEEVENTS_H
#define SHADEREDITOR_MOUSEEVENTS_H

#include "Event.h"
class MouseMovedEvent : public Event
{
public:
    MouseMovedEvent(const float x, const float y)
            : mouseX(x), mouseY(y) {}

    EventType getEventType() const override {return type;}
    static EventType getStaticType() {return EventType::MouseButtonPressed;}

    float getX() const { return mouseX; }
    float getY() const { return mouseY; }

private:
    float mouseX, mouseY;
    EventType type = EventType::MouseButtonPressed;
};

#endif //SHADEREDITOR_MOUSEEVENTS_H
