//
// Created by elise on 16/09/2022.
//

#include "Window.h"
#include "Application.h"
#include "KeyboardEvents.h"
Window::Window(int pHeight, int pWidth): size(pHeight,pWidth) {

}
Window::~Window(){
    cleanUp();
}
void Window::init(){

    SE_UI_DEBUG("Window initialisation");

    std::string name = Application::Get().appName;
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(size.width, size.height, name.c_str(), nullptr, nullptr);

    if (window == nullptr) {
        SE_UI_ERROR("Window failed to initialise");
    }
    SE_UI_DEBUG("Window initialised successfully");
    isInit = true;


    //Setting GLFW callbacks
    // GLFW callbacks work by passing the window ptr and a function+defition as second parameter (of type GLFW**fun)
    glfwSetKeyCallback(window,[](GLFWwindow* window, int key, int scancode, int action, int mods){

        //Cannot capture this, so we get a pointer that we cast as a Window object.
        // I'm confused but the IDE doesn't scream anymore.
        auto window_ptr = (Window*)glfwGetWindowUserPointer(window);
        switch (action) {
            case GLFW_PRESS:
            {
                KeyPressedEvent e(key);
                window_ptr->eventCallback(e);
                break;
            }

        }
    });
}
void Window::updateSize(){
    glfwGetFramebufferSize(window, &size.width, &size.height);

}
void Window::cleanUp(){
    glfwDestroyWindow(window);
    glfwTerminate();
    SE_UI_INFO("Window destruction and GLFW termination successful");
}

void Window::update() {
    glfwPollEvents();
}
