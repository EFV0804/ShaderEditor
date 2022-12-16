//
// Created by elise.vidal.
//

#include "Inputs.h"
#include "UI.h"
void Inputs::init() {
    cursor = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    glfwSetCursor(UI::Get().window.getWindow(), cursor);
}

void Inputs::update() {


}
