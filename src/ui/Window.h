//
// Created by elise on 16/09/2022.
//

#ifndef SHADEREDITOR_WINDOW_H
#define SHADEREDITOR_WINDOW_H
#define GLFW_INCLUDE_VULKAN

#include <string>
#include <GLFW/glfw3.h>
#include "Logger.h"

class Renderer;

struct Size{
    Size(int height, int width):height(height), width(width){};
    int height;
    int width;
};

class Window {

public:
    Window(int pHeight, int pWidth);
    ~Window() = default;
    Window &operator=(const Window&) = delete;
    Window(const Window&) = delete;

    void init();
    void cleanUp();
    GLFWwindow* getWindow(){return window;}
    const int getHeight()const {return size.height;}
    const int getWidth()const {return size.width;}

private:
    bool isInit = false;
    GLFWwindow* window = nullptr;
    Size size;

    void updateSize();


};


#endif //SHADEREDITOR_WINDOW_H
