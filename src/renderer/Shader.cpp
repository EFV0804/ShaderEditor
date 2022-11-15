//
// Created by elise on 22/09/2022.
//

#include <fstream>
#include "Shader.h"

Shader::Shader(Renderer* renderer, std::string filename,vk::ShaderStageFlagBits stage):
renderer{renderer},
filename{filename},
stage{stage},
shaderCode{getShaderCode()},
module{makeModule()}{

}


vk::ShaderModule Shader::makeModule() {

    vk::ShaderModuleCreateInfo shaderModuleCreateInfo{};
    shaderModuleCreateInfo.sType = vk::StructureType::eShaderModuleCreateInfo;
    shaderModuleCreateInfo.codeSize = shaderCode.size();
    shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

    return renderer->device.createShaderModule(shaderModuleCreateInfo);
}

std::vector<char> Shader::getShaderCode() {
    // Open shader file
    // spv files are binary data, put the pointer at the end of the file to get its size
    std::ifstream file{ filename, std::ios::binary | std::ios::ate };
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open a file");
    }
    // Buffer preparation
    size_t fileSize = (size_t)file.tellg(); // Get the size through the position of the pointer
    std::vector<char> fileBuffer(fileSize); // Set file buffer to the file size
    file.seekg(0); // Move in file to start of the file
    // Reading and closing
    file.read(fileBuffer.data(), fileSize);
    file.close();
    return fileBuffer;
}

void Shader::cleanUp(){
    renderer->device.destroyShaderModule(module);
}