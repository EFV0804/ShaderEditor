//
// Created by elise.vidal.
//

#pragma once

#include <vulkan/vulkan.hpp>
#include "Logger.h"

struct SwapchainDetails {
    vk::SurfaceCapabilitiesKHR surfaceCapabilities; /**< Store the surface's capabilities. */
    std::vector<vk::SurfaceFormatKHR> supportedFormats; /**< A vector of the surface's supported formats. */
    std::vector<vk::PresentModeKHR> supportedPresentationModes; /**< A vector the surface's supported presentation modes. */
};

class Swapchain {
private:
    vk::SwapchainKHR swapchain;
public:
    Swapchain() = default;

    ~Swapchain() = default;

    Swapchain(const Swapchain &) = delete;

    Swapchain &operator=(const Swapchain &) = delete;

    /*!
    * \brief initialises swapchain.
    */
    void init();

    void cleanUp();

    const vk::Extent2D &getSwapchainExtent() const { return swapchainExtent; }

    const vk::SwapchainKHR &getSwapchain() const { return swapchain; }

    const vk::Format getSwapchainImageFormat() const { return swapchainImageFormat; }

    const SwapchainDetails getSwapchainDetails() const { return swapchainDetails; }

    void setSwapchainDetails();

private:

    /**
     * The extent of the swapchain, a width and height.
     */
    vk::Extent2D swapchainExtent{1920, 1080};
    /**
    * The format used for the swapchain images. Defaults to 32bits unsigned normalised.
    */
    vk::Format swapchainImageFormat{vk::Format::eB8G8R8A8Unorm};

    SwapchainDetails swapchainDetails;

    /*!
    * \brief Sets the extent of the swapchain.
    *
    * Sets the extent of the swapchain by getting the width and height of the GLFW window.
    */
    void setSwapchainExtent(const vk::SurfaceCapabilitiesKHR &surfaceCapabilities);

    vk::PresentModeKHR selectPresentationMode(const std::vector<vk::PresentModeKHR> &presentationModes,
                                              vk::PresentModeKHR mode);

    vk::SurfaceFormatKHR getSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &formats);

};
