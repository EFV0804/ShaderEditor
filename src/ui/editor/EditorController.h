//
// Created by elise.vidal on 12/12/2022.
//
#include "Event.h"
#include "KeyboardEvents.h"


#ifndef SHADEREDITOR_EDITORCONTROLLER_H
#define SHADEREDITOR_EDITORCONTROLLER_H


class EditorController {
public:
    EditorController() = default;
    ~EditorController() = default;
    EditorController &operator=(const EditorController&) = delete;
    EditorController(const EditorController&) = delete;

    void OnEvent(Event& e);
    void update();
    bool onKeyPressed(KeyPressedEvent& e);
};


#endif //SHADEREDITOR_EDITORCONTROLLER_H
