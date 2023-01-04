//
// Created by elise.vidal on 12/16/2022.
//
#include <imgui.h>
#ifndef SHADEREDITOR_EDITORVIEW_H
#define SHADEREDITOR_EDITORVIEW_H


class EditorView {
public:
    EditorView() = default;

    ~EditorView() = default;

    EditorView &operator=(const EditorView &) = delete;

    EditorView(const EditorView &) = delete;

    void init();
    void render();

};


#endif //SHADEREDITOR_EDITORVIEW_H
