//
// Created by elise.vidal.
//

#ifndef SHADEREDITOR_EVENT_H
#define SHADEREDITOR_EVENT_H
#define BIT(x) (1 << x)

enum class EventType{
    None = 0,
    WindowClose, WindowResize, WindowFocus, WindowNoFocus,
    KeyPressed, KeyReleased,
    MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
};

enum EventCategory{
    None = 0,
    EventCategoryInput = BIT(1),
    EventCategoryMouse = BIT(2),
    EventCategoryMouseButton = BIT(3),
    EventCategoryKeyboard = BIT(4),
};

#define EVENT_CLASS_TYPE(type) static EventType getStaticType() { return EventType::type; }\
								virtual EventType getEventType() const override { return getStaticType(); }\

#define EVENT_CLASS_CATEGORY(category) virtual int getCategoryFlagBits() const override { return category; }

class Event{
public:
    virtual ~Event() = default;

    bool isHandled = false;

    virtual EventType getEventType() const = 0;
    virtual int getCategoryFlagBits() const = 0;

    bool isInCategory(EventCategory cat){
        return getCategoryFlagBits() & cat;
    }
};

class EventDispatcher{
public:
    EventDispatcher(Event& pEvent): event(pEvent){}

    //Runs the given event function if it matches the event type
    //Takes in an event type T and a function "type" F
    template<typename T, typename F>
    bool Dispatch(const F& func)
    {
        //Check if the event being dispatched matches the given Event Type
        if (event.getEventType() == T::GetStaticType())
        {
            //bitwise OR assignment |=
            //isHandled = isHandled or function return
            event.isHandled |= func(static_cast<T&>(event));
            return true;
        }
        return false;
    }

private:
    Event& event;
};
#endif //SHADEREDITOR_EVENT_H
