//
// Created by elise on 26/09/2022.
//

#include "Scene.h"
#include "Mesh.h"
#include "Material.h"

Scene::Scene(){

};

Scene::~Scene() = default;

void Scene::load() {
    camBuffer.camPos = {0.f,0.f,-2.f};
    camBuffer.view = glm::translate(glm::mat4(1.f), camBuffer.camPos);
    camBuffer.proj = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.0f);

    camBuffer.proj[1][1] *= -1;
    camBuffer.viewproj = camBuffer.proj* camBuffer.view;

    glm::mat4 model = glm::rotate(glm::mat4{ 1.0f }, glm::radians( 0.1f), glm::vec3(2, 1, 0));


    ShaderInfo vertInfo{"../../assets/shaders/compiled/shader.vert.spv",
                        vk::ShaderStageFlagBits::eVertex, true};
    ShaderInfo fragInfo{"../../assets/shaders/compiled/shader.frag.spv",
                        vk::ShaderStageFlagBits::eFragment, false};

    std::vector<ShaderInfo> shadersInfo;
    shadersInfo.reserve(2);
    shadersInfo.push_back(vertInfo);
    shadersInfo.push_back(fragInfo);

    materials.emplace_back(shadersInfo, "triangleMat");
    meshes.emplace_back();
    renderables.emplace_back(meshes.back(), materials.back(), model);
    Renderer::Get().loadMeshes(&renderables);


    // ----------- ADD TO DELETION QUEUE ----------------
    for(auto material : materials){
        sceneDeletionQueue.push_function([=]() {material.cleanUp();});
    }
}

void Scene::draw() {
    Renderer::Get().draw(&renderables);
}

void Scene::cleanUp() {
    Renderer::Get().device.waitIdle();
    sceneDeletionQueue.flush();
}