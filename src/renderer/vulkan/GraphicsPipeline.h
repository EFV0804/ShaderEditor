#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

class VKRenderer;

class GraphicsPipeline
{
public:
    GraphicsPipeline(std::vector<vk::PipelineShaderStageCreateInfo> &stages, std::vector<vk::PushConstantRange>& pushConstants);
    GraphicsPipeline() = delete;
    GraphicsPipeline &operator=(const GraphicsPipeline &) = delete;
	~GraphicsPipeline() = default;

    /*!
     * \brief Getter function for vk::Pipeline member var.
     * \return vk::Pipeline
     */
    const vk::Pipeline& getPipeline() const {return graphicsPipeline;}
    const vk::PipelineLayout& getLayout() const {return pipelineLayout;}
    /*!
     * \brief Calls Vulkan function to destroy vk::Pipeline and vk::PipelineLayout
     * \param renderer
     */
    void cleanUp() const;

private:
    /*
     * Pipeline Layout, allows to specify uniforms, push constants in shader code.
     * //TODO Create vk::PipelineLayout based uniforms present in shader code. Using SPIRV-Cross
     */
	vk::PipelineLayout pipelineLayout;
    vk::DescriptorSetLayout descriptorSetLayout;
    vk::DescriptorSet descriptorSet;
    /*
     * The pipeline :)
     */
	vk::Pipeline graphicsPipeline;

    void createGraphicsPipeline(std::vector<vk::PipelineShaderStageCreateInfo> &stages,
                                std::vector<vk::PushConstantRange> pushConstants);


};

