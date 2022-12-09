//
// Created by elise on 22/09/2022.
//

#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>
#include <string>
#include "VKRenderer.h"

class Shader {
public:
    Shader(std::string filename, vk::ShaderStageFlagBits stage);
    Shader() = delete;
    Shader& operator=(const Shader&) = delete;
    ~Shader() = default;

    /*!
     * \brief Getter for module member variable.
     * \return vk::ShaderModule module
     */
    const vk::ShaderModule& getModule() const {return module;}
    /*!
     * \brief Getter for shader stage mebmer variable.
     * \return vk::ShaderStageFlagBits stage
     */
    const vk::ShaderStageFlagBits& getStage() const {return stage;}
    /*!
     * \brief Call Vulkan function to destroy vk::ShaderModule
     *
     * \note This function is to be added to the main deletion queue.
     */
    void cleanUp();
private:

    /*
     * path to the shader file.
     */
    std::string filename;
    /*
     *  The flag bit corresponding to the shader stage: ie: e::Vertex, e::Fragment etc.
     */
    vk::ShaderStageFlagBits stage;
    /*
     * Vector to hold shader code, to be stored in shader module
     */
    std::vector<char> shaderCode;
    /*
     * Shader Module that holds all information about the shader, used by the pipeline initialisation process.
     */
    vk::ShaderModule module;
    /*!
     * \brief Reads binary code from shader file found at the location of fileName
     * \return std::vector<char> shader code
     */
    std::vector<char> getShaderCode();
    /*!
     * \brief makes vk::ShaderModule
     * \return vk::ShaderModule
     * \note Must be called after the shader code has been read and stored in member variable shaderCode.
     */
    vk::ShaderModule makeModule();

};
