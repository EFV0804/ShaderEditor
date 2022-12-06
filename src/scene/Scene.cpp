//
// Created by elise on 26/09/2022.
//

#include "Scene.h"
#include "Mesh.h"
#include "Material.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
Scene::Scene(){

};

Scene::~Scene() = default;

void Scene::load() {
    glm::vec3 camPos = { 0.0f, 0.0f, -2.0f};
    camBuffer.view = glm::translate(glm::mat4(1.f), camPos);
    camBuffer.proj = glm::perspective(glm::radians(70.f),
                                      Renderer::Get().getSwapchainExtent().width / (float) Renderer::Get().getSwapchainExtent().height,
                                      0.1f,
                                      200.0f);

    camBuffer.proj[1][1] *= -1;
    camBuffer.viewproj = camBuffer.proj* camBuffer.view;

//    glm::mat4 model = glm::rotate(glm::mat4{ 1.0f }, glm::radians( 0.1f), glm::vec3(2, 1, 0));
    glm::mat4 model = glm::mat4{ 1.0f };


    ShaderInfo vertInfo{"../../assets/shaders/compiled/shader_descriptorSet.vert.spv",
                        vk::ShaderStageFlagBits::eVertex, true};
    ShaderInfo fragInfo{"../../assets/shaders/compiled/shader.frag.spv",
                        vk::ShaderStageFlagBits::eFragment, false};

    std::vector<ShaderInfo> shadersInfo;
    shadersInfo.reserve(2);
    shadersInfo.push_back(vertInfo);
    shadersInfo.push_back(fragInfo);

    materials.emplace_back(shadersInfo, "triangleMat");
    meshes.emplace_back("../../assets/models/suzanne.obj");
    for(auto& mesh : meshes){
        mesh.loadFromFile();
    }
    renderables.emplace_back(meshes.back(), materials.back(), model);
    Renderer::Get().loadMeshes(&renderables);
//    Renderer::Get().updateCameraBuffer(camBuffer);

    // ----------- ADD TO DELETION QUEUE ----------------
    for(auto material : materials){
        sceneDeletionQueue.push_function([=]() {material.cleanUp();});
    }
}
void Scene::update(){
    //update renderable positions, world transforms, etc.

    // Update CameraBuffer with new data


    for(auto& renderable : renderables){

        renderable.transform = glm::rotate(renderable.transform, glm::radians( Renderer::Get().currentFrame*0.9f), glm::vec3(0, 1, 0));
    }
    Renderer::Get().updateCameraBuffer(camBuffer);

}
void Scene::draw() {
    Renderer::Get().updateCameraBuffer(camBuffer);
    Renderer::Get().draw(&renderables);
}

void Scene::cleanUp() {
    Renderer::Get().device.waitIdle();
    sceneDeletionQueue.flush();
}