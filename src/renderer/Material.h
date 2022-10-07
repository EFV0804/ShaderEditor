//
// Created by elise on 26/09/2022.
//

#ifndef SHADEREDITOR_MATERIAL_H
#define SHADEREDITOR_MATERIAL_H
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
    Material(std::vector<ShaderInfo>, Renderer* renderer);
    Material() = default;
    ~Material();

    std::vector<Shader*> shaders;
    GraphicsPipeline pipeline;

    void addShaders(Renderer* renderer, std::vector<ShaderInfo> shadersInfo);
    void addShader(vk::Device device, std::string filename, vk::ShaderStageFlagBits stage);
    void addPipeline(Renderer* renderer);
    std::vector<vk::PipelineShaderStageCreateInfo> getShaderStages();
    void load();
    void destroy(vk::Device device);
};


#endif //SHADEREDITOR_MATERIAL_H
