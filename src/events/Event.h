//
// Created by elise.vidal.
//
#include <iostream>
#ifndef SHADEREDITOR_EVENT_H
#define SHADEREDITOR_EVENT_H

enum class EventType{
    None = 0,
    WindowClose, WindowResize, WindowFocus, WindowNoFocus,
    KeyPressed, KeyReleased,
    MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
};

class Event{
public:
    virtual ~Event() = default;

    bool isHandled = false;

    virtual EventType getEventType() const = 0;

private:
    EventType type = EventType::None;
};

class EventDispatcher{
public:
    EventDispatcher() = default;

    //Runs the given event function if it matches the event type
    //Takes in an event class T and a function lamba F
    template<typename T, typename F>
    bool dispatch(const F& func, Event& e){
        if(e.getEventType() == T::getStaticType()){
            func(static_cast<T&>(e));
            return true;
        }
        return false;
    }
};
#endif //SHADEREDITOR_EVENT_H
