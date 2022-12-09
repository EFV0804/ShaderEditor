//
// Created by elise on 26/09/2022.
//

#include "Scene.h"
#include "UI.h"
#include "vulkan/Mesh.h"
#include "vulkan/Material.h"
#include "Timer.h"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

Scene::Scene() {

};

Scene::~Scene() = default;

void Scene::load() {
    glm::vec3 camPos = {-18.0f, -17.0f, -25.0f};
    camBuffer.view = glm::translate(glm::mat4(1.f), camPos);
    camBuffer.proj = glm::perspective(glm::radians(70.f),
                                      (float) UI::Get().window.getWidth() / (float) UI::Get().window.getHeight(),
                                      0.1f,
                                      200.0f);

    camBuffer.proj[1][1] *= -1;
    camBuffer.viewproj = camBuffer.proj * camBuffer.view;

//    glm::mat4 model = glm::rotate(glm::mat4{ 1.0f }, glm::radians( 0.1f), glm::vec3(2, 1, 0));
//    glm::mat4 model = glm::mat4{ 1.0f };


    ShaderInfo vertInfo{"../../assets/shaders/compiled/shader_descriptorSet.vert.spv",
                        vk::ShaderStageFlagBits::eVertex, true};
    ShaderInfo fragInfo{"../../assets/shaders/compiled/shader.frag.spv",
                        vk::ShaderStageFlagBits::eFragment, false};

    std::vector<ShaderInfo> shadersInfo;
    shadersInfo.reserve(2);
    shadersInfo.push_back(vertInfo);
    shadersInfo.push_back(fragInfo);

    materials.emplace_back(shadersInfo, "suzanne_mat");
    meshes.emplace_back("../../assets/models/suzanne.obj");

    for (auto &mesh: meshes) {
        mesh.loadFromFile();

    }
    glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(18, 17, 20));
    renderables.emplace_back(meshes.back(), materials.back(), model);

    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 15; j++) {
            for (int h = 0; h < 15; h++) {
                glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(i * 2.5, j * 2.5, -5 - (h * 2.5)));
                renderables.emplace_back(meshes.back(), materials.back(), model);
            }
        }
    }


    Renderer::Get().loadMeshes(&renderables);

    // ----------- ADD TO DELETION QUEUE ----------------
    for (auto material: materials) {
        sceneDeletionQueue.push_function([=]() { material.cleanUp(); });
    }
}

void Scene::update(float dt) {
    //update renderable positions, world transforms, etc.

    // Update CameraBuffer with new data

//    renderables.at(0).transform = glm::rotate(renderables.at(0).transform, glm::radians( Renderer::Get().currentFrame*0.9f), glm::vec3(0, 1, 0));
//    renderables.at(1).transform = glm::translate(glm::vec3(1,0,0));

    for (auto &renderable: renderables) {

        renderable.transform = glm::rotate(renderable.transform, glm::radians(Renderer::Get().getCurrentFrame() * 20.f) * dt,
                                           glm::vec3(0, 1, 0));
//        renderable.transform += glm::translate( glm::vec3(index*3.5,0,-5));
//        glm::mat4 scale = glm::scale(glm::vec3(0.1,0.1,0.1));
//        glm::mat4 new_model  = translation * rotation * renderable.transform;
//        renderable.transform = new_model;
    }
    Renderer::Get().updateCamera(camBuffer);

}

void Scene::draw() {
    Renderer::Get().draw(&renderables);
}

void Scene::cleanUp() {
    Renderer::Get().waitIdle();
    sceneDeletionQueue.flush();
}