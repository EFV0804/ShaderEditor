//
// Created by elise.vidal on 12/16/2022.
//

#include "EditorView.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "UI.h"
#include "Renderer.h"

void EditorView::init() {
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(UI::Get().window.getWindow(), true);
    Renderer::Get().initUI();
}

void EditorView::render() {

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //imgui commands
    ImGui::ShowDemoWindow();
    ImGui::Render();

}
