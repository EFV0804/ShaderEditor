//
// Created by elise on 26/09/2022.
//

#include "Material.h"
#include "VKRenderer.h"
#include <utility>
#include "glm/glm.hpp"

Material::Material(std::vector<ShaderInfo> shadersInfo, std::string pName) :
shaders{loadShaders(shadersInfo)},
pipeline{loadPipeline(shadersInfo)},
name{pName}{

}

std::vector<Shader> Material::loadShaders(std::vector<ShaderInfo> info){
    std::vector<Shader> shaders;
    shaders.reserve(info.size());

    for(auto shaderInfo : info){
        shaders.emplace_back( shaderInfo.fileName, shaderInfo.stage);
    }

    return shaders;
}

GraphicsPipeline Material::loadPipeline(std::vector<ShaderInfo> shadersInfo) {
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
    for(auto info : shadersInfo){
        if(info.hasPushConstant == true){
            vk::PushConstantRange pushConstant;
            pushConstant.offset = 0;
            pushConstant.size = sizeof(glm::mat4);
            pushConstant.stageFlags = info.stage;
            pushConstants.push_back(pushConstant);
        }
    }
    //Copy elision, no call to copy constructor is made.
    return GraphicsPipeline{ stages, pushConstants};
}

void Material::cleanUp() const {
    for(auto shader : shaders){
        shader.cleanUp();
    }
    pipeline.cleanUp();

}










