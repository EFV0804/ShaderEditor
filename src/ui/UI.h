//
// Created by elise.vidal.
//

#pragma once
#include "Window.h"

class UI {
public:
    UI() = default;

    ~UI() = default;

    UI(const UI &) = delete;

    UI &operator=(const UI &) = delete;

    static UI &Get() {
        static UI instance;
        return instance;
    }

    void init();
    void close();

    Window window{1080, 1920};

};
