//
// Created by elise on 09/05/2023.
//

#ifndef SHADEREDITOR_IPIPELINE_H
#define SHADEREDITOR_IPIPELINE_H

#include <vulkan/vulkan.hpp>
#include <vector>

class VKRenderer;

class Pipeline {
public:
    Pipeline();

    virtual Pipeline &operator=(const Pipeline &) = delete;
    ~Pipeline() = default;

    /*!
     * \brief Getter function for vk::Pipeline member var.
     * \return vk::Pipeline
     */
    const vk::Pipeline & getPipeline() const {return pipeline;}
    const vk::PipelineLayout & getPipelineLayout() const {return pipelineLayout;}
    const vk::DescriptorSet& getDsescriptorSet() const {return descriptorSet;}
    const vk::DescriptorSetLayout& getDescriptorSetLayout() const {return descriptorSetLayout;}
    void SetPipeline(vk::Pipeline pipelineP){pipeline = pipelineP;}
    void SetPipelineLayout(vk::PipelineLayout pipelineLayoutP){pipelineLayout = pipelineLayoutP;}
    void SetDescriptorSet(vk::DescriptorSet descriptorSetP){descriptorSet = descriptorSetP;}
    void SetDescriptorSetLayout(vk::DescriptorSetLayout descriptorSetLayoutP){descriptorSetLayout = descriptorSetLayoutP;}

     /*!
     * \brief Calls Vulkan function to destroy vk::Pipeline and vk::PipelineLayout
     * \param renderer
     */
     virtual void cleanUp() const;

private:
    vk::DescriptorSetLayout descriptorSetLayout;
    vk::DescriptorSet descriptorSet;
    /*
     * The pipeline :)
     */
    vk::Pipeline pipeline;
/*
 * Pipeline Layout, allows to specify uniforms, push constants in shader code.
 * //TODO Create vk::PipelineLayout based uniforms present in shader code. Using SPIRV-Cross
 */
    vk::PipelineLayout pipelineLayout;
};



#endif //SHADEREDITOR_IPIPELINE_H
