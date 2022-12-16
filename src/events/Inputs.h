//
// Created by elise.vidal.
//

#pragma once
#include "glfw/glfw3.h"

class Inputs {
public:
    Inputs() = default;

    ~Inputs() = default;

    Inputs(const Inputs &) = delete;

    Inputs &operator=(const Inputs &) = delete;

    void init();
    void update();

    GLFWcursor* cursor;
};
