//
// Created by elise on 16/09/2022.
//

#ifndef SHADEREDITOR_WINDOW_H
#define SHADEREDITOR_WINDOW_H
#define GLFW_INCLUDE_VULKAN

#include <string>
#include <GLFW/glfw3.h>
#include "Logger.h"
#include "Event.h"
#include <functional>

struct Size{
    Size(int height, int width):height(height), width(width){};
    int height;
    int width;
};

class Window {

public:
    Window(int pHeight, int pWidth);
    ~Window();
    Window &operator=(const Window&) = delete;
    Window(const Window&) = delete;

    void init();
    void update();
    void cleanUp();
    GLFWwindow* getWindow(){return window;}
    const int getHeight()const {return size.height;}
    const int getWidth()const {return size.width;}
    void setCallbackFunction(std::function<void(Event& e)> pEventCallback){eventCallback = pEventCallback;}
    void setWindowShouldClose(bool value);


private:
    bool isInit = false;
    GLFWwindow* window = nullptr;
    Size size;
    std::function<void(Event& e)> eventCallback;
    void updateSize();


};


#endif //SHADEREDITOR_WINDOW_H
