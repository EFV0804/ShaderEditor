#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#include "Pipeline.h"

class VKRenderer;

class GraphicsPipeline:
    public Pipeline
{
public:
    GraphicsPipeline(std::vector<vk::PipelineShaderStageCreateInfo> &stages, std::vector<vk::PushConstantRange>& pushConstants);
    GraphicsPipeline() = delete;
    GraphicsPipeline &operator=(const GraphicsPipeline &) = delete;
	~GraphicsPipeline() = default;

    /*!
     * \brief Calls Vulkan function to destroy vk::Pipeline and vk::PipelineLayout
     * \param renderer
     */
     void cleanUp() const override;

private:
    void createPipeline(std::vector<vk::PipelineShaderStageCreateInfo> &stages,
                                std::vector<vk::PushConstantRange> pushConstants);

};

