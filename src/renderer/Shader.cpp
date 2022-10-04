//
// Created by elise on 22/09/2022.
//

#include <fstream>
#include "Shader.h"
Shader::Shader(vk::Device device, std::string filename,vk::ShaderStageFlagBits stage):
device{device},
filename{filename},
stage{stage}{
    setShaderModule();
}
Shader::Shader(){
    //TODO load default shader
}
void Shader::createShaderModule(std::vector<char> shaderCode)
{
    vk::ShaderModuleCreateInfo shaderModuleCreateInfo{};
    shaderModuleCreateInfo.sType = vk::StructureType::eShaderModuleCreateInfo;
    shaderModuleCreateInfo.codeSize = shaderCode.size();
    shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

    module = device.createShaderModule(shaderModuleCreateInfo);
}

void Shader::setShaderModule() {
    std::vector<char> shaderCode = readShaderFile(filename);
    createShaderModule(shaderCode);
}

std::vector<char> Shader::readShaderFile(const std::string &filename) {
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

Shader::~Shader(){
    device.destroyShaderModule(module);
}