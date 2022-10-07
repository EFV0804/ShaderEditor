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


//    const std::string vertFilename = "../../assets/shaders/compiled/shader.vert.spv";
//    const std::string fragFilename = "../../assets/shaders/compiled/shader.frag.spv";
//
//    std::vector<std::string> shaderNames = {vertFilename,fragFilename};
//    Material triangleMaterial = createMaterial(shaderNames, renderer);
//
//    Renderable triangle("triangle", triangleMaterial);
//
//    renderables.emplace_back(triangle);

    Mesh triangleMesh = Mesh();
    Material triangleMaterial = Material();

    Shader* triangleVertShader = new Shader(renderer->device,
                                        "../../assets/shaders/compiled/shader.vert.spv",
                                        vk::ShaderStageFlagBits::eVertex);
    Shader* triangleFragShader = new Shader(renderer->device,
                                        "../../assets/shaders/compiled/shader.frag.spv",
                                        vk::ShaderStageFlagBits::eVertex);

//    triangleMaterial.addShader(renderer->device,
//                               "../../assets/shaders/compiled/shader.vert.spv",
//                               vk::ShaderStageFlagBits::eVertex);
//
//    triangleMaterial.addShader(renderer->device,
//                               "../../assets/shaders/compiled/shader.frag.spv",
//                               vk::ShaderStageFlagBits::eFragment);

//    triangleMaterial.addPipeline(renderer);


    Renderable triangle{triangleMesh, triangleMaterial};

    renderables.emplace_back(triangle);
    renderer->loadMeshes(&renderables);
}

void Scene::draw(Renderer* renderer){
    renderer->draw(&renderables);
}

void Scene::close() {

}

Material Scene::createMaterial(std::vector<std::string> shaderNames, Renderer* renderer){

    std::vector<ShaderInfo> shadersInfo = {
            {"../../assets/shaders/compiled/shader.vert.spv", vk::ShaderStageFlagBits::eVertex},
            {"../../assets/shaders/compiled/shader.frag.spv", vk::ShaderStageFlagBits::eFragment}
    };

//
//    for(auto shaderName : shaderNames){
//        if(shaderName.find("vert") != std::string::npos){
//            std::string file = shaderName;
//            ShaderInfo info{file, vk::ShaderStageFlagBits::eVertex};
//            shadersInfo.emplace_back(info);
//        }
//        else if(shaderName.find("frag") != std::string::npos){
//            std::string file = shaderName;
//            ShaderInfo info{file, vk::ShaderStageFlagBits::eFragment};
//            shadersInfo.emplace_back(info);
//        }
//    }


    Material triangleMaterial(shadersInfo, renderer);
    return triangleMaterial;
}