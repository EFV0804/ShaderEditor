//
// Created by elise on 16/09/2022.
//

#include "Window.h"
#include "Renderer.h"

Window::Window(int pHeight, int pWidth): size(pHeight,pWidth) {

}
Window::~Window(){
    cleanUp();
}
void Window::init(){

    SE_UI_DEBUG("Window initialisation");

    std::string name = "The Best Window";
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(size.height, size.width, name.c_str(), nullptr, nullptr);

    if (window == nullptr) {
        SE_UI_ERROR("Window failed to initialise");
    }
    SE_UI_DEBUG("Window initialised successfully");
    isInit = true;
}
void Window::updateSize(){
    glfwGetFramebufferSize(window, &size.width, &size.height);

}
void Window::cleanUp(){
    glfwDestroyWindow(window);
    glfwTerminate();
    SE_UI_INFO("Window destruction and GLFW termination successful");
}