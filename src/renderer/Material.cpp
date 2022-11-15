//
// Created by elise on 26/09/2022.
//

#include "Material.h"
#include "Renderer.h"
#include <utility>

Material::Material(Renderer* pRenderer, std::vector<ShaderInfo> shadersInfo, std::string pName ):
renderer{pRenderer},
shaders{loadShaders(shadersInfo)},
pipeline{loadPipeline()},
name{pName}{
    renderer->mainDeletionQueue.push_function([=]() {this->cleanUp();});
}

std::vector<Shader> Material::loadShaders(std::vector<ShaderInfo> info){
    std::vector<Shader> shaders;
    shaders.reserve(info.size());

    for(auto shaderInfo : info){
        shaders.emplace_back(renderer, shaderInfo.fileName, shaderInfo.stage);
    }

    return shaders;
}

GraphicsPipeline Material::loadPipeline() {
    std::vector<vk::PipelineShaderStageCreateInfo> stages;
    stages.reserve(shaders.size());

    for(auto shader : shaders){
        vk::PipelineShaderStageCreateInfo shaderStageInfo;
        shaderStageInfo.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
        shaderStageInfo.pNext = nullptr;
        shaderStageInfo.stage = shader.getStage();
        shaderStageInfo.module = shader.getModule();
        shaderStageInfo.pName = "main";

        stages.emplace_back(shaderStageInfo);
    }
    //Copy elision, not call to copy constructor is made.
    return GraphicsPipeline{renderer, stages};
}

void Material::cleanUp() {
    for(auto shader : shaders){
        shader.cleanUp();
    }
    pipeline.cleanUp(renderer);

}










