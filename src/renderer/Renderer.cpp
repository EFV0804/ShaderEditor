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
//        createDepthBufferResources();
        initRenderPass();
//        initFramebuffers();
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
    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, isInit,
                                "Renderer is not initialised, initialise before calling rendering functions")

//******************--- START NEW FRAME ---******************//
    vk::Result result = device.waitForFences(getCurrentFrame()->renderFence, VK_TRUE, 1000000000);
    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, result == vk::Result::eSuccess,
                                "Device timed out waiting for previous frame.");
    device.resetFences(getCurrentFrame()->renderFence);
    getCurrentFrame()->commandBuffer.reset();

    uint32_t imageToBeDrawnIndex;

    vk::ResultValue r = device.acquireNextImageKHR(swapchain.getSwapchain(),
                                                   1000000000,
                                                   getCurrentFrame()->presentSemaphore,
                                                   VK_NULL_HANDLE);
    imageToBeDrawnIndex = r.value;

//***************--- BEGIN COMMAND BUFFER---**********µ*******//
//    vk::CommandBufferBeginInfo commandBufferBeginInfo{};
//    commandBufferBeginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
//    commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
//    getCurrentFrame()->commandBuffer.begin(commandBufferBeginInfo);

    getCurrentFrame()->commandBuffer.begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
//***************--- RENDER PASS BEGINS ---**********µ*******//
    renderpass.begin(imageToBeDrawnIndex, getCurrentFrame()->commandBuffer.getCommandBuffer());

//******************--- DRAW OBJECTS ---********************//
    drawRenderables(renderables);

//******************--- RENDER PASS ENDS ---*****************//
    renderpass.end(getCurrentFrame()->commandBuffer.getCommandBuffer());

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
    submitInfo.pCommandBuffers = &getCurrentFrame()->commandBuffer.getCommandBuffer();
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &getCurrentFrame()->renderSemaphore;

    graphicsQueue.submit(submitInfo, getCurrentFrame()->renderFence);

//******************--- PRESENTATION ---********************//
    vk::PresentInfoKHR presentInfo{};
    presentInfo.sType = vk::StructureType::ePresentInfoKHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &getCurrentFrame()->renderSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain.getSwapchain();
    presentInfo.pImageIndices = &imageToBeDrawnIndex;

    vk::Result presentationResult = graphicsQueue.presentKHR(presentInfo);
    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, presentationResult == vk::Result::eSuccess,
                                "Graphics queue failed to present correctly.")

//************--- NEXT FRAME INCREMENTATION ---************//
    currentFrame = (currentFrame + 1) % MAX_FRAME_DRAWS;
}

void Renderer::drawRenderables(std::vector<Renderable> *renderables) {
    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, isInit,
                                "Renderer is not initialised, initialise before calling rendering functions")
    //TODO bind vertexBuffer here and add counter to multiply Vertex.size()*count to offset vertexBuffer binding

    const Material *lastMaterial = nullptr;

    for (int i = 0; i < renderables->size(); i++) {
        const Material *current_mat = renderables->at(i).getMaterial();

        if (current_mat != lastMaterial) {
//            vk::Pipeline& pipeline = renderable.material->pipeline.getPipeline();
            getCurrentFrame()->commandBuffer.getCommandBuffer().bindPipeline(vk::PipelineBindPoint::eGraphics,
                                                                             current_mat->pipeline.getPipeline());
            //TODO add semaphores to make sure camera buffer is not in use before copying or are frame semaphores enough?
            getCurrentFrame()->commandBuffer.getCommandBuffer().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
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
        for (auto constant: renderables->at(i).getMaterial()->getPushConstants()) {
            getCurrentFrame()->commandBuffer.getCommandBuffer().pushConstants(
                    renderables->at(i).getMaterial()->pipeline.getLayout(),
                    constant.stageFlags,
                    constant.offset,
                    constant.size,
                    &renderables->at(i).transform);
        }

        getCurrentFrame()->commandBuffer.getCommandBuffer().bindVertexBuffers(0, 1, &vertexBuffer.getBuffer(), &offset);
        getCurrentFrame()->commandBuffer.getCommandBuffer().bindIndexBuffer(indexBuffer.getBuffer(), 0,
                                                                            vk::IndexType::eUint32);
        getCurrentFrame()->commandBuffer.getCommandBuffer().drawIndexed(
                static_cast<uint32_t>(renderables->at(i).getMesh()->indices.size()), 1, 0, 0, 0);
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
    swapchain.init();
    mainDeletionQueue.push_function([=]() {
        swapchain.cleanUp();
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


        frames.back().commandBuffer.allocate(frames.back().commandPool, true);

        mainDeletionQueue.push_function([=]() {
            device.destroyCommandPool(frames[i].commandPool);
            frames.at(i).commandBuffer.cleanUp();
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

    for (int i = 0; i < MAX_FRAME_DRAWS; i++) {
        frames.at(i).cameraBuffer.init(queueFamilyIndices.graphicsFamily);
        frames.at(i).cameraBuffer.allocate(getMemoryTypeIndex(flags));
        frames.at(i).cameraBuffer.bind();
        frames.at(i).cameraBuffer.map(0, sizeof(CameraBuffer));


        mainDeletionQueue.push_function([=]() {
            frames.at(i).cameraBuffer.unMap();
            frames.at(i).cameraBuffer.destroy();
        });

        // Allocate Camera Descriptor
        vk::DescriptorSetAllocateInfo allocInfo = {};
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

void Renderer::updateCameraBuffer(const CameraBuffer &camData) {
    //Copy param scene cam data into frame camerabuffer
    //TODO add semaphores to make sure camera buffer is not in use before copying
    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, frames.at(currentFrame).cameraBuffer.getState() == BufferState::Mapped,
                                "Buffer is not mapped and cannot be copied into")
    frames.at(currentFrame).cameraBuffer.copy(&camData, sizeof(camData));
}

void Renderer::initCameraDescriptors() {
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
                    {vk::DescriptorType::eUniformBuffer, 10}
            };

    vk::DescriptorPoolCreateInfo descriptorPoolInfo = {};
    descriptorPoolInfo.sType = vk::StructureType::eDescriptorPoolCreateInfo;
    descriptorPoolInfo.maxSets = 10;
    descriptorPoolInfo.poolSizeCount = (uint32_t) sizes.size();
    descriptorPoolInfo.pPoolSizes = sizes.data();
//    descriptorPoolInfo.flags = vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind;

    descriptorPool = device.createDescriptorPool(descriptorPoolInfo);


    mainDeletionQueue.push_function([=]() {
        device.destroyDescriptorPool(descriptorPool);
        device.destroyDescriptorSetLayout(cameraDescriptorLayout);

    });
}

void Renderer::initRenderPass() {
    renderpass.init();
    renderpass.initFramebuffers();
    mainDeletionQueue.push_function([=]() { renderpass.cleanUp(); });
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

    for (auto physDevice: physicalDevices) {
        vk::PhysicalDeviceProperties properties = physDevice.getProperties();
        if(properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu){
            if (checkDeviceSuitable(physDevice)) {
                physicalDevice = physDevice;
                physicalDeviceMemoryProperties = physicalDevice.getMemoryProperties();

            }
        }

    }
    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, !physicalDevice == NULL, "No suitable devices found")
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

    mainDeletionQueue.push_function([=]() {
        instance.destroySurfaceKHR(surface);
    });
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
        SwapchainDetails tempSwapchainDetails;
        tempSwapchainDetails.surfaceCapabilities = pPhysicalDevice.getSurfaceCapabilitiesKHR(surface);
        tempSwapchainDetails.supportedFormats = pPhysicalDevice.getSurfaceFormatsKHR(surface);
        tempSwapchainDetails.supportedPresentationModes = pPhysicalDevice.getSurfacePresentModesKHR(surface);
        swapchainValid =
                !tempSwapchainDetails.supportedPresentationModes.empty() &&
                !tempSwapchainDetails.supportedFormats.empty();
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

bool Renderer::hasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

vk::Format Renderer::findSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling,
                                         vk::FormatFeatureFlags features) {
    for (vk::Format format: candidates) {
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

    const Mesh* last_mesh = nullptr;

//    for (int i = 0; i < renderables->size(); i++) {
//
//
//
//        if(last_mesh != renderables->at(i).getMesh()){
//            last_mesh = renderables->at(i).getMesh();
//            for (auto &index: renderables->at(i).getMesh()->indices) {
//                indices.push_back(index);
//            }
//            for (auto &vertex: renderables->at(i).getMesh()->vertices) {
//                vertices.push_back(vertex);
//            }
//
//        }
//
//        indexDeviceSize += indices.size() * sizeof(uint32_t);
//        vertexDeviceSize += vertices.size() * sizeof(Vertex);
//    }


//TODO check if last loaded mesh is the same as current one

    for (auto &index: renderables->at(0).getMesh()->indices) {
        indices.push_back(index);
    }
    for (auto &vertex: renderables->at(0).getMesh()->vertices) {
        vertices.push_back(vertex);
    }
    indexDeviceSize += indices.size() * sizeof(uint32_t);
    vertexDeviceSize += vertices.size() * sizeof(Vertex);


    std::vector<vk::MemoryPropertyFlagBits> stagingFlags;
    stagingFlags.reserve(2);
    stagingFlags.emplace_back(vk::MemoryPropertyFlagBits::eHostVisible);
    stagingFlags.emplace_back(vk::MemoryPropertyFlagBits::eHostCoherent);

    int32_t stagingMemoryTypeIndex = getMemoryTypeIndex(stagingFlags);
    stagingBuffer.setSize(vertexDeviceSize);
    stagingBuffer.init(stagingMemoryTypeIndex);

    stagingBuffer.allocate(stagingMemoryTypeIndex);
    stagingBuffer.bind();

    CommandBuffer singleUseCmd;
    singleUseCmd.allocate(getCurrentFrame()->commandPool, true);


    vk::SubmitInfo submitInfo = {};
    submitInfo.sType = vk::StructureType::eSubmitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &singleUseCmd.getCommandBuffer();


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
    initIndexBuffer(flags, indexDeviceSize);


    // Copy to index buffer
    vk::BufferCopy indexCopyRegion = {};
    indexCopyRegion.srcOffset = 0;
    indexCopyRegion.dstOffset = 0;
    indexCopyRegion.size = indexBuffer.getSize();

    singleUseCmd.begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    singleUseCmd.getCommandBuffer().copyBuffer(stagingBuffer.getBuffer(),
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


    singleUseCmd.begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    vk::BufferCopy vertexCopyRegion = {};
    vertexCopyRegion.srcOffset = 0;
    vertexCopyRegion.dstOffset = 0;
    vertexCopyRegion.size = vertexDeviceSize;

    singleUseCmd.getCommandBuffer().copyBuffer(stagingBuffer.getBuffer(),
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
                              &singleUseCmd.getCommandBuffer());

    mainDeletionQueue.push_function([=]() { stagingBuffer.destroy(); });
    mainDeletionQueue.push_function([=]() { vertexBuffer.destroy(); });
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



}

void Renderer::initIndexBuffer(std::vector<vk::MemoryPropertyFlagBits> flags, vk::DeviceSize size) {

    indexBuffer.setSize(size);
    uint32_t indexMemoryTypeIndex = getMemoryTypeIndex(flags);
    indexBuffer.init(queueFamilyIndices.graphicsFamily);
    indexBuffer.allocate(indexMemoryTypeIndex);
    indexBuffer.bind();
    mainDeletionQueue.push_function([=]() { indexBuffer.destroy(); });

}

uint32_t Renderer::getMemoryTypeIndex(const std::vector<vk::MemoryPropertyFlagBits> &flags) {

    vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();

    uint32_t memoryTypeIndex = uint32_t(~0);
    // TODO make sure it's big enough for buffer requested size


    for (uint32_t currentMemoryTypeIndex = 0;
         currentMemoryTypeIndex < memoryProperties.memoryTypeCount; ++currentMemoryTypeIndex) {
        vk::MemoryType memoryType = memoryProperties.memoryTypes[currentMemoryTypeIndex];
        if (std::all_of(flags.begin(), flags.end(),
                        [&memoryType](vk::MemoryPropertyFlagBits flag) { return flag & memoryType.propertyFlags; })) {
            return currentMemoryTypeIndex;
        } else {
            SE_RENDERER_DEBUG("No matching memory types");
        }
    }

    return memoryTypeIndex;
}

void Renderer::createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling,
                           vk::ImageUsageFlags usage, vk::MemoryPropertyFlagBits properties, vk::Image &image,
                           vk::DeviceMemory &imageMemory) {
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

    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_,
                                device.allocateMemory(&allocInfo, nullptr, &imageMemory) == vk::Result::eSuccess,
                                "Failed to allocate image");

    device.bindImageMemory(image, imageMemory, 0);
}
