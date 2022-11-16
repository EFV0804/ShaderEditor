#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

class Renderer;

class GraphicsPipeline
{
public:
    GraphicsPipeline(std::vector<vk::PipelineShaderStageCreateInfo> &stages);
    GraphicsPipeline() = delete;
    GraphicsPipeline &operator=(const GraphicsPipeline &) = delete;
	~GraphicsPipeline() = default;

    /*!
     * \brief Getter function for vk::Pipeline member var.
     * \return vk::Pipeline
     */
    const vk::Pipeline& getPipeline() const {return graphicsPipeline;}
    /*!
     * \brief Calls Vulkan function to destroy vk::Pipeline and vk::PipelineLayout
     * \param renderer
     */
    void cleanUp() const;

private:
    /*
     * Pipeline Layout, allows to specify uniforms, push constants in shader code.
     * Right now this is a 'blank' pipeline layout until implemention of push constants etc.
     * //TODO Create vk::PipelineLayout based uniforms present in shader code. Using SPIRV-Cross
     */
	vk::PipelineLayout pipelineLayout;
    /*
     * The pipeline :)
     */
	vk::Pipeline graphicsPipeline;

    void createGraphicsPipeline(std::vector<vk::PipelineShaderStageCreateInfo>& stages);


};

