//
// Created by elise on 26/09/2022.
//

#include "Scene.h"
#include "Mesh.h"
#include "Material.h"

Scene::Scene(){

};

Scene::~Scene() = default;

void Scene::load(Renderer* renderer) {

    ShaderInfo vertInfo{"../../assets/shaders/compiled/shader.vert.spv",
                        vk::ShaderStageFlagBits::eVertex};
    ShaderInfo fragInfo{"../../assets/shaders/compiled/shader.frag.spv",
                        vk::ShaderStageFlagBits::eFragment};

    std::vector<ShaderInfo> shadersInfo;
    shadersInfo.reserve(2);
    shadersInfo.push_back(vertInfo);
    shadersInfo.push_back(fragInfo);

    materials.emplace_back(renderer, shadersInfo, "triangleMat");
    meshes.emplace_back();
    renderables.emplace_back(meshes.back(), materials.back());
    renderer->loadMeshes(&renderables);


    // ----------- ADD TO DELETION QUEUE ----------------
    for(auto material : materials){
        sceneDeletionQueue.push_function([=]() {material.cleanUp();});
    }
}

void Scene::draw(Renderer* renderer){
    renderer->draw(&renderables);
}

void Scene::cleanUp(Renderer* renderer) {
    renderer->device.waitIdle();
    sceneDeletionQueue.flush();
}