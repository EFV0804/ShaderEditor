//
// Created by elise.vidal on 12/12/2022.
//

#include "EditorController.h"
#include "Logger.h"
#include "UI.h"
#include <memory>

#define LAMBDA(func) [this](auto& boop){func(boop);}

void EditorController::onEvent(Event& e) {

    EventDispatcher dispatcher;
    dispatcher.dispatch<KeyPressedEvent>(LAMBDA(EditorController::onKeyPressed),e);

}

void EditorController::update() {

}

bool EditorController::onKeyPressed(const KeyPressedEvent& e) {
    SE_UI_DEBUG("Key pressed event");
    SE_UI_DEBUG("Pressed key code: {}", e.getKeycode());

    switch (e.getKeycode()) {
        case 256: //escape
        {
            UI::Get().window.setWindowShouldClose(true);
            break;
        }

    }
    return false;
}
