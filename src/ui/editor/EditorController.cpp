//
// Created by elise.vidal on 12/12/2022.
//

#include "EditorController.h"

void EditorController::OnEvent(Event& e) {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<EventType::KeyPressed>();
    dispatcher.Dispatch<EventType::MouseButtonPressed>()
}

void EditorController::update() {

}

bool EditorController::onKeyPressed(KeyPressedEvent& e) {
    switch (e.getKeycode()) {
        case 256: //escape
        {
            //Exit program
            //TODO: exit program on key pressed
            break;
        }

    }
    return false;
}
