//
// Created by elise on 26/09/2022.
//

#ifndef SHADEREDITOR_MATERIAL_H
#define SHADEREDITOR_MATERIAL_H
#include "Shader.h"
#include "GraphicsPipeline.h"
#include <vector>
#include <memory>

class Material {
public:
    Material();
    ~Material();

    std::vector<std::shared_ptr<Shader>> shaders;
    std::shared_ptr<GraphicsPipeline> pipeline;

    //Function to assign material with object
    //  void use();
    void addShader(vk::Device device, std::string filename,vk::ShaderStageFlagBits stage);
    void addPipeline(Renderer* renderer);
    std::vector<vk::PipelineShaderStageCreateInfo> getShaderStages();
    void init();
    void load();
    void destroy(vk::Device device);
};


#endif //SHADEREDITOR_MATERIAL_H
