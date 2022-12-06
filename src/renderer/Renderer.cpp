#include "Renderer.h"
#include <set>
#include <iostream>
#include <GLFW/glfw3.h>
#include <string>
#include "Renderable.h"


int Renderer::init() {

    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, !isInit, "Renderer is already initialised, cannot be initialised twice.")
    window.init();
    try {
        initInstance();
        initSurface();
        initPhysicalDevice();
        setQueueFamilyIndices();
        initLogicalDevice();
        initQueues();
        initSwapchain();
        createDepthBufferRessources();
        initRenderPass();
        initFramebuffers();
        initVertexBuffer();
        initCommandBuffers();
        initCameraDescriptors();
        initCameraBuffers();
        createSynchronisation();
    }
    catch (const std::runtime_error &e) {
        SE_RENDERER_ERROR("Renderer failed to initialise: {}", e.what());
        return EXIT_FAILURE;
    }
    SE_RENDERER_INFO("Renderer initialised successfully");

    isInit = true;
    return EXIT_SUCCESS;

}

//// DRAW
void Renderer::draw(std::vector<Renderable> *renderables) {
    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, isInit, "Renderer is not initialised, initialise before calling rendering functions")

//******************--- START NEW FRAME ---******************//
    vk::Result result = device.waitForFences(getCurrentFrame()->renderFence, VK_TRUE, 1000000000);
    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, result == vk::Result::eSuccess,
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

//    const vk::ClearValue clearValues{
//            std::array<float, 4>{0.f, 0.f, .0f, .0f}
//    };

// ORDER OF CLEARVALUE SHOULD BE IDENTICAL TO RENDER PASSES
    std::array<vk::ClearValue, 2> clearValues{};
    clearValues[0].color = {std::array<float, 4>{0.f, 0.f, .0f, .0f}};
    clearValues[1].depthStencil = {{1.f, 0}};

    renderPassBeginInfo.pClearValues = clearValues.data();
    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());

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
    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, presentationResult == vk::Result::eSuccess,
                                "Graphics queue failed to present correctly.")

//************--- NEXT FRAME INCREMENTATION ---************//
    currentFrame = (currentFrame + 1) % MAX_FRAME_DRAWS;
}

void Renderer::drawRenderables(std::vector<Renderable> *renderables) {
    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, isInit, "Renderer is not initialised, initialise before calling rendering functions")
    //TODO bind vertexBuffer here and add counter to multiply Vertex.size()*count to offset vertexBuffer binding

    const Material* lastMaterial = nullptr;

    for(int i = 0; i < renderables->size(); i++){
        const Material* current_mat = renderables->at(i).getMaterial();

        if(current_mat != lastMaterial){
//            vk::Pipeline& pipeline = renderable.material->pipeline.getPipeline();
            getCurrentFrame()->commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, current_mat->pipeline.getPipeline());
            //TODO add semaphores to make sure camera buffer is not in use before copying or are frame semaphores enough?
            getCurrentFrame()->commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                                current_mat->pipeline.getLayout(),
                                                                0,
                                                                1,
                                                                &getCurrentFrame()->cameraDescriptorSet,
                                                                0,
                                                                nullptr);
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
        getCurrentFrame()->commandBuffer.bindIndexBuffer(indexBuffer.getBuffer(), 0, vk::IndexType::eUint32);
//        getCurrentFrame()->commandBuffer.draw(renderables->at(i).getMesh()->vertices.size(), 1, 0,0);
        getCurrentFrame()->commandBuffer.drawIndexed(static_cast<uint32_t>(renderables->at(i).getMesh()->indices.size()), 1, 0,0,0);

    }
}

void Renderer::initInstance() {
    SE_RENDERER_DEBUG("Instance initialisation");
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
        SE_RENDERER_ERROR("Instance doesn't support extension required");
//        throw std::runtime_error("");
    }
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();

    instance = vk::createInstance(instanceCreateInfo);
////    instance.get
//    mainDeletionQueue.push_function([=](){
//       instance.destroy();
//    });
    SE_RENDERER_DEBUG("Vulkan instance initialised");
}

void Renderer::initLogicalDevice() {

//    VK_KHR_SWAPCHAIN_EXTENSION_NAME

    SE_RENDERER_DEBUG("Logical Device initialisation");
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
    SE_RENDERER_DEBUG("Swapchain initialisation");

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


    swapchainImageFormat = surfaceFormat.format;

//    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, device.getSwapchainImagesKHR(swapchain) == vk::Result::eSuccess, "Swapchain image creation failed.")

    mainDeletionQueue.push_function([=]() {
        device.destroySwapchainKHR(swapchain);
    });

}

void Renderer::initCommandBuffers() {

    SE_RENDERER_DEBUG("Command Buffer initialisation");

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

void Renderer::initCameraBuffers() {
    SE_RENDERER_DEBUG("Camera uniform buffer initialisation.");
    // init buffers
    // map buffers - they stay mapped because of persistent mapping
    // add to deletion queue
    std::vector<vk::MemoryPropertyFlagBits> flags;
    flags.reserve(2);
    flags.emplace_back(vk::MemoryPropertyFlagBits::eHostVisible);
    flags.emplace_back(vk::MemoryPropertyFlagBits::eHostCoherent);

    for(int i = 0; i < MAX_FRAME_DRAWS; i++){
        frames.at(i).cameraBuffer.init(queueFamilyIndices.graphicsFamily);
        frames.at(i).cameraBuffer.allocate(getMemoryTypeIndex(flags));
        frames.at(i).cameraBuffer.bind();
        frames.at(i).cameraBuffer.map(0, sizeof(CameraBuffer));


        mainDeletionQueue.push_function([=]() {
            frames.at(i).cameraBuffer.unMap();
            frames.at(i).cameraBuffer.destroy();
        });

        // Allocate Camera Descriptor
        vk::DescriptorSetAllocateInfo allocInfo ={};
        allocInfo.pNext = nullptr;
        allocInfo.sType = vk::StructureType::eDescriptorSetAllocateInfo;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = 1;
        //using the global data layout
        allocInfo.pSetLayouts = &cameraDescriptorLayout;

        auto result = device.allocateDescriptorSets(&allocInfo, &frames.at(i).cameraDescriptorSet);

        // Point Descriptor to camera buffer
        vk::DescriptorBufferInfo binfo;
        binfo.buffer = frames.at(i).cameraBuffer.getBuffer();
        binfo.offset = 0;
        binfo.range = sizeof(CameraBuffer);

        vk::WriteDescriptorSet setWrite = {};
        setWrite.sType = vk::StructureType::eWriteDescriptorSet;
        setWrite.pNext = nullptr;
        setWrite.dstBinding = 0;
        setWrite.dstSet = frames.at(i).cameraDescriptorSet;
        setWrite.descriptorCount = 1;
        setWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
        setWrite.pBufferInfo = &binfo;

        device.updateDescriptorSets(1, &setWrite, 0, nullptr);

    }
}

void Renderer::updateCameraBuffer(const CameraBuffer& camData){
    //Copy param scene cam data into frame camerabuffer
    //TODO add semaphores to make sure camera buffer is not in use before copying
    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, frames.at(currentFrame).cameraBuffer.getState() == BufferState::Mapped, "Buffer is not mapped and cannot be copied into")
    frames.at(currentFrame).cameraBuffer.copy(&camData, sizeof(camData));
}

void Renderer::initCameraDescriptors(){
    // Descriptor set
    cameraDescriptorBinding.binding = 0;
    cameraDescriptorBinding.descriptorCount = 1;
    cameraDescriptorBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
    cameraDescriptorBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;

    vk::DescriptorSetLayoutCreateInfo descriptorInfo = {};
    descriptorInfo.sType = vk::StructureType::eDescriptorSetLayoutCreateInfo;
    descriptorInfo.pBindings = &cameraDescriptorBinding;
    descriptorInfo.pNext = nullptr;
    descriptorInfo.bindingCount = 1;
//    descriptorInfo.flags = vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool;

    cameraDescriptorLayout = device.createDescriptorSetLayout(descriptorInfo);

    std::vector<vk::DescriptorPoolSize> sizes =
            {
                    { vk::DescriptorType::eUniformBuffer, 10 }
            };

    vk::DescriptorPoolCreateInfo descriptorPoolInfo = {};
    descriptorPoolInfo.sType = vk::StructureType::eDescriptorPoolCreateInfo;
    descriptorPoolInfo.maxSets = 10;
    descriptorPoolInfo.poolSizeCount = (uint32_t)sizes.size();
    descriptorPoolInfo.pPoolSizes = sizes.data();
//    descriptorPoolInfo.flags = vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind;

    descriptorPool = device.createDescriptorPool(descriptorPoolInfo);


    mainDeletionQueue.push_function([=]() {
        device.destroyDescriptorPool(descriptorPool);
        device.destroyDescriptorSetLayout(cameraDescriptorLayout);

    });
}

void Renderer::initRenderPass() {
    SE_RENDERER_DEBUG("RenderPass initialisation");

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

    vk::AttachmentDescription depthAttachment{};
    depthAttachment.format = findDepthFormat();
    depthAttachment.samples = vk::SampleCountFlagBits::e1;
    depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
    depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    std::array<vk::AttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

    renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassCreateInfo.pAttachments = attachments.data();

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
    subpassDependencies[0].srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    subpassDependencies[0].srcAccessMask = vk::AccessFlagBits::eMemoryRead;
    subpassDependencies[0].dstSubpass = 0;
    subpassDependencies[0].dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    subpassDependencies[0].dstAccessMask = vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite;
    subpassDependencies[0].dependencyFlags = vk::DependencyFlagBits::eDeviceGroup;
//depth
    subpassDependencies[1].srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[1].dstSubpass = 0;
    subpassDependencies[1].srcStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
    subpassDependencies[1].srcAccessMask = vk::AccessFlagBits::eNone;
    subpassDependencies[1].dstStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
    subpassDependencies[1].dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    subpassDependencies[1].dependencyFlags = vk::DependencyFlagBits::eDeviceGroup;
    renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
    renderPassCreateInfo.pDependencies = subpassDependencies.data();


    renderPass = device.createRenderPass(renderPassCreateInfo);

    mainDeletionQueue.push_function([=]() {
        device.destroyRenderPass(renderPass);
    });
    SE_RENDERER_DEBUG("RenderPass initialised and added to deletion queue");
}

void Renderer::initFramebuffers() {

    SE_RENDERER_DEBUG("Frame buffers initialisation");
    std::vector<vk::Image> images = device.getSwapchainImagesKHR(swapchain);

    swapchainImagesViews.reserve(images.size());
    swapchainFramebuffers.resize(images.size());

    for (size_t i = 0; i < images.size(); ++i) {
        swapchainImagesViews.emplace_back(createImageView(images.at(i),
                                                          swapchainImageFormat,
                                                          vk::ImageAspectFlagBits::eColor));

        std::array<vk::ImageView, 2> attachments = {swapchainImagesViews.at(i), depthBufferImage.depthImageView};
        vk::FramebufferCreateInfo framebufferCreateInfo = {};
        framebufferCreateInfo.sType = vk::StructureType::eFramebufferCreateInfo;
        framebufferCreateInfo.renderPass = renderPass;
        framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferCreateInfo.pAttachments = attachments.data();
        framebufferCreateInfo.width = swapchainExtent.width;
        framebufferCreateInfo.height = swapchainExtent.height;
        framebufferCreateInfo.layers = 1;
        swapchainFramebuffers.at(i) = device.createFramebuffer(framebufferCreateInfo);

        mainDeletionQueue.push_function([=]() {
            device.destroyFramebuffer(swapchainFramebuffers.at(i));
            device.destroyImageView(swapchainImagesViews.at(i));

        });
    }
    SE_RENDERER_DEBUG("Frame buffers added to deletion queue");

}

void Renderer::createSynchronisation() {
    SE_RENDERER_DEBUG("Semaphore and Fence initialisation");

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
    SE_RENDERER_DEBUG("Fences and Semaphores added to deletion queue");

}

Renderer::SwapchainDetails Renderer::getSwapchainDetails(vk::PhysicalDevice pPhysicalDevice) const {
    SE_RENDERER_DEBUG("Getting Swapchain Details");
    SwapchainDetails swapchainDetails;
    swapchainDetails.surfaceCapabilities = pPhysicalDevice.getSurfaceCapabilitiesKHR(surface);
    swapchainDetails.supportedFormats = pPhysicalDevice.getSurfaceFormatsKHR(surface);
    swapchainDetails.supportedPresentationModes = pPhysicalDevice.getSurfacePresentModesKHR(surface);
    return swapchainDetails;
}

vk::SurfaceFormatKHR Renderer::getSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &formats) {
    SE_RENDERER_DEBUG("Getting Surface Format");
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
    SE_RENDERER_DEBUG("Getting presentation mode");
    for (const auto &presentationMode: presentationModes) {
        if (presentationMode == mode) {
            return presentationMode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

void Renderer::setQueueFamilyIndices() {

    SE_RENDERER_DEBUG("Queues initialisation");
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

    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, temp_index >= 0 && presentationSupport,
                                "Graphics and Presentation queue initialisation failed");
    queueFamilyIndices.graphicsFamily = temp_index;
    queueFamilyIndices.presentationFamily = temp_index;

}

void Renderer::initPhysicalDevice() {
    SE_RENDERER_DEBUG("Physical Device initialisation");
    std::vector<vk::PhysicalDevice> physicalDevices{instance.enumeratePhysicalDevices()};

    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, !physicalDevices.empty(), "No GPU with vulkan support found")

    for (auto device: physicalDevices) {
        if (checkDeviceSuitable(device)) {
            physicalDevice = device;
            physicalDeviceMemoryProperties = physicalDevice.getMemoryProperties();

        }
    }

}

void Renderer::initSurface() {
    SE_RENDERER_DEBUG("Surface initialisation");
    VkSurfaceKHR surface_temp;

    VkResult result = glfwCreateWindowSurface(instance,
                                window.getWindow(),
                                nullptr,
                                &surface_temp);
    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_,
                                result == VkResult::VK_SUCCESS,
                                "Surface initialisation failed:");
    surface = surface_temp; //c++ wrapper type conversion: vk::SurfaceKHR contains VkSurfaceKHR address

//    instance.destroySurfaceKHR(surface_temp);
    mainDeletionQueue.push_function([=]() {
        instance.destroySurfaceKHR(surface);
    });


}

void Renderer::setSwapchainExtent(const vk::SurfaceCapabilitiesKHR &surfaceCapabilities) {
    SE_RENDERER_DEBUG("Setting Swapchain Extent");
    if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        swapchainExtent = surfaceCapabilities.currentExtent;
    } else {
        VkExtent2D newExtent{};
        newExtent.width = static_cast<uint32_t>(window.getWidth());
        newExtent.height = static_cast<uint32_t>(window.getHeight());

        newExtent.width = std::max(surfaceCapabilities.minImageExtent.width,
                                   std::min(surfaceCapabilities.maxImageExtent.width, newExtent.width));
        newExtent.height = std::max(surfaceCapabilities.minImageExtent.height,
                                    std::min(surfaceCapabilities.maxImageExtent.height, newExtent.height));
        swapchainExtent = newExtent;
    }
}

bool Renderer::checkInstanceExtensionSupport(const std::vector<const char *> &checkExtensions) {
    SE_RENDERER_DEBUG("Checking extension support");
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
            SE_RENDERER_ERROR("Required extension is not supported: {0}", checkExtension);
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
//        device.destroy(imageView);
    });

    return imageView;
}
void Renderer::createDepthBufferRessources(){
    vk::Format depthFormat = findDepthFormat();
    createImage(swapchainExtent.width,
                swapchainExtent.height,
                depthFormat,
                vk::ImageTiling::eOptimal,
                vk::ImageUsageFlagBits::eDepthStencilAttachment,
                vk::MemoryPropertyFlagBits::eDeviceLocal,
                depthBufferImage.depthImage,
                depthBufferImage.depthImageMemory);
    depthBufferImage.depthImageView = createImageView(depthBufferImage.depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth);

    mainDeletionQueue.push_function([=]() {
        SE_RENDERER_DEBUG("deleting depth buffer imageView");
    device.destroyImageView(depthBufferImage.depthImageView);
        SE_RENDERER_DEBUG("deleting depth buffer image");
    device.destroyImage(depthBufferImage.depthImage);
    device.freeMemory(depthBufferImage.depthImageMemory);});

}
vk::Format Renderer::findDepthFormat() {
    return findSupportedFormat(
            {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
}
bool Renderer::hasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}
vk::Format Renderer::findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features){
    for (vk::Format format : candidates) {
        vk::FormatProperties props;
        physicalDevice.getFormatProperties(format, &props);

        if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}
void Renderer::cleanUp() {
    device.waitIdle();
    mainDeletionQueue.flush();
//    window.cleanUp();
    instance.destroy();
    SE_RENDERER_INFO("Renderer clean up successful");

}

void Renderer::loadMeshes(std::vector<Renderable> *renderables) {
    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, isInit, "Renderer is not initialised.")
//    //TODO make sure vertexBuffer is not being overwritten by each renderable: add offset?

// Get vertices and indices size
// Get single use cmd buffer

// 1. Indices
//      a.copy indices to staging buffer
//      b.Region copy staging buffer to index buffer
//          - init index buffer
//          - get buffer alloc info  + allocate cmd buffer
//          - begin cmd record
//          - region copy
//          - end cmd record
//          - submit
//          - free cmd buffer

// 1. Vertices
//      a.copy vertices to staging buffer
//      b.Region copy staging buffer to vertexBuffer
            //cmd record begin
            //region copy
            //cmd record end
            //submit
            // free cmd buffer

    //Indices size && vertices size


    for(int i = 0; i < renderables->size(); i++){
        for(auto& index :renderables->at(i).getMesh()->indices ){
            indices.push_back(index);
        }
        for(auto& vertex : renderables->at(i).getMesh()->vertices){
            vertices.push_back(vertex);
        }

        indexDeviceSize += indices.size() * sizeof(uint32_t);
        vertexDeviceSize += vertices.size() * sizeof(Vertex);
    }
    std::vector<vk::MemoryPropertyFlagBits> stagingFlags;
    stagingFlags.reserve(2);
    stagingFlags.emplace_back(vk::MemoryPropertyFlagBits::eHostVisible);
    stagingFlags.emplace_back(vk::MemoryPropertyFlagBits::eHostCoherent);

    int32_t stagingMemoryTypeIndex = getMemoryTypeIndex(stagingFlags);
    stagingBuffer.setSize(vertexDeviceSize);
    stagingBuffer.init(stagingMemoryTypeIndex);

    stagingBuffer.allocate(stagingMemoryTypeIndex);
    stagingBuffer.bind();

    //get buffer allocation info + allocate cmd buffer
    vk::CommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandPool = getCurrentFrame()->commandPool;
    allocInfo.commandBufferCount = 1;

    //Single use command buffer
    vk::CommandBuffer singleUseCmd;
    vk::CommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    vk::Result cmdAlloc = device.allocateCommandBuffers(&allocInfo, &singleUseCmd);
    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, cmdAlloc == vk::Result::eSuccess, "Command Buffer allocation failed.");


    vk::SubmitInfo submitInfo = {};
    submitInfo.sType = vk::StructureType::eSubmitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &singleUseCmd;


    //---------------------------------//

    // INDICES
    // Copy indices to staging buffer
    stagingBuffer.map(0, indexDeviceSize);
    stagingBuffer.copy(indices.data(), indexDeviceSize);
    stagingBuffer.unMap();
    // Copy indices from staging buffer to index buffer
        // Init index buffer
    std::vector<vk::MemoryPropertyFlagBits> flags;
    flags.push_back(vk::MemoryPropertyFlagBits::eHostCoherent);
    flags.push_back(vk::MemoryPropertyFlagBits::eHostVisible);
    initIndexBuffer(flags, indexDeviceSize, indices);


        // Copy to index buffer
    vk::BufferCopy indexCopyRegion  = {};
    indexCopyRegion.srcOffset = 0;
    indexCopyRegion.dstOffset = 0;
    indexCopyRegion.size = indexBuffer.getSize();

    singleUseCmd.begin(beginInfo);

    singleUseCmd.copyBuffer(stagingBuffer.getBuffer(),
                            indexBuffer.getBuffer(),
                            1,
                            &indexCopyRegion);

    singleUseCmd.end();


    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_,
                                graphicsQueue.submit(1,
                                                     &submitInfo,
                                                     VK_NULL_HANDLE) == vk::Result::eSuccess,
                                "Failed to submit staging buffer copying");
    graphicsQueue.waitIdle();


//----------------------------------------------------------------------------//
// VERTICES
    std::vector<vk::MemoryPropertyFlagBits> vertexFlags;
    vertexFlags.reserve(1);
    vertexFlags.emplace_back(vk::MemoryPropertyFlagBits::eDeviceLocal);

    uint32_t vertexMemoryTypeIndex = getMemoryTypeIndex(vertexFlags);

    vertexBuffer.setSize(vertexDeviceSize);
    vertexBuffer.init(queueFamilyIndices.graphicsFamily);
    vertexBuffer.allocate(vertexMemoryTypeIndex);
    vertexBuffer.bind();

    // Copy indices to staging buffer
    stagingBuffer.map(0, vertexDeviceSize);
    stagingBuffer.copy(vertices.data(), vertexDeviceSize);
    stagingBuffer.unMap();


    singleUseCmd.begin(beginInfo);

    vk::BufferCopy vertexCopyRegion  = {};
    vertexCopyRegion.srcOffset = 0;
    vertexCopyRegion.dstOffset = 0;
    vertexCopyRegion.size = vertexDeviceSize;

    singleUseCmd.copyBuffer(stagingBuffer.getBuffer(),
                            vertexBuffer.getBuffer(),
                            1,
                            &vertexCopyRegion);

    singleUseCmd.end();

    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_,
                                graphicsQueue.submit(1,
                                                     &submitInfo,
                                                     VK_NULL_HANDLE) == vk::Result::eSuccess,
                                "Failed to submit staging buffer copying");
    graphicsQueue.waitIdle();
    device.freeCommandBuffers(getCurrentFrame()->commandPool,
                              1,
                              &singleUseCmd);
}

void Renderer::initVertexBuffer() {

    SE_RENDERER_DEBUG("Initialising staging buffer.");
//    std::vector<vk::MemoryPropertyFlagBits> stagingFlags;
//    stagingFlags.reserve(2);
//    stagingFlags.emplace_back(vk::MemoryPropertyFlagBits::eHostVisible);
//    stagingFlags.emplace_back(vk::MemoryPropertyFlagBits::eHostCoherent);
//
//    int32_t stagingMemoryTypeIndex = getMemoryTypeIndex(stagingFlags);
//    stagingBuffer.init(stagingMemoryTypeIndex);
//
//    stagingBuffer.allocate(stagingMemoryTypeIndex);
//    stagingBuffer.bind();

    SE_RENDERER_DEBUG("Initialising vertex buffer.");
//    std::vector<vk::MemoryPropertyFlagBits> vertexFlags;
//    vertexFlags.reserve(1);
//    vertexFlags.emplace_back(vk::MemoryPropertyFlagBits::eDeviceLocal);
//
//    uint32_t vertexMemoryTypeIndex = getMemoryTypeIndex(vertexFlags);
//    vertexBuffer.init(queueFamilyIndices.graphicsFamily);
    mainDeletionQueue.push_function([=]() {stagingBuffer.destroy();});
    mainDeletionQueue.push_function([=]() {vertexBuffer.destroy();});



}
void Renderer::initIndexBuffer(std::vector<vk::MemoryPropertyFlagBits> flags, vk::DeviceSize size,
                               std::vector<uint32_t> indices) {

    indexBuffer.setSize(size);
    std::vector<vk::MemoryPropertyFlagBits> indexFlags;
    uint32_t indexMemoryTypeIndex = getMemoryTypeIndex(indexFlags);
    indexBuffer.init(queueFamilyIndices.graphicsFamily);
    indexBuffer.allocate(indexMemoryTypeIndex);
    indexBuffer.bind();
    mainDeletionQueue.push_function([=]() {indexBuffer.destroy();});

}

uint32_t Renderer::getMemoryTypeIndex(const std::vector<vk::MemoryPropertyFlagBits>& flags) {

    vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();

    uint32_t memoryTypeIndex = uint32_t(~0);
    // TODO make sure it's big enough for buffer requested size


    for (uint32_t currentMemoryTypeIndex = 0;
         currentMemoryTypeIndex < memoryProperties.memoryTypeCount; ++currentMemoryTypeIndex)
    {
        vk::MemoryType memoryType = memoryProperties.memoryTypes[currentMemoryTypeIndex];
        if(std::all_of(flags.begin(), flags.end(), [&memoryType](vk::MemoryPropertyFlagBits flag){return flag & memoryType.propertyFlags;}))
        {
            return currentMemoryTypeIndex;
        }
        else{
            SE_RENDERER_DEBUG("No matching memory types");
        }
    }

    return memoryTypeIndex;
}

void Renderer::createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlagBits properties, vk::Image& image, vk::DeviceMemory& imageMemory){
    vk::ImageCreateInfo imageInfo{};
    imageInfo.sType = vk::StructureType::eImageCreateInfo;
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.usage = usage;
    imageInfo.samples = vk::SampleCountFlagBits::e1;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;

    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_,
                                device.createImage(&imageInfo,
                                                               nullptr,
                                                               &image) == vk::Result::eSuccess,
                                 "Failed to create image")

    vk::MemoryRequirements memRequirements;
    device.getImageMemoryRequirements(image, &memRequirements);

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.sType = vk::StructureType::eMemoryAllocateInfo;
    allocInfo.allocationSize = memRequirements.size;
    const std::vector<vk::MemoryPropertyFlagBits> flags{properties};
//    allocInfo.memoryTypeIndex = getMemoryTypeIndex(memRequirements.memoryTypeBits, flags);
    allocInfo.memoryTypeIndex = getMemoryTypeIndex(flags);

    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_,device.allocateMemory( &allocInfo, nullptr, &imageMemory) == vk::Result::eSuccess, "Failed to allocate image");

    device.bindImageMemory(image, imageMemory, 0);
}
