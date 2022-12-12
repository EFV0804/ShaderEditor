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

    float getX() const { return mouseX; }
    float getY() const { return mouseY; }

    EVENT_CLASS_TYPE(MouseMoved)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

private:
    float mouseX, mouseY;
};

#endif //SHADEREDITOR_MOUSEEVENTS_H
