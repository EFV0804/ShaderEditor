//
// Created by elise on 26/09/2022.
//

#include "Scene.h"
#include "Mesh.h"
#include "Material.h"

Scene::Scene() = default;

Scene::~Scene() = default;

void Scene::load(Renderer* renderer) {
    Mesh triangleMesh = Mesh();
    Material triangleMaterial = Material();

    triangleMaterial.addShader(renderer->device,
                       "../../assets/shaders/compiled/shader.vert.spv",
                       vk::ShaderStageFlagBits::eVertex);
    triangleMaterial.addShader(renderer->device,
                       "../../assets/shaders/compiled/shader.frag.spv",
                       vk::ShaderStageFlagBits::eFragment);
    triangleMaterial.addPipeline(renderer);

    Renderable triangle = Renderable(&triangleMesh, &triangleMaterial);

    renderables.emplace_back(triangle);
}

void Scene::draw(Renderer* renderer){
    renderer->draw(&renderables);
}

void Scene::close() {

}
