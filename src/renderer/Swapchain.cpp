//
// Created by elise.vidal.
//

#include "Swapchain.h"
#include "Renderer.h"

void Swapchain::init() {
    SE_RENDERER_DEBUG("Swapchain initialisation");
    Renderer &renderer = Renderer::Get();
    swapchainExtent.width = renderer.window.getWidth();
    swapchainExtent.height = renderer.window.getHeight();

    setSwapchainDetails();

    vk::SurfaceFormatKHR surfaceFormat = getSurfaceFormat(swapchainDetails.supportedFormats);
    vk::PresentModeKHR presentationMode = selectPresentationMode(swapchainDetails.supportedPresentationModes,
                                                                 vk::PresentModeKHR::eMailbox);
    setSwapchainExtent(swapchainDetails.surfaceCapabilities);

    vk::SwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType = vk::StructureType::eSwapchainCreateInfoKHR;
    swapchainCreateInfo.surface = renderer.surface;
    swapchainCreateInfo.imageFormat = surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCreateInfo.presentMode = presentationMode;
    swapchainCreateInfo.imageExtent = swapchainExtent;

    uint32_t imageCount = swapchainDetails.surfaceCapabilities.minImageCount + 1;
    if (swapchainDetails.surfaceCapabilities.maxImageCount > 0
        && swapchainDetails.surfaceCapabilities.maxImageCount < imageCount) {
        imageCount = swapchainDetails.surfaceCapabilities.maxImageCount;
    }
    swapchainCreateInfo.minImageCount = imageCount;

    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    swapchainCreateInfo.preTransform = swapchainDetails.surfaceCapabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    swapchainCreateInfo.clipped = VK_TRUE;

    if (renderer.queueFamilyIndices.graphicsFamily != renderer.queueFamilyIndices.presentationFamily) {
        uint32_t queueFamilyIndicesArr[]{renderer.queueFamilyIndices.graphicsFamily,
                                         renderer.queueFamilyIndices.presentationFamily};
        swapchainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndicesArr;
    } else {
        swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    }

    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    swapchain = renderer.device.createSwapchainKHR(swapchainCreateInfo);


    swapchainImageFormat = surfaceFormat.format;
}

void Swapchain::setSwapchainDetails() {
    SE_RENDERER_DEBUG("Setting Swapchain Details");
    Renderer &renderer = Renderer::Get();
    swapchainDetails.surfaceCapabilities = renderer.physicalDevice.getSurfaceCapabilitiesKHR(renderer.surface);
    swapchainDetails.supportedFormats = renderer.physicalDevice.getSurfaceFormatsKHR(renderer.surface);
    swapchainDetails.supportedPresentationModes = renderer.physicalDevice.getSurfacePresentModesKHR(renderer.surface);
}

void Swapchain::setSwapchainExtent(const vk::SurfaceCapabilitiesKHR &surfaceCapabilities) {
    SE_RENDERER_DEBUG("Setting Swapchain Extent");
    Renderer &renderer = Renderer::Get();
    if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        swapchainExtent = surfaceCapabilities.currentExtent;
    } else {
        VkExtent2D newExtent{};
        newExtent.width = static_cast<uint32_t>(renderer.window.getWidth());
        newExtent.height = static_cast<uint32_t>(renderer.window.getHeight());

        newExtent.width = std::max(surfaceCapabilities.minImageExtent.width,
                                   std::min(surfaceCapabilities.maxImageExtent.width, newExtent.width));
        newExtent.height = std::max(surfaceCapabilities.minImageExtent.height,
                                    std::min(surfaceCapabilities.maxImageExtent.height, newExtent.height));
        swapchainExtent = newExtent;
    }
}

vk::PresentModeKHR
Swapchain::selectPresentationMode(const std::vector<vk::PresentModeKHR> &presentationModes, vk::PresentModeKHR mode) {
    SE_RENDERER_DEBUG("Getting presentation mode");
    for (const auto &presentationMode: presentationModes) {
        if (presentationMode == mode) {
            return presentationMode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

vk::SurfaceFormatKHR Swapchain::getSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &formats) {
    SE_RENDERER_DEBUG("Getting Swapchain surface format");
    if (formats.size() == 1 && formats[0].format == vk::Format::eUndefined) {
        return {vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear};
    }
    for (auto &format: formats) {
        if (format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return format;
        }
    }

    return formats[0];
}

void Swapchain::cleanUp() {
    Renderer::Get().device.destroySwapchainKHR(swapchain);
}
