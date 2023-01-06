//
// Created by elise.vidal on 1/6/2023.
//

#include "Viewport.h"
#include "Renderer.h"
#include "imgui.h"

void Viewport::init() {
    Renderer::Get().initViewport();

//    ImGui::Begin("Viewport");
//    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
//    int currentFrame = Renderer::Get().getCurrentFrame();
//    //ImGui::Image accepts descriptor set pointing to texture
//    ImGui::Image(Renderer::Get().getViewportImages<std::vector<vk::DescriptorSet>>().at(currentFrame),
//                 ImVec2{viewportPanelSize.x, viewportPanelSize.y});
//
//    ImGui::End();
}