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

    projection[1][1] *= -1;
    glm::mat4 model = glm::rotate(glm::mat4{ 1.0f }, glm::radians( 0.1f), glm::vec3(2, 1, 0));
    glm::mat4 mesh_matrix = projection * view * model;

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
    renderables.emplace_back(meshes.back(), materials.back(), mesh_matrix);
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