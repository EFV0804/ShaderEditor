//
// Created by elise.vidal on 12/16/2022.
//

#include "EditorView.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "UI.h"
#include "Renderer.h"
#include "VKRenderer.h"

void EditorView::init() {
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(UI::Get().window.getWindow(), true);
    Renderer::Get().initUI();
    viewport.init();
}

void EditorView::render() {

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Viewport");
    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    int currentFrame = Renderer::Get().getCurrentFrame();
    //ImGui::Image accepts descriptor set pointing to texture
    ImGui::Image(VKRenderer::Get().ds.at(0),
                 ImVec2{viewportPanelSize.x, viewportPanelSize.y});

    ImGui::End();
    //imgui commands
    ImGui::ShowDemoWindow();
    ImGui::EndFrame();
    ImGui::Render();

}
