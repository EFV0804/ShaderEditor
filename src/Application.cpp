//
// Created by elise.vidal.
//

#include "Application.h"
#include "Core.h"
#include "UI.h"
#include "VKRenderer.h"

void Application::init(std::string pAppName) {

    appName = pAppName;
    // INIT SUBSYSTEMS
    Core::init();
    UI::Get().init();
    Renderer::Get().init();
    loadedScene.load();
}

void Application::run() {
    while (!glfwWindowShouldClose(UI::Get().window.getWindow())) {
        glfwPollEvents();
        loadedScene.update();
        loadedScene.draw();
    }
}

void Application::close() {
    loadedScene.cleanUp();
    Renderer::Get().close();
}