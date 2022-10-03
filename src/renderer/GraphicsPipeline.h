#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

class Renderer;

class GraphicsPipeline
{
public:
    GraphicsPipeline(Renderer* renderer, std::vector<vk::PipelineShaderStageCreateInfo> stages);
    GraphicsPipeline() = default;
	~GraphicsPipeline() = default;

    void destroy(Renderer* renderer);
	void draw();

    vk::Pipeline getPipeline(){return graphicsPipeline;}

private:
	vk::PipelineLayout pipelineLayout;
	vk::Pipeline graphicsPipeline;

//	vk::ImageView createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags);
	void addShaders();
    void createGraphicsPipeline(Renderer* renderer, std::vector<vk::PipelineShaderStageCreateInfo> stages);

};

