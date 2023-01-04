//
// Created by elise.vidal.
//

#pragma once
#include "Scene.h"
#include <string>
#include "Event.h"
#include "EditorController.h"
#include "EditorView.h"

class Application {
public:
    Application() = default;

    ~Application() = default;

    Application(const Application &) = delete;

    Application &operator=(const Application &) = delete;

    void init(std::string pAppName);
    void run();
    void close();
    void onEvent(Event& e);

    static Application &Get() {
        static Application instance;
        return instance;
    }

    Scene loadedScene{};
    std::string appName;
    EditorController editorCtrl;
    EditorView editorView;
};
