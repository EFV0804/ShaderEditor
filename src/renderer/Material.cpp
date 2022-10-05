//
// Created by elise on 26/09/2022.
//

#include "Material.h"

Material::Material(){

}
Material::~Material() {

}

void Material::addShader(vk::Device device, std::string filename, vk::ShaderStageFlagBits stage){
    std::shared_ptr<Shader> shader(new Shader(device, filename, stage));
    shaders.emplace_back(shader);
}

void Material::addPipeline(Renderer* renderer){
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = getShaderStages();
    std::shared_ptr<GraphicsPipeline> pipelinePtr (new GraphicsPipeline(renderer, &shaderStages));
    pipeline = pipelinePtr;
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
