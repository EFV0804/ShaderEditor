//
// Created by elise.vidal.
//

#include "RenderPass.h"
#include "Renderer.h"

void RenderPass::init() {
    SE_RENDERER_DEBUG("RenderPass initialisation");

    Renderer &renderer = Renderer::Get();

    vk::RenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = vk::StructureType::eRenderPassCreateInfo;
    renderPassCreateInfo.pNext = nullptr;
    renderPassCreateInfo.flags = vk::RenderPassCreateFlags();


    vk::AttachmentDescription colorAttachment{};
    colorAttachment.format = renderer.swapchain.getSwapchainImageFormat();
    colorAttachment.samples = vk::SampleCountFlagBits::e1;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentDescription depthAttachment{};
    depthAttachment.format = findDepthFormat();
    depthAttachment.samples = vk::SampleCountFlagBits::e1;
    depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
    depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    std::array<vk::AttachmentDescription, 2> attachmentsDescription = {colorAttachment, depthAttachment};

    renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentsDescription.size());
    renderPassCreateInfo.pAttachments = attachmentsDescription.data();

    vk::AttachmentReference colorAttachmentReference = {};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::AttachmentReference depthAttachmentReference = {};
    depthAttachmentReference.attachment = 1;
    depthAttachmentReference.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::SubpassDescription subpass = {};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentReference;
    subpass.pDepthStencilAttachment = &depthAttachmentReference;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;

    std::array<vk::SubpassDependency, 2> subpassDependencies;
//color
    subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[0].srcStageMask =
            vk::PipelineStageFlagBits::eBottomOfPipe | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    subpassDependencies[0].srcAccessMask = vk::AccessFlagBits::eMemoryRead;
    subpassDependencies[0].dstSubpass = 0;
    subpassDependencies[0].dstStageMask =
            vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    subpassDependencies[0].dstAccessMask = vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite;
    subpassDependencies[0].dependencyFlags = vk::DependencyFlagBits::eDeviceGroup;
//depth
    subpassDependencies[1].srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[1].dstSubpass = 0;
    subpassDependencies[1].srcStageMask =
            vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
    subpassDependencies[1].srcAccessMask = vk::AccessFlagBits::eNone;
    subpassDependencies[1].dstStageMask =
            vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
    subpassDependencies[1].dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    subpassDependencies[1].dependencyFlags = vk::DependencyFlagBits::eDeviceGroup;
    renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
    renderPassCreateInfo.pDependencies = subpassDependencies.data();


    renderPass = renderer.device.createRenderPass(renderPassCreateInfo);

    SE_RENDERER_DEBUG("RenderPass initialised and added to deletion queue");
}

void RenderPass::initFramebuffers() {
    SE_RENDERER_DEBUG("Frame buffers initialisation");
    createDepthBufferResources();

    Renderer &renderer = Renderer::Get();
    const Swapchain &swapchain = renderer.swapchain;
    std::vector<vk::Image> images = renderer.device.getSwapchainImagesKHR(swapchain.getSwapchain());

    imageViews.reserve(images.size());
    framebuffers.resize(images.size());

    for (size_t i = 0; i < images.size(); ++i) {

        imageViews.emplace_back(renderer.createImageView(images.at(i),
                                                         swapchain.getSwapchainImageFormat(),
                                                         vk::ImageAspectFlagBits::eColor));

        attachments = {imageViews.at(i), depthBufferImage.depthImageView};

        vk::FramebufferCreateInfo framebufferCreateInfo = {};
        framebufferCreateInfo.sType = vk::StructureType::eFramebufferCreateInfo;
        framebufferCreateInfo.renderPass = renderPass;
        framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferCreateInfo.pAttachments = attachments.data();
        framebufferCreateInfo.width = swapchain.getSwapchainExtent().width;
        framebufferCreateInfo.height = swapchain.getSwapchainExtent().height;
        framebufferCreateInfo.layers = 1;
        framebuffers.at(i) = renderer.device.createFramebuffer(framebufferCreateInfo);
    }
    SE_RENDERER_DEBUG("Frame buffers added to deletion queue");
}

void RenderPass::cleanUp() {
    Renderer::Get().device.destroyRenderPass(renderPass);

    for (auto &image: imageViews) {
        Renderer::Get().device.destroyImageView(image);
    }
    for (auto &framebuffer: framebuffers) {
        Renderer::Get().device.destroyFramebuffer(framebuffer);
    }
    Renderer::Get().device.destroyImageView(depthBufferImage.depthImageView);
    Renderer::Get().device.destroyImage(depthBufferImage.depthImage);
    Renderer::Get().device.freeMemory(depthBufferImage.depthImageMemory);
}

void RenderPass::begin(const uint32_t &frameIndex, const vk::CommandBuffer &cmd) {
    vk::RenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = vk::StructureType::eRenderPassBeginInfo;
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent = Renderer::Get().swapchain.getSwapchainExtent();
    renderPassBeginInfo.framebuffer = framebuffers.at(frameIndex);

    std::array<vk::ClearValue, 2> clearValues{};
    clearValues[0].color = {std::array<float, 4>{0.f, 0.f, .0f, .0f}};
    clearValues[1].depthStencil = {{1.f, 0}};

    renderPassBeginInfo.pClearValues = clearValues.data();
    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());

    cmd.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

}

void RenderPass::end(const vk::CommandBuffer &cmd) {
    cmd.endRenderPass();
}

vk::Format RenderPass::findDepthFormat() {
    return Renderer::Get().findSupportedFormat(
            {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
}

void RenderPass::createDepthBufferResources() {
    Renderer &renderer = Renderer::Get();
    vk::Format depthFormat = findDepthFormat();
    renderer.createImage(renderer.swapchain.getSwapchainExtent().width,
                         renderer.swapchain.getSwapchainExtent().height,
                         depthFormat,
                         vk::ImageTiling::eOptimal,
                         vk::ImageUsageFlagBits::eDepthStencilAttachment,
                         vk::MemoryPropertyFlagBits::eDeviceLocal,
                         depthBufferImage.depthImage,
                         depthBufferImage.depthImageMemory);
    depthBufferImage.depthImageView = renderer.createImageView(depthBufferImage.depthImage, depthFormat,
                                                               vk::ImageAspectFlagBits::eDepth);

}