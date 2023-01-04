//
// Created by elise.vidal.
//

#include "Renderer.h"

void Renderer::init() {
    VKRenderer::Get().init();
}
void Renderer::initUI() {
    VKRenderer::Get().initImgui();
}
void Renderer::loadMeshes(std::vector<Renderable>* renderables){
    VKRenderer::Get().loadMeshes(renderables);
}
void Renderer::draw(std::vector<Renderable>* renderables) {
    VKRenderer::Get().draw(renderables);
}
void Renderer::update(){

}
void Renderer::updateCamera(CameraBuffer camBuffer) {
    //TODO: pass cam transform rather a cam buffer that belongs on the vulkan side
    VKRenderer::Get().updateCameraBuffer(camBuffer);
}
void Renderer::waitIdle(){
    VKRenderer::Get().device.waitIdle();
}
void Renderer::close() {
    VKRenderer::Get().close();
}