//
// Created by elise on 22/09/2022.
//

#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>
#include <string>
#include "Renderer.h"

class Shader {
public:
    Shader(Renderer* renderer, std::string filename, vk::ShaderStageFlagBits stage);
    Shader() = delete;
//    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    ~Shader() = default;

    const vk::ShaderModule& getModule() const {return module;}
    const vk::ShaderStageFlagBits& getStage() const {return stage;}

    void cleanUp();
private:

    std::string filename;
    Renderer* renderer;
    vk::ShaderStageFlagBits stage;
    std::vector<char> shaderCode;
    vk::ShaderModule module;


    std::vector<char> getShaderCode();
    vk::ShaderModule makeModule();

};
