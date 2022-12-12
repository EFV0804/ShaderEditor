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

    EVENT_CLASS_TYPE(KeyPressed)
    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

private:
    int keycode;
};
#endif //SHADEREDITOR_KEYBOARDEVENTS_H
