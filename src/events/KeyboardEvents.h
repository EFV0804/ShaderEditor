//
// Created by elise.vidal.
//

#ifndef SHADEREDITOR_KEYBOARDEVENTS_H
#define SHADEREDITOR_KEYBOARDEVENTS_H
#include "Event.h"
class KeyPressedEvent : public Event
{
public:
    KeyPressedEvent(int pKeycode)
            : keycode(pKeycode) {}

    int getKeycode() const { return keycode; }
    EventType getEventType() const override {return type;}
    static EventType getStaticType() {return EventType::KeyPressed;}

private:
    int keycode;
    EventType type = EventType::KeyPressed;
};
#endif //SHADEREDITOR_KEYBOARDEVENTS_H
