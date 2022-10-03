#define GLFW_INCLUDE_NONE
#define VMA_IMPLEMENTATION
#include <GLFW/glfw3.h>
#include "Renderer.h"
#include <string>
#include "Scene.h"
#include <iostream>

int main() {

//    std::cout<<"test"<<std::endl;

    Renderer renderer = Renderer();
    if (renderer.init() == EXIT_FAILURE) return EXIT_FAILURE;
    Scene sceneExample = Scene();
    sceneExample.load(&renderer);
    renderer.loadMeshes(&sceneExample.renderables);

    while (!glfwWindowShouldClose(renderer.window))
    {
        glfwPollEvents();
        sceneExample.draw(&renderer);
    }

    renderer.cleanUp();

    return 0;
}