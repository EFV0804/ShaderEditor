//
// Created by elise on 22/09/2022.
//

#ifndef SHADEREDITOR_SHADER_H
#define SHADEREDITOR_SHADER_H
#include <vulkan/vulkan.hpp>
#include <vector>
#include <string>

class Shader {
public:
    Shader(vk::Device device, std::string filename, vk::ShaderStageFlagBits stage);
    Shader();
    ~Shader();

    std::string filename;
    vk::Device device;
    vk::ShaderModule module;
    vk::ShaderStageFlagBits stage;

    static std::vector<char> readShaderFile(const std::string& filename);
    void setShaderModule();
    void createShaderModule(std::vector<char> shaderCode);
};


#endif //SHADEREDITOR_SHADER_H
