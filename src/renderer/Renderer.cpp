#include "Renderer.h"
#include <set>
#include <iostream>
#include <GLFW/glfw3.h>
#include <string>
#include "../Renderable.h"

Renderer::Renderer(): vertexBuffer(vk::BufferUsageFlagBits::eVertexBuffer, 5000000)
{

}

Renderer::~Renderer()
{
}

int Renderer::init()
{

    createWindow();
    try
    {
        createInstance();
        setSurface();
        setPhysicalDevice();
        setQueueFamilyIndices();
        createLogicalDevice();
        createQueues();
        createSwapchain();
        createRenderPass();
        createFramebuffers();
        createVertexBuffer();
        createCommandBuffers();
//        createSyncStructures();
        createSynchronisation();

    }
    catch (const std::runtime_error& e)
    {
        printf("ERROR: %s\n", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;

    
}
// DRAW
void Renderer::draw(std::vector<Renderable>* renderables)
{
    // BEGIN FRAME
    device.waitForFences(getCurrentFrame().renderFence, VK_TRUE, 1000000000);
    device.resetFences(getCurrentFrame().renderFence);
    getCurrentFrame().commandBuffer.reset();

    uint32_t imageToBeDrawnIndex;

    vk::ResultValue result = device.acquireNextImageKHR(swapchain,
                                                        1000000000,
                                                        getCurrentFrame().presentSemaphore,
                                                        VK_NULL_HANDLE);
    imageToBeDrawnIndex = result.value;

    // BEGIN COMMAND INFO
    vk::CommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
    commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    getCurrentFrame().commandBuffer.begin(commandBufferBeginInfo);

    // RENDERPASS BEGIN
    vk::RenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = vk::StructureType::eRenderPassBeginInfo;
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent = swapchainExtent;
    renderPassBeginInfo.framebuffer = swapchainFramebuffers.at(imageToBeDrawnIndex);

    const vk::ClearValue clearValues{
            std::array<float,4>{0.f, 0.f, .4f, 1.0f}
    };

    renderPassBeginInfo.pClearValues = &clearValues;
    renderPassBeginInfo.clearValueCount = 1;

    getCurrentFrame().commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

    // DRAW OBJECTS
    drawRenderables(renderables);
    // RENDERPASS END
    getCurrentFrame().commandBuffer.endRenderPass();
    // ADD COMMAND BUFFER
    getCurrentFrame().commandBuffer.end();

    // SUBMIT INFO
    vk::PipelineStageFlags waitStages[]{ vk::PipelineStageFlagBits::eColorAttachmentOutput };

    vk::SubmitInfo submitInfo{};
    submitInfo.sType = vk::StructureType::eSubmitInfo;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &getCurrentFrame().presentSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &getCurrentFrame().commandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &getCurrentFrame().renderSemaphore;

    graphicsQueue.submit(submitInfo, getCurrentFrame().renderFence);


    // PRESENTATION
    vk::PresentInfoKHR presentInfo{};
    presentInfo.sType = vk::StructureType::ePresentInfoKHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores =  &getCurrentFrame().renderSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &imageToBeDrawnIndex;

    graphicsQueue.presentKHR(presentInfo);

    //NEXT FRAME INCREMENT
    currentFrame = (currentFrame + 1) % MAX_FRAME_DRAWS;
}

void Renderer::drawRenderables(std::vector<Renderable>* renderables){
    //TODO bind vertexBuffer here and add counter to multiply Vertex.size()*count to offset vertexBuffer binding

    Material* lastMaterial = nullptr;

    for(auto renderable : *renderables){
        if(&renderable.material != lastMaterial){
//            vk::Pipeline& pipeline = renderable.material->pipeline.getPipeline();
            getCurrentFrame().commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, renderable.material.pipeline.getPipeline());
            lastMaterial = &renderable.material;
        }
        VkDeviceSize offset = 0;
        getCurrentFrame().commandBuffer.bindVertexBuffers(0, 1, &vertexBuffer.getBuffer(), &offset);
        getCurrentFrame().commandBuffer.draw(renderable.mesh.vertices.size(), 1, 0,0);

    }

}
// INIT
void Renderer::createWindow() {
    std::string name = "The Best Window";
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(800, 600, name.c_str(), nullptr, nullptr);
}

void Renderer::createInstance()
{
    vk::ApplicationInfo AppInfo{
    "ShaderEditor",
    0,
    nullptr,
    0,
    VK_API_VERSION_1_1
    };

    const vector<const char*> layers = { "VK_LAYER_KHRONOS_validation" }; // VK_LAYER_KHRONOS_validation = debug help

    vector<const char*> instanceExtensions;
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    for (size_t i = 0; i < glfwExtensionCount; ++i)
    {
        instanceExtensions.push_back(glfwExtensions[i]);
    }

    if (!checkInstanceExtensionSupport(instanceExtensions))
    {
        throw std::runtime_error("VkInstance does not support required extensions, which is a shame, isn't it?");
    }

    vk::InstanceCreateInfo instanceCreateInfo(vk::InstanceCreateFlags(),
        &AppInfo,
        layers.size(),
        layers.data());

    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();

    instance = vk::createInstance(instanceCreateInfo);
}

void Renderer::createLogicalDevice()
{
    float queuePriority = 1.0f;
    vector<vk::DeviceQueueCreateInfo> queuesCreateInfos;

    std::set<uint32_t> indices = { queueFamilyIndices.computeFamily, queueFamilyIndices.graphicsFamily };

    for (auto index : indices) {
        vk::DeviceQueueCreateInfo deviceComputeQueueCreateInfo{};
        deviceComputeQueueCreateInfo.sType = vk::StructureType::eDeviceQueueCreateInfo;
        deviceComputeQueueCreateInfo.flags = vk::DeviceQueueCreateFlags();
        deviceComputeQueueCreateInfo.pNext = nullptr;
        deviceComputeQueueCreateInfo.queueCount = 1;
        deviceComputeQueueCreateInfo.queueFamilyIndex = index;
        deviceComputeQueueCreateInfo.pQueuePriorities = &queuePriority;
        queuesCreateInfos.push_back(deviceComputeQueueCreateInfo);
    }

    vk::PhysicalDeviceFeatures deviceFeatures{};
    vk::DeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.flags = vk::DeviceCreateFlags();
    deviceCreateInfo.queueCreateInfoCount = queuesCreateInfos.size();
    deviceCreateInfo.pQueueCreateInfos = queuesCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    device = physicalDevice.createDevice(deviceCreateInfo);
}

void Renderer::createQueues()
{
    graphicsQueue = device.getQueue(queueFamilyIndices.graphicsFamily, 0);
    computeQueue = device.getQueue(queueFamilyIndices.computeFamily, 0);
    presentationQueue = device.getQueue(queueFamilyIndices.presentationFamily, 0);
}

void Renderer::createSwapchain()
{
    SwapchainDetails swapchainDetails = getSwapchainDetails(physicalDevice);
    vk::SurfaceFormatKHR surfaceFormat = getSurfaceFormat(swapchainDetails.formats);
    vk::PresentModeKHR presentationMode = getPresentationMode(swapchainDetails.presentationModes);
    setSwapchainExtent(swapchainDetails.surfaceCapabilities);

    vk::SwapchainCreateInfoKHR swapchainCreateInfo{};
    swapchainCreateInfo.sType = vk::StructureType::eSwapchainCreateInfoKHR;
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.imageFormat = surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCreateInfo.presentMode = presentationMode;
    swapchainCreateInfo.imageExtent = swapchainExtent;

    uint32_t imageCount = swapchainDetails.surfaceCapabilities.minImageCount + 1;
    if (swapchainDetails.surfaceCapabilities.maxImageCount > 0
        && swapchainDetails.surfaceCapabilities.maxImageCount < imageCount)
    {
        imageCount = swapchainDetails.surfaceCapabilities.maxImageCount;
    }
    swapchainCreateInfo.minImageCount = imageCount;

    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    swapchainCreateInfo.preTransform = swapchainDetails.surfaceCapabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    swapchainCreateInfo.clipped = VK_TRUE;


    if (queueFamilyIndices.graphicsFamily != queueFamilyIndices.presentationFamily)
    {
        uint32_t queueFamilyIndicesArr[]{ queueFamilyIndices.graphicsFamily,
                                          queueFamilyIndices.presentationFamily };
        swapchainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndicesArr;
    }
    else
    {
        swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    }

    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    swapchain = device.createSwapchainKHR(swapchainCreateInfo);

    swapchainImageFormat = surfaceFormat.format;

    vector<vk::Image> images = device.getSwapchainImagesKHR(swapchain);
    for (vk::Image image : images)
    {
        SwapchainImage swapchainImage{};
        swapchainImage.image = image;
        swapchainImage.imageView = createImageView(image, swapchainImageFormat, vk::ImageAspectFlagBits::eColor);
        swapchainImages.push_back(swapchainImage);
    }
}

void Renderer::createCommandBuffers()
{

    vk::CommandPoolCreateInfo poolInfo{};
    poolInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

    for(int i = 0; i < MAX_FRAME_DRAWS; i++){
        FrameData frame;
        frame.commandPool = device.createCommandPool(poolInfo);
        vk::CommandBufferAllocateInfo commandBufferAllocInfo{};
        commandBufferAllocInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
        commandBufferAllocInfo.commandPool = frame.commandPool;
        commandBufferAllocInfo.commandBufferCount = 1;
        commandBufferAllocInfo.level = vk::CommandBufferLevel::ePrimary;

        frame.commandBuffer = device.allocateCommandBuffers(commandBufferAllocInfo).at(0);
        frames.emplace_back(frame);
    }
}

void Renderer::createRenderPass()
{
    vk::RenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = vk::StructureType::eRenderPassCreateInfo;

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

    vk::AttachmentReference colorAttachmentReference{};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::SubpassDescription subpass{};
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
    mainDeletionQueue.push_function([=](){
       device.destroyRenderPass(renderPass);
    });

}

void Renderer::createFramebuffers()
{
    swapchainFramebuffers.resize(swapchainImages.size());
    for (size_t i = 0; i < swapchainFramebuffers.size(); ++i)
    {
        std::array<vk::ImageView, 1> attachments{ swapchainImages[i].imageView };
        vk::FramebufferCreateInfo framebufferCreateInfo{};
        framebufferCreateInfo.sType = vk::StructureType::eFramebufferCreateInfo;
        framebufferCreateInfo.renderPass = renderPass;
        framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferCreateInfo.pAttachments = attachments.data();
        framebufferCreateInfo.width = swapchainExtent.width;
        framebufferCreateInfo.height = swapchainExtent.height;
        framebufferCreateInfo.layers = 1;

        swapchainFramebuffers[i] = device.createFramebuffer(framebufferCreateInfo);
        mainDeletionQueue.push_function([=](){
            device.destroyFramebuffer(swapchainFramebuffers[i]);
        });
    }
}

void Renderer::createSynchronisation()
{
    vk::FenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = vk::StructureType::eFenceCreateInfo;
    fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;
    fenceCreateInfo.pNext = nullptr;

    vk::SemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = vk::StructureType::eSemaphoreCreateInfo;
    semaphoreCreateInfo.pNext = nullptr;

    for(auto frame : frames){
        frame.renderFence = device.createFence(fenceCreateInfo);
        frame.presentSemaphore = device.createSemaphore(semaphoreCreateInfo);
        frame.renderSemaphore = device.createSemaphore(semaphoreCreateInfo);

        mainDeletionQueue.push_function([=](){device.destroyFence(frame.renderFence);
            device.destroySemaphore(frame.renderSemaphore);
            device.destroySemaphore(frame.presentSemaphore);});

    }
//    semaphores.imageAvailable.resize(MAX_FRAME_DRAWS);
//    semaphores.renderFinished.resize(MAX_FRAME_DRAWS);
//    drawFences.resize(MAX_FRAME_DRAWS);
//
//    vk::SemaphoreCreateInfo semaphoreCreateInfo{};
//    semaphoreCreateInfo.sType = vk::StructureType::eSemaphoreCreateInfo;
//
//    vk::FenceCreateInfo fenceCreateInfo{};
//    fenceCreateInfo.sType = vk::StructureType::eFenceCreateInfo;
//    fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;
//
//    for (size_t i = 0; i < MAX_FRAME_DRAWS; ++i)
//    {
//        semaphores.imageAvailable[i] = device.createSemaphore(semaphoreCreateInfo);
//        semaphores.renderFinished[i] = device.createSemaphore(semaphoreCreateInfo);
//        drawFences[i] = device.createFence(fenceCreateInfo);
//    }


}


// GETTERS
SwapchainDetails Renderer::getSwapchainDetails(vk::PhysicalDevice pPhysicalDevice)
{
    SwapchainDetails swapchainDetails;
    swapchainDetails.surfaceCapabilities = pPhysicalDevice.getSurfaceCapabilitiesKHR(surface);
    swapchainDetails.formats = pPhysicalDevice.getSurfaceFormatsKHR(surface);
    swapchainDetails.presentationModes = pPhysicalDevice.getSurfacePresentModesKHR(surface);
    return swapchainDetails;
}

vk::SurfaceFormatKHR Renderer::getSurfaceFormat(const vector<vk::SurfaceFormatKHR>& formats)
{
    if (formats.size() == 1 && formats[0].format == vk::Format::eUndefined)
    {
        return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
    }
    for (auto& format : formats)
    {
        if (format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            return format;
        }
    }

    return formats[0];
}

vk::PresentModeKHR Renderer::getPresentationMode(const vector<vk::PresentModeKHR>& presentationModes)
{
    for (const auto& presentationMode : presentationModes)
    {
        if (presentationMode == vk::PresentModeKHR::eMailbox)
        {
            return presentationMode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

// SETTERS
void Renderer::setQueueFamilyIndices()
{

    vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

    auto computePropertiesIterator = std::find_if(queueFamilyProperties.begin(),
                                                  queueFamilyProperties.end(),
                                                  [&](const vk::QueueFamilyProperties& properties)
                                                  {
                                                      return properties.queueFlags & vk::QueueFlagBits::eCompute;
                                                  });

    queueFamilyIndices.computeFamily = std::distance(queueFamilyProperties.begin(), computePropertiesIterator);

    auto graphicsPropertiesIterator = std::find_if(queueFamilyProperties.begin(),
                                                   queueFamilyProperties.end(),
                                                   [&](const vk::QueueFamilyProperties& properties)
                                                   {
                                                       return properties.queueFlags & vk::QueueFlagBits::eGraphics;
                                                   });

    uint32_t temp_index = std::distance(queueFamilyProperties.begin(), graphicsPropertiesIterator);
    vk::Bool32 presentationSupport = false;
    presentationSupport = physicalDevice.getSurfaceSupportKHR(temp_index, surface);
    if (temp_index >= 0 && presentationSupport)
    {
        queueFamilyIndices.presentationFamily = temp_index;
        queueFamilyIndices.graphicsFamily = temp_index;
    }

//    queueFamilyIndices.graphicsFamily = std::distance(queueFamilyProperties.begin(), graphicsPropertiesIterator);

}

void Renderer::setPhysicalDevice()
{
    std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();


    if (physicalDevices.size() == 0)
    {
        throw std::runtime_error("Can't find any GPU that supports vulkan, and what are you gonna do about it?");
    }
    for (auto device : physicalDevices)
    {
        if (checkDeviceSuitable(device))
        {
            physicalDevice = device;
            physicalDeviceMemoryProperties = physicalDevice.getMemoryProperties();

        }
    }

}

void Renderer::setSurface()
{
    VkSurfaceKHR surface_temp;
    VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface_temp);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create a vulkan surface.");
    }
    surface = surface_temp; //c++ wrapper type conversion: vk::SurfaceKHR contains VkSurfaceKHR adress
}

void Renderer::setSwapchainExtent(const vk::SurfaceCapabilitiesKHR& surfaceCapabilities)
{
    if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        swapchainExtent = surfaceCapabilities.currentExtent;
    }
    else
    {
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

// CHECKS
bool Renderer::checkInstanceExtensionSupport(const vector<const char*>& checkExtensions)
{
    vector<vk::ExtensionProperties> extensions = vk::enumerateInstanceExtensionProperties();

    for (const auto& checkExtension : checkExtensions)
    {
        bool hasExtension = false;
        for (const auto& extension : extensions)
        {
            if (strcmp(checkExtension, extension.extensionName) == 0)
            {
                hasExtension = true;
                break;
            }
        }
        if (!hasExtension) return false;
    }
    return true;
}

bool Renderer::checkDeviceExtensionSupport(vk::PhysicalDevice pPhysicalDevice)
{
    vector<vk::ExtensionProperties> extensions = pPhysicalDevice.enumerateDeviceExtensionProperties(nullptr);

    for (auto deviceextension : deviceExtensions)
    {
        bool hasextension = false;
        for (const auto& extension : extensions)
        {
            if (strcmp(deviceextension, extension.extensionName) == 0)
            {
                hasextension = true;
                break;
            }
        }
        if (!hasextension) return false;
    }
    return true;
}

bool Renderer::checkDeviceSuitable(vk::PhysicalDevice pPhysicalDevice)
{
//    vk::PhysicalDeviceProperties physicalDeviceProperties = physicalDevice.getProperties();
//    vk::PhysicalDeviceFeatures physicalDeviceFeatures = physicalDevice.getFeatures();

    bool extensionsSupported = checkDeviceExtensionSupport(pPhysicalDevice);

    bool swapchainValid = false;

    if (extensionsSupported)
    {
        SwapchainDetails swapchainDetails = getSwapchainDetails(pPhysicalDevice);
        swapchainValid = !swapchainDetails.presentationModes.empty() && !swapchainDetails.formats.empty();
    }

    return queueFamilyIndices.isValid() && extensionsSupported && swapchainValid;
}

// NOT REFACTORED :)

vk::ImageView Renderer::createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags)
{
//    // TODO Refactor into a texture class
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

    return imageView;
}

void Renderer::cleanUp()
{
    device.waitIdle();
    mainDeletionQueue.flush();

    device.destroySwapchainKHR(swapchain);
    instance.destroySurfaceKHR(surface);

//    for(auto frame : frames){
//        device.destroyCommandPool(frame.commandPool);
//        device.destroyFence(frame.renderFence);
//        device.destroySemaphore(frame.presentSemaphore);
//        device.destroySemaphore(frame.renderSemaphore);
//    }
    for (auto image : swapchainImages)
    {
        device.destroyImageView(image.imageView);
    }
    for (auto framebuffer : swapchainFramebuffers)
    {
        device.destroyFramebuffer(framebuffer);
    }
    device.destroyRenderPass(renderPass);
    glfwDestroyWindow(window);
    glfwTerminate();

    vertexBuffer.destroy(device);
    device.destroy();
    instance.destroy();

}

void Renderer::loadMeshes(std::vector<Renderable>* renderables){
    //TODO make sure vertexBuffer is not being overwritten by each renderable: add offset?
//    vertexBuffer.map(this, 0);

    for(auto renderable : *renderables){
        vertexBuffer.map(this, 0, renderable.mesh.getSize());
        vertexBuffer.copy(renderable.mesh.vertices.data(), renderable.mesh.getSize());
        vertexBuffer.unMap(this);
    }
}

void Renderer::createVertexBuffer(){
    vertexBuffer = Buffer(vk::BufferUsageFlags {vk::BufferUsageFlagBits::eVertexBuffer}, 500000);

    uint32_t memoryTypeIndex = getMemoryTypeIndex();
    vertexBuffer.load(this);
    vertexBuffer.allocate(memoryTypeIndex, this);
    vertexBuffer.bind(this);

}

uint32_t Renderer::getMemoryTypeIndex() {

    vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();

    uint32_t memoryTypeIndex = uint32_t(~0);
    // TODO make sure it's big enough for buffer requested size
//    vk::DeviceSize memoryHeapSize = uint32_t(~0);

// TODO add MemoryPropertyFlags as parameters. Use switch case to determine, based on passed buffer usage?
    for (uint32_t currentMemoryTypeIndex = 0; currentMemoryTypeIndex < memoryProperties.memoryTypeCount; ++currentMemoryTypeIndex) {
        vk::MemoryType memoryType = memoryProperties.memoryTypes[currentMemoryTypeIndex];
        if ((vk::MemoryPropertyFlagBits::eHostVisible & memoryType.propertyFlags) &&
            (vk::MemoryPropertyFlagBits::eHostCoherent & memoryType.propertyFlags)) {
//            memoryHeapSize = memoryProperties.memoryHeaps[memoryType.heapIndex].size;
            return currentMemoryTypeIndex;
        }
    }
    return memoryTypeIndex;
}

Renderer::FrameData &Renderer::getCurrentFrame() {
    return frames.at(currentFrame % MAX_FRAME_DRAWS);
}
