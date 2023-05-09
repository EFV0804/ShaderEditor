//
// Created by elise on 09/05/2023.
//

#include "ComputeShader.h"
#include "VKRenderer.h"

ComputeShader::ComputeShader(std::string pFilename, uint32_t pSizeElement, int pNumElements):
sizeDateElement{pSizeElement},
numElements{pNumElements},
Shader(filename, vk::ShaderStageFlagBits::eCompute),
pipeline{loadPipeline()}
{


    loadPipeline();
}

const ComputePipeline ComputeShader::loadPipeline() {

    vk::PipelineShaderStageCreateInfo shaderStageInfo = {};
    shaderStageInfo.flags = vk::PipelineShaderStageCreateFlags(),
    shaderStageInfo.stage = vk::ShaderStageFlagBits::eCompute,
    shaderStageInfo.pName = "main";

    return ComputePipeline{shaderStageInfo, sizeDateElement, workGroups, numElements};
}
