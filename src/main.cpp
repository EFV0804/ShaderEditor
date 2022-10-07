#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "Renderer.h"
#include <string>
#include "Scene.h"

int main() {

    Renderer renderer;
    if (renderer.init() == EXIT_FAILURE) return EXIT_FAILURE;
    Scene sceneExample{};
    sceneExample.load(&renderer);


    while (!glfwWindowShouldClose(renderer.window))
    {
        glfwPollEvents();
        sceneExample.draw(&renderer);
    }

    renderer.cleanUp();

    return 0;
}