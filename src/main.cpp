#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include "Renderer.h"
#include "Logger.h"
#include <string>
#include "Material.h"
#include "Mesh.h"
#include "Renderable.h"
//#include "Scene.h"


/*!
 *\brief Example function to demonstrate how to use the logging functions.
 */
void loggingExample(){
// ***************--- LOGGING EXAMPLE ---************//
//    int a = 5;
//    SD_RENDERER_INFO("Test info message VAR{0}", a);
//    SD_RENDERER_WARN("Test warning message");
//    SD_RENDERER_ERROR("Test error message");
//    SD_RENDERER_DEBUG("Test debug message");
//    SD_RENDERER_CRITICAL("Test critical message");
//    SD_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, a>5, "a is inferior to 5");
//    SD_INTERNAL_ASSERT_NO_MSG(_RENDERER_, a>5);
// **************************************************//
}
int main() {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    Logger::init();
    loggingExample();

    Renderer renderer;
    if (renderer.init() == EXIT_FAILURE) return EXIT_FAILURE;

    // ---------MATERIAL TEST ZONE ------------
    ShaderInfo vertInfo{"../../assets/shaders/compiled/shader.vert.spv",
                        vk::ShaderStageFlagBits::eVertex};
    ShaderInfo fragInfo{"../../assets/shaders/compiled/shader.frag.spv",
                        vk::ShaderStageFlagBits::eFragment};

    std::vector<ShaderInfo> shadersInfo;
    shadersInfo.reserve(2);
    shadersInfo.push_back(vertInfo);
    shadersInfo.push_back(fragInfo);

    Material triangleMat{&renderer, shadersInfo, "triangleMat" };

    Mesh m{};
    std::vector<Renderable> renderables;
    renderables.emplace_back(&m, &triangleMat);
    renderer.loadMeshes(&renderables);
    //----------------------------------------
//    Scene sceneExample{};
//    sceneExample.load(&renderer);

    while (!glfwWindowShouldClose(renderer.window)) {
        glfwPollEvents();
//        sceneExample.draw(&renderer);
        renderer.draw(&renderables);
    }

    renderer.cleanUp();

    return 0;
}