//
// Created by elise.vidal.
//

#include "Application.h"
#include "Core.h"
#include "UI.h"
#include "Timer.h"
#include "EditorController.h"
#include "EditorView.h"


void Application::init(std::string pAppName) {

    appName = pAppName;
    // INIT SUBSYSTEMS

    Core::init();
    UI::Get().init();
    std::function<void(Event& e)> callback = [this](auto && PH1) { onEvent(std::forward<decltype(PH1)>(PH1)); };
    UI::Get().window.setCallbackFunction(callback);
    Renderer::Get().init();
    loadedScene.load();
    editorView.init();
}

void Application::run() {
    while (!glfwWindowShouldClose(UI::Get().window.getWindow())) {
        Timer timer;
        float dt = timer.computeDeltaTime();

        UI::Get().window.update();
        loadedScene.update(dt);
        editorView.render();
        loadedScene.draw();
    }
}

void Application::close() {
    loadedScene.cleanUp();
    Renderer::Get().close();

}

void Application::onEvent(Event &e) {
    editorCtrl.onEvent(e);
}
