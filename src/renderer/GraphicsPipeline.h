#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

class Renderer;

class GraphicsPipeline
{
public:
    GraphicsPipeline(Renderer* renderer, std::vector<vk::PipelineShaderStageCreateInfo>& stages);
    GraphicsPipeline() = delete;
    GraphicsPipeline(const GraphicsPipeline&) = delete;
    GraphicsPipeline &operator=(const GraphicsPipeline &) = delete;
	~GraphicsPipeline() = default;

    const vk::Pipeline& getPipeline() const {return graphicsPipeline;}
    void cleanUp(Renderer* renderer);

private:
	vk::PipelineLayout pipelineLayout;
	vk::Pipeline graphicsPipeline;

    void createGraphicsPipeline(Renderer* renderer, std::vector<vk::PipelineShaderStageCreateInfo>& stages);


};

