//
// Created by elise on 26/09/2022.
//

#pragma once
#include "Shader.h"
#include "GraphicsPipeline.h"
#include <vector>
#include <memory>

struct ShaderInfo{
    std::string fileName;
    vk::ShaderStageFlagBits stage;
};

class Material {
public:
    Material(Renderer* pRenderer, std::vector<ShaderInfo> shadersInfo, std::string pName);
    Material() = delete;
    ~Material() = default;
    Material(const Material&) = delete;
    Material& operator=(const Material&) = delete;

    std::string name;

private:
    Renderer* renderer;
    std::vector<Shader> loadShaders(std::vector<ShaderInfo> info);
    GraphicsPipeline loadPipeline();
    std::vector<Shader> shaders;
    GraphicsPipeline pipeline;

    void cleanUp();

};
