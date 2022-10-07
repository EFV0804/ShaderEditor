//
// Created by elise on 26/09/2022.
//

#include "Material.h"
#include "Renderer.h"
#include <utility>

Material::Material(std::vector<ShaderInfo> shadersInfo, Renderer* renderer){
    addShaders(renderer, shadersInfo);
    addPipeline(renderer);
}
Material::~Material() {

}

void Material::addShaders(Renderer* renderer, std::vector<ShaderInfo> shadersInfo){

    for(auto shaderInfo : shadersInfo){
        Shader shader{renderer->device, shaderInfo.fileName, shaderInfo.stage};
        shaders.emplace_back(&shader);
    }


}

void Material::addShader(vk::Device device, std::string filename, vk::ShaderStageFlagBits stage) {

    Shader shader{device, filename, stage};
    shaders.emplace_back(&shader);
}

void Material::addPipeline(Renderer* renderer){
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = getShaderStages();
    pipeline = GraphicsPipeline(renderer, shaderStages);

}
std::vector<vk::PipelineShaderStageCreateInfo> Material::getShaderStages(){

    // Get the shader stage info of each shader that will be used
    std::vector<vk::PipelineShaderStageCreateInfo> pipelineShaderStages;

    for(auto shader : shaders){
        vk::PipelineShaderStageCreateInfo shaderStageInfo;
        shaderStageInfo.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
        shaderStageInfo.pNext = nullptr;
        shaderStageInfo.stage = shader->stage;
        shaderStageInfo.module = shader->module;
        shaderStageInfo.pName = "main";

        pipelineShaderStages.emplace_back(shaderStageInfo);
    }
    return pipelineShaderStages;

}
void Material::load() {


    // load pipeline
}

void Material::destroy(vk::Device device){
    device.destroyPipeline();
}

//
//void Material::assign() {

//}
