#include "Renderer.h"
#include <set>
#include <iostream>
#include <GLFW/glfw3.h>
#include <string>
#include "../Renderable.h"


int Renderer::init() {

    SD_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, !isInit, "Renderer is already initialised, cannot be initialised twice.")
    initWindow();
    try {
        initInstance();
        initSurface();
        initPhysicalDevice();
        setQueueFamilyIndices();
        initLogicalDevice();
        initQueues();
        initSwapchain();
        initRenderPass();
        initFramebuffers();
        initVertexBuffer();
        initCommandBuffers();
        createSynchronisation();
    }
    catch (const std::runtime_error &e) {
        SD_RENDERER_ERROR("Renderer failed to initialise: {}", e.what());
        return EXIT_FAILURE;
    }
    SD_RENDERER_INFO("Renderer initialised successfully");

    isInit = true;
    return EXIT_SUCCESS;

}

//// DRAW
void Renderer::draw(std::vector<Renderable> *renderables) {
    SD_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, isInit, "Renderer is not initialised, initialise before calling rendering functions")

//******************--- START NEW FRAME ---******************//
    vk::Result result = device.waitForFences(getCurrentFrame()->renderFence, VK_TRUE, 1000000000);
    SD_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, result == vk::Result::eSuccess,
                                "Device timed out waiting for previous frame.");
    device.resetFences(getCurrentFrame()->renderFence);
    getCurrentFrame()->commandBuffer.reset();

    uint32_t imageToBeDrawnIndex;

    vk::ResultValue r = device.acquireNextImageKHR(swapchain,
                                                   1000000000,
                                                   getCurrentFrame()->presentSemaphore,
                                                   VK_NULL_HANDLE);
    imageToBeDrawnIndex = r.value;

//***************--- BEGIN COMMAND BUFFER---**********µ*******//
    vk::CommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
    commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    getCurrentFrame()->commandBuffer.begin(commandBufferBeginInfo);

//***************--- RENDER PASS BEGINS ---**********µ*******//
    vk::RenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = vk::StructureType::eRenderPassBeginInfo;
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent = swapchainExtent;
    renderPassBeginInfo.framebuffer = swapchainFramebuffers.at(imageToBeDrawnIndex);

    const vk::ClearValue clearValues{
            std::array<float, 4>{0.f, 0.f, .4f, 1.0f}
    };

    renderPassBeginInfo.pClearValues = &clearValues;
    renderPassBeginInfo.clearValueCount = 1;

    getCurrentFrame()->commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

//******************--- DRAW OBJECTS ---********************//
    drawRenderables(renderables);

//******************--- RENDER PASS ENDS ---*****************//
    getCurrentFrame()->commandBuffer.endRenderPass();

//******************--- END COMMAND BUFFER ---**************//
    getCurrentFrame()->commandBuffer.end();

//******************--- SUBMIT INFO ---********************//
    vk::PipelineStageFlags waitStages[]{vk::PipelineStageFlagBits::eColorAttachmentOutput};

    vk::SubmitInfo submitInfo{};
    submitInfo.sType = vk::StructureType::eSubmitInfo;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &getCurrentFrame()->presentSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &getCurrentFrame()->commandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &getCurrentFrame()->renderSemaphore;

    graphicsQueue.submit(submitInfo, getCurrentFrame()->renderFence);

//******************--- PRESENTATION ---********************//
    vk::PresentInfoKHR presentInfo{};
    presentInfo.sType = vk::StructureType::ePresentInfoKHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &getCurrentFrame()->renderSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &imageToBeDrawnIndex;

    vk::Result presentationResult = graphicsQueue.presentKHR(presentInfo);
    SD_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, presentationResult == vk::Result::eSuccess,
                                "Graphics queue failed to present correctly.")

//************--- NEXT FRAME INCREMENTATION ---************//
    currentFrame = (currentFrame + 1) % MAX_FRAME_DRAWS;
}

void Renderer::drawRenderables(std::vector<Renderable> *renderables) {
    SD_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, isInit, "Renderer is not initialised, initialise before calling rendering functions")
    //TODO bind vertexBuffer here and add counter to multiply Vertex.size()*count to offset vertexBuffer binding

    const Material* lastMaterial = nullptr;

    for(int i = 0; i < renderables->size(); i++){
        const Material* current_mat = renderables->at(i).getMaterial();

        if(current_mat != lastMaterial){
//            vk::Pipeline& pipeline = renderable.material->pipeline.getPipeline();
            getCurrentFrame()->commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, current_mat->pipeline.getPipeline());
            lastMaterial = current_mat;
        }
        VkDeviceSize offset = 0;

        //Push constants
        for(auto constant : renderables->at(i).getMaterial()->getPushConstants()){
            getCurrentFrame()->commandBuffer.pushConstants(
                    renderables->at(i).getMaterial()->pipeline.getLayout(),
                    constant.stageFlags,
                    constant.offset,
                    constant.size,
                    &renderables->at(i).transform);
        }

        getCurrentFrame()->commandBuffer.bindVertexBuffers(0, 1, &vertexBuffer.getBuffer(), &offset);
        getCurrentFrame()->commandBuffer.draw(renderables->at(i).getMesh()->vertices.size(), 1, 0,0);

    }
}

void Renderer::initWindow() {
    SD_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, !isInit, "Renderer is not initialised, initialise renderer before window initialisation.")
    //TODO Abstract GLFW code into a window class to allow easy replacement of GLFW if needed.
    SD_RENDERER_DEBUG("Window initialisation");
    std::string name = "The Best Window";
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(800, 600, name.c_str(), nullptr, nullptr);

    if (window == nullptr) {
        SD_RENDERER_ERROR("Window failed to initialise");
    }
    SD_RENDERER_DEBUG("Window initialised successfully");
}

void Renderer::initInstance() {
    SD_RENDERER_DEBUG("Instance initialisation");
    vk::ApplicationInfo AppInfo{
            "ShaderEditor",
            0,
            nullptr,
            0,
            VK_API_VERSION_1_1
    };
    std::vector<const char *> layers;
    if (VERBOSITY < spdlog::level::err) {
        layers = {"VK_LAYER_KHRONOS_validation", "VK_LAYER_LUNARG_api_dump",
                  "VK_LAYER_LUNARG_monitor"}; // VK_LAYER_KHRONOS_validation = debug help
    } else {
        layers = {"VK_LAYER_LUNARG_monitor"};
    }

    std::vector<const char *> instanceExtensions;
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    instanceExtensions.reserve(glfwExtensionCount + 1);

    for (size_t i = 0; i < glfwExtensionCount; ++i) {
        instanceExtensions.emplace_back(glfwExtensions[i]);
    }
    instanceExtensions.emplace_back("VK_EXT_debug_utils");


    vk::InstanceCreateInfo instanceCreateInfo(vk::InstanceCreateFlags(),
                                              &AppInfo,
                                              layers.size(),
                                              layers.data());

    if (!checkInstanceExtensionSupport(instanceExtensions)) {
        SD_RENDERER_ERROR("Instance doesn't support extension required");
//        throw std::runtime_error("");
    }
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();

    instance = vk::createInstance(instanceCreateInfo);
////    instance.get
//    mainDeletionQueue.push_function([=](){
//       instance.destroy();
//    });
    SD_RENDERER_DEBUG("Vulkan instance initialised");
}

void Renderer::initLogicalDevice() {

//    VK_KHR_SWAPCHAIN_EXTENSION_NAME

    SD_RENDERER_DEBUG("Logical Device initialisation");
    float queuePriority = 1.0f;
    std::vector<vk::DeviceQueueCreateInfo> queuesCreateInfos;
    std::set<uint32_t> indices{queueFamilyIndices.computeFamily, queueFamilyIndices.graphicsFamily};
    queuesCreateInfos.reserve(indices.size());

    for (auto index: indices) {
        queuesCreateInfos.emplace_back(vk::DeviceQueueCreateFlags(),
                                       index,
                                       1,
                                       &queuePriority,
                                       nullptr);
        queuesCreateInfos.back().sType = vk::StructureType::eDeviceQueueCreateInfo;
    }

    std::vector<const char *> deviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};    // Allows swapchain creation
    vk::PhysicalDeviceFeatures deviceFeatures = {}; //set to false by default
    vk::DeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.flags = vk::DeviceCreateFlags();
    deviceCreateInfo.queueCreateInfoCount = queuesCreateInfos.size();
    deviceCreateInfo.pQueueCreateInfos = queuesCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    device = physicalDevice.createDevice(deviceCreateInfo);

    mainDeletionQueue.push_function([=]() {
        device.destroy();
    });
}

void Renderer::initQueues() {
    graphicsQueue = device.getQueue(queueFamilyIndices.graphicsFamily, 0);
    computeQueue = device.getQueue(queueFamilyIndices.computeFamily, 0);
    presentationQueue = device.getQueue(queueFamilyIndices.presentationFamily, 0);
}

void Renderer::initSwapchain() {
    SD_RENDERER_DEBUG("Swapchain initialisation");

    SwapchainDetails swapchainDetails = getSwapchainDetails(physicalDevice);
    vk::SurfaceFormatKHR surfaceFormat = getSurfaceFormat(swapchainDetails.supportedFormats);
    vk::PresentModeKHR presentationMode = selectPresentationMode(swapchainDetails.supportedPresentationModes,
                                                                 vk::PresentModeKHR::eMailbox);
    setSwapchainExtent(swapchainDetails.surfaceCapabilities);

    vk::SwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType = vk::StructureType::eSwapchainCreateInfoKHR;
    swapchainCreateInfo.surface = surface;
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

    if (queueFamilyIndices.graphicsFamily != queueFamilyIndices.presentationFamily) {
        uint32_t queueFamilyIndicesArr[]{queueFamilyIndices.graphicsFamily,
                                         queueFamilyIndices.presentationFamily};
        swapchainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndicesArr;
    } else {
        swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    }

    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    swapchain = device.createSwapchainKHR(swapchainCreateInfo);
    mainDeletionQueue.push_function([=]() {
        device.destroySwapchainKHR(swapchain);
    });

    swapchainImageFormat = surfaceFormat.format;


    /// CREATE IMAGES FOR SWAPCHAIN
    /// These are destroyed by the destroy call on the swapchain, no need to explicitly destroy them
    std::vector<vk::Image> images{device.getSwapchainImagesKHR(swapchain)};
    swapchainImages.reserve(images.size());

    for (vk::Image image: images) {
        swapchainImages.emplace_back();
        swapchainImages.back().image = image;
        swapchainImages.back().imageView = createImageView(image,
                                                           swapchainImageFormat,
                                                           vk::ImageAspectFlagBits::eColor);
    }


}

void Renderer::initCommandBuffers() {

    SD_RENDERER_DEBUG("Command Buffer initialisation");

    vk::CommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

    frames.reserve(MAX_FRAME_DRAWS);

    for (int i = 0; i < MAX_FRAME_DRAWS; i++) {
        frames.emplace_back();
        frames.back().commandPool = device.createCommandPool(poolInfo);
        vk::CommandBufferAllocateInfo commandBufferAllocInfo = {};
        commandBufferAllocInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
        commandBufferAllocInfo.commandPool = frames.back().commandPool;
        commandBufferAllocInfo.commandBufferCount = 1;
        commandBufferAllocInfo.level = vk::CommandBufferLevel::ePrimary;
        frames.back().commandBuffer = device.allocateCommandBuffers(commandBufferAllocInfo).at(0);
        mainDeletionQueue.push_function([=]() {
            device.destroyCommandPool(frames[i].commandPool);
        });

    }
}

void Renderer::initRenderPass() {
    SD_RENDERER_DEBUG("RenderPass initialisation");

    vk::RenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = vk::StructureType::eRenderPassCreateInfo;
    renderPassCreateInfo.pNext = nullptr;
    renderPassCreateInfo.flags = vk::RenderPassCreateFlags();


    vk::AttachmentDescription colorAttachment{};
    colorAttachment.format = swapchainImageFormat;
    colorAttachment.samples = vk::SampleCountFlagBits::e1;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = &colorAttachment;

    vk::AttachmentReference colorAttachmentReference = {};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::SubpassDescription subpass = {};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentReference;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;

    std::array<vk::SubpassDependency, 2> subpassDependencies;

    subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[0].srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
    subpassDependencies[0].srcAccessMask = vk::AccessFlagBits::eMemoryRead;
    subpassDependencies[0].dstSubpass = 0;
    subpassDependencies[0].dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependencies[0].dstAccessMask = vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite;
    subpassDependencies[0].dependencyFlags = vk::DependencyFlagBits::eDeviceGroup;
    subpassDependencies[1].srcSubpass = 0;
    subpassDependencies[1].srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependencies[1].srcAccessMask = vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite;
    subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[1].dstStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
    subpassDependencies[1].dstAccessMask = vk::AccessFlagBits::eMemoryRead;
    subpassDependencies[1].dependencyFlags = vk::DependencyFlagBits::eDeviceGroup;
    renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
    renderPassCreateInfo.pDependencies = subpassDependencies.data();


    renderPass = device.createRenderPass(renderPassCreateInfo);

    mainDeletionQueue.push_function([=]() {
        device.destroyRenderPass(renderPass);
    });
    SD_RENDERER_DEBUG("RenderPass initialised and added to deletion queue");
}

void Renderer::initFramebuffers() {

    SD_RENDERER_DEBUG("Frame buffers initialisation");
    swapchainFramebuffers.resize(swapchainImages.size());

    for (size_t i = 0; i < swapchainImages.size(); ++i) {
        std::array<vk::ImageView, 1> attachments = {swapchainImages[i].imageView};
        vk::FramebufferCreateInfo framebufferCreateInfo = {};
        framebufferCreateInfo.sType = vk::StructureType::eFramebufferCreateInfo;
        framebufferCreateInfo.renderPass = renderPass;
        framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferCreateInfo.pAttachments = attachments.data();
        framebufferCreateInfo.width = swapchainExtent.width;
        framebufferCreateInfo.height = swapchainExtent.height;
        framebufferCreateInfo.layers = 1;
        swapchainFramebuffers[i] = device.createFramebuffer(framebufferCreateInfo);

        mainDeletionQueue.push_function([=]() {
            device.destroyFramebuffer(swapchainFramebuffers[i]);
        });
    }
    SD_RENDERER_DEBUG("Frame buffers added to deletion queue");

}

void Renderer::createSynchronisation() {
    SD_RENDERER_DEBUG("Semaphore and Fence initialisation");

    vk::FenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = vk::StructureType::eFenceCreateInfo;
    fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;
    fenceCreateInfo.pNext = nullptr;

    vk::SemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = vk::StructureType::eSemaphoreCreateInfo;
    semaphoreCreateInfo.pNext = nullptr;

    for (int i = 0; i < MAX_FRAME_DRAWS; i++) {
        frames[i].renderFence = device.createFence(fenceCreateInfo);
        frames[i].presentSemaphore = device.createSemaphore(semaphoreCreateInfo);
        frames[i].renderSemaphore = device.createSemaphore(semaphoreCreateInfo);

        mainDeletionQueue.push_function([=]() {
            device.destroyFence(frames[i].renderFence);
            device.destroySemaphore(frames[i].renderSemaphore);
            device.destroySemaphore(frames[i].presentSemaphore);
        });
    }
    SD_RENDERER_DEBUG("Fences and Semaphores added to deletion queue");

}

Renderer::SwapchainDetails Renderer::getSwapchainDetails(vk::PhysicalDevice pPhysicalDevice) const {
    SD_RENDERER_DEBUG("Getting Swapchain Details");
    SwapchainDetails swapchainDetails;
    swapchainDetails.surfaceCapabilities = pPhysicalDevice.getSurfaceCapabilitiesKHR(surface);
    swapchainDetails.supportedFormats = pPhysicalDevice.getSurfaceFormatsKHR(surface);
    swapchainDetails.supportedPresentationModes = pPhysicalDevice.getSurfacePresentModesKHR(surface);
    return swapchainDetails;
}

vk::SurfaceFormatKHR Renderer::getSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &formats) {
    SD_RENDERER_DEBUG("Getting Surface Format");
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

vk::PresentModeKHR
Renderer::selectPresentationMode(const std::vector<vk::PresentModeKHR> &presentationModes, vk::PresentModeKHR mode) {
    SD_RENDERER_DEBUG("Getting presentation mode");
    for (const auto &presentationMode: presentationModes) {
        if (presentationMode == mode) {
            return presentationMode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

void Renderer::setQueueFamilyIndices() {

    SD_RENDERER_DEBUG("Queues initialisation");
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties{physicalDevice.getQueueFamilyProperties()};

    auto computePropertiesIterator = std::find_if(queueFamilyProperties.begin(),
                                                  queueFamilyProperties.end(),
                                                  [&](const vk::QueueFamilyProperties &properties) {
                                                      return properties.queueFlags & vk::QueueFlagBits::eCompute;
                                                  });

    queueFamilyIndices.computeFamily = std::distance(queueFamilyProperties.begin(), computePropertiesIterator);

    auto graphicsPropertiesIterator = std::find_if(queueFamilyProperties.begin(),
                                                   queueFamilyProperties.end(),
                                                   [&](const vk::QueueFamilyProperties &properties) {
                                                       return properties.queueFlags & vk::QueueFlagBits::eGraphics;
                                                   });

    uint32_t temp_index = std::distance(queueFamilyProperties.begin(), graphicsPropertiesIterator);
    vk::Bool32 presentationSupport = false;
    presentationSupport = physicalDevice.getSurfaceSupportKHR(temp_index, surface);

    SD_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, temp_index >= 0 && presentationSupport,
                                "Graphics and Presentation queue initialisation failed");
    queueFamilyIndices.graphicsFamily = temp_index;
    queueFamilyIndices.presentationFamily = temp_index;

}

void Renderer::initPhysicalDevice() {
    SD_RENDERER_DEBUG("Physical Device initialisation");
    std::vector<vk::PhysicalDevice> physicalDevices{instance.enumeratePhysicalDevices()};

    SD_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, !physicalDevices.empty(), "No GPU with vulkan support found")

    for (auto device: physicalDevices) {
        if (checkDeviceSuitable(device)) {
            physicalDevice = device;
            physicalDeviceMemoryProperties = physicalDevice.getMemoryProperties();

        }
    }

}

void Renderer::initSurface() {
    SD_RENDERER_DEBUG("Surface initialisation");
    VkSurfaceKHR surface_temp;

    if (glfwCreateWindowSurface(instance,
                                window,
                                nullptr,
                                &surface_temp) != VK_SUCCESS) {
        throw std::runtime_error("Surface failed to initialise");
    }
    surface = surface_temp; //c++ wrapper type conversion: vk::SurfaceKHR contains VkSurfaceKHR address
//    instance.destroySurfaceKHR(surface_temp);

    mainDeletionQueue.push_function([=]() {
        instance.destroySurfaceKHR(surface);
    });


}

void Renderer::setSwapchainExtent(const vk::SurfaceCapabilitiesKHR &surfaceCapabilities) {
    SD_RENDERER_DEBUG("Setting Swapchain Extent");
    if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        swapchainExtent = surfaceCapabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        VkExtent2D newExtent{};
        newExtent.width = static_cast<uint32_t>(width);
        newExtent.height = static_cast<uint32_t>(height);

        newExtent.width = std::max(surfaceCapabilities.minImageExtent.width,
                                   std::min(surfaceCapabilities.maxImageExtent.width, newExtent.width));
        newExtent.height = std::max(surfaceCapabilities.minImageExtent.height,
                                    std::min(surfaceCapabilities.maxImageExtent.height, newExtent.height));
        swapchainExtent = newExtent;
    }
}

bool Renderer::checkInstanceExtensionSupport(const std::vector<const char *> &checkExtensions) {
    SD_RENDERER_DEBUG("Checking extension support");
    std::vector<vk::ExtensionProperties> extensions = vk::enumerateInstanceExtensionProperties();

    for (const auto &checkExtension: checkExtensions) {
        bool hasExtension = false;
        for (const auto &extension: extensions) {
            if (strcmp(checkExtension, extension.extensionName) == 0) {
                hasExtension = true;
                break;
            }
        }
        if (!hasExtension) {
            SD_RENDERER_ERROR("Required extension is not supported: {0}", checkExtension);
            return false;
        }
    }
    return true;
}

bool Renderer::checkDeviceExtensionSupport(vk::PhysicalDevice pPhysicalDevice) {
//    vector<vk::ExtensionProperties> extensions = pPhysicalDevice.enumerateDeviceExtensionProperties(nullptr);
//
//    for (auto deviceextension : deviceExtensions)
//    {
//        bool hasextension = false;
//        for (const auto& extension : extensions)
//        {
//            if (strcmp(deviceextension, extension.extensionName) == 0)
//            {
//                hasextension = true;
//                break;
//            }
//        }
//        if (!hasextension) return false;
//    }
    return true;
}

bool Renderer::checkDeviceSuitable(vk::PhysicalDevice pPhysicalDevice) {
//    vk::PhysicalDeviceProperties physicalDeviceProperties = physicalDevice.getProperties();
//    vk::PhysicalDeviceFeatures physicalDeviceFeatures = physicalDevice.getFeatures();

    bool extensionsSupported = checkDeviceExtensionSupport(pPhysicalDevice);

    bool swapchainValid = false;

    if (extensionsSupported) {
        SwapchainDetails swapchainDetails = getSwapchainDetails(pPhysicalDevice);
        swapchainValid =
                !swapchainDetails.supportedPresentationModes.empty() && !swapchainDetails.supportedFormats.empty();
    }

    return queueFamilyIndices.isValid() && extensionsSupported && swapchainValid;
}

vk::ImageView Renderer::createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags) {

    vk::ImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType = vk::StructureType::eImageViewCreateInfo;
    imageViewCreateInfo.image = image;
    imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
    imageViewCreateInfo.format = format;
    imageViewCreateInfo.components.r = vk::ComponentSwizzle::eIdentity;
    imageViewCreateInfo.components.g = vk::ComponentSwizzle::eIdentity;
    imageViewCreateInfo.components.b = vk::ComponentSwizzle::eIdentity;
    imageViewCreateInfo.components.a = vk::ComponentSwizzle::eIdentity;
    imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    vk::ImageView imageView;
    imageView = device.createImageView(imageViewCreateInfo);

    mainDeletionQueue.push_function([=]() {
        device.destroy(imageView);
    });

    return imageView;
}

void Renderer::cleanUp() {
    device.waitIdle();
    mainDeletionQueue.flush();

    glfwDestroyWindow(window);
    glfwTerminate();

    instance.destroy();
    SD_RENDERER_INFO("Renderer clean up successful");

}

void Renderer::loadMeshes(std::vector<Renderable> *renderables) {
    SD_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, isInit, "Renderer is not initialised.")
//    //TODO make sure vertexBuffer is not being overwritten by each renderable: add offset?

    for(int i = 0; i < renderables->size(); i++){
        vertexBuffer.map(0, renderables->at(i).getMesh()->getSize());
        vertexBuffer.copy(renderables->at(i).getMesh()->vertices.data(), renderables->at(i).getMesh()->getSize());
        vertexBuffer.unMap();
    }
}

void Renderer::initVertexBuffer() {

    SD_RENDERER_DEBUG("Initialising vertex buffer.");
    uint32_t memoryTypeIndex = getMemoryTypeIndex();
    vertexBuffer.init(queueFamilyIndices.graphicsFamily);
    mainDeletionQueue.push_function([=]() {vertexBuffer.destroy();});

    vertexBuffer.allocate(memoryTypeIndex);
    vertexBuffer.bind();

}

uint32_t Renderer::getMemoryTypeIndex() {

    vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();

    uint32_t memoryTypeIndex = uint32_t(~0);
    // TODO make sure it's big enough for buffer requested size


    // TODO add MemoryPropertyFlags as parameters. Use switch case to determine, based on passed buffer usage?
    for (uint32_t currentMemoryTypeIndex = 0;
         currentMemoryTypeIndex < memoryProperties.memoryTypeCount; ++currentMemoryTypeIndex) {
        vk::MemoryType memoryType = memoryProperties.memoryTypes[currentMemoryTypeIndex];
        if ((vk::MemoryPropertyFlagBits::eHostVisible & memoryType.propertyFlags) &&
            (vk::MemoryPropertyFlagBits::eHostCoherent & memoryType.propertyFlags)) {
            return currentMemoryTypeIndex;
        }
    }
    return memoryTypeIndex;
}
