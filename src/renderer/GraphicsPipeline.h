#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

class Renderer;

class GraphicsPipeline
{
public:
    GraphicsPipeline(Renderer* renderer, std::vector<vk::PipelineShaderStageCreateInfo>& stages);
    GraphicsPipeline() = default;
//    GraphicsPipeline(const GraphicsPipeline&) = delete;
	~GraphicsPipeline() = default;

    vk::Pipeline& getPipeline(){return graphicsPipeline;}

private:
	vk::PipelineLayout pipelineLayout;
	vk::Pipeline graphicsPipeline;

    void createGraphicsPipeline(Renderer* renderer, std::vector<vk::PipelineShaderStageCreateInfo>& stages);
    void destroy(Renderer* renderer);

};

