//
// Created by elise.vidal.
//

#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>


struct DepthBufferImage {
    vk::Image depthImage;
    vk::DeviceMemory depthImageMemory;
    vk::ImageView depthImageView;
};

class RenderPass {
private:
    vk::RenderPass renderPass;
public:
    RenderPass() = default;

    ~RenderPass() = default;

    RenderPass(const RenderPass &) = delete;

    RenderPass &operator=(const RenderPass &) = delete;

    void init();

    void initFramebuffers();
    void initViewportFramebuffers();
    void cleanUp();

    void begin(const uint32_t &frameIndex, const vk::CommandBuffer &cmd);

    void end(const vk::CommandBuffer &cmd);

    const vk::RenderPass &getRenderPass() const { return renderPass; }
    const std::vector<vk::ImageView> getImageViews() const {return imageViews;}
    DepthBufferImage depthBufferImage;
private:
    vk::Format findDepthFormat();

    void createDepthBufferResources();

    std::vector<vk::Image> viewportImages;
    std::vector<vk::DeviceMemory> viewportImagesMemory;
    /*
     * A vector of framebuffers. A framebuffer references an image view for the swapchain that are used for color, depth and stencils.
     */
    std::vector<vk::Framebuffer> framebuffers;
    /**
    * A vector of swapchain images, that are used as attachments for framebuffers.
    */
    std::vector<vk::ImageView> imageViews;
    /*
    * Describes how the rendering process should go. Manages the relationship between attachments and subpasses.
    */

    std::vector<vk::ImageView> attachments;
//    DepthBufferImage depthBufferImage;
};
