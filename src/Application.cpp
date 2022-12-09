//
// Created by elise.vidal.
//

#include "Application.h"
#include "Core.h"
#include "UI.h"
#include "VKRenderer.h"
#include "Timer.h"

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
        Timer timer;
        float dt = timer.computeDeltaTime();

        glfwPollEvents();
        loadedScene.update(dt);
        loadedScene.draw();
    }
}

void Application::close() {
    loadedScene.cleanUp();
    Renderer::Get().close();
}