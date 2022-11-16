//
// Created by elise on 26/09/2022.
//

#pragma once
#include "Shader.h"
#include "GraphicsPipeline.h"
#include <vector>
#include <memory>

/*!
 * \brief Struct the holds shader information used to create Shader object.
 *
 * \var fileName the path to the shader code.
 * \var stage The flag bit corresponding to the shader stage: ie: e::Vertex, e::Fragment etc.
 */
struct ShaderInfo{
    std::string fileName;
    vk::ShaderStageFlagBits stage;
    bool hasPushConstant;
};

/*!
 * \brief Class responsible for shaders and pipeline initialisation.
 */
class Material {
public:
    Material(std::vector<ShaderInfo> shadersInfo, std::string pName);
    Material() = delete;
    ~Material() = default;
//    Material(const Material&) = delete;
    Material& operator=(const Material&) = delete;

    /*
     * Name of the material. Must be informed on initialisation.
     */
    std::string name;
    const std::vector<vk::PushConstantRange>& getPushConstants() const {return pushConstants;}

private:
    /*!
     * \brief Initialises and stores Shader object in vector.
     *
     * \param info a vector a ShaderInfo struct instances containing filename and stage of shaders that need to be loaded.
     * \return std::vector<Shader>
     */
    std::vector<Shader> loadShaders(std::vector<ShaderInfo> info);
    /*!
     * \brief initialises the GraphicsPipeline object.
     *
     * \note the vk::Pipeline initialisation takes place in the GraphicsPipeline class.
     *
     * \return GraphicsPipeline
     */
    GraphicsPipeline loadPipeline(std::vector<ShaderInfo> shadersInfo);
    /*
     * store shaders needed for rendering of this material.
     */
    std::vector<Shader> shaders;
    std::vector<vk::PushConstantRange> pushConstants;

public:
    /*
     * The pipeline used to render.
     */
    GraphicsPipeline pipeline;

    void cleanUp() const;

};
