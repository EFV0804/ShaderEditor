#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include "Renderer.h"
#include "Logger.h"
#include <string>
#include "Material.h"
#include "Mesh.h"
#include "Renderable.h"
#include "Scene.h"


/*!
 *\brief Example function to demonstrate how to use the logging functions.
 */
void loggingExample(){
// ***************--- LOGGING EXAMPLE ---************//
    int a = 5;
    SE_RENDERER_INFO("Test info message VAR{0}", a);
    SE_RENDERER_WARN("Test warning message");
    SE_RENDERER_ERROR("Test error message");
    SE_RENDERER_DEBUG("Test debug message");
    SE_RENDERER_CRITICAL("Test critical message");
    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, a>5, "a is inferior to 5");
    SE_INTERNAL_ASSERT_NO_MSG(_RENDERER_, a>5);
// **************************************************//
}
int main() {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    Logger::init();
//    loggingExample();

    Renderer& renderer = Renderer::Get();

    if (renderer.init() == EXIT_FAILURE) return EXIT_FAILURE;

    Scene sceneExample{};
    sceneExample.load();

    while (!glfwWindowShouldClose(renderer.window.getWindow())) {
        glfwPollEvents();
        sceneExample.update();
        sceneExample.draw();
    }

    sceneExample.cleanUp();
    renderer.cleanUp();

    return 0;
}