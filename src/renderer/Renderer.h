#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <stdexcept>
#include <vector>
#include <array>
#include "VkUtilities.h"
#include "Buffer.h"
#include <vk_mem_alloc.hpp>

class Renderable;

class Renderer
{
public:
	Renderer();
	~Renderer();


    vk::Instance instance;

    // PRESENTATION
    GLFWwindow* window;
    vk::SurfaceKHR surface;
    vk::Format swapchainImageFormat;
    vk::SwapchainKHR swapchain;
    vk::Extent2D swapchainExtent;
    std::vector<SwapchainImage> swapchainImages;

    // DEVICES
    vk::PhysicalDevice physicalDevice;
    vk::Device device;
    vk::PhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
    vma::Allocator allocator;


    // QUEUES
	struct {
		uint32_t graphicsFamily = -1;
		uint32_t presentationFamily = -1;
		uint32_t  computeFamily = -1;
		bool isValid()
		{
			return graphicsFamily >= 0 && presentationFamily >= 0 && computeFamily >= 0;
		}
	} queueFamilyIndices;

	vk::Queue computeQueue;
	vk::Queue graphicsQueue;
	vk::Queue presentationQueue;

    // SYNCHRONISATION
    struct {
        vector<vk::Semaphore> imageAvailable;
        vector<vk::Semaphore> renderFinished;
        vector<vk::Semaphore> graphicsWaitSemaphores;
        vector<vk::Semaphore> graphicsSignalSemaphores;

        //graphics semaphore used to signal the graphics process is done
        vk::Semaphore graphics;

    } semaphores;
    vector<vk::Fence> drawFences;


    // COMMANDS
    std::vector<vk::Framebuffer> swapchainFramebuffers;
//    vk::Framebuffer framebuffer;
    vk::CommandPool commandPool;
    vector<vk::CommandBuffer> commandBuffers;
//    vk::CommandBuffer commandBuffer;
    const int MAX_FRAME_DRAWS = 2;
    int currentFrame = 0;
    Buffer vertexBuffer{vk::BufferUsageFlagBits::eVertexBuffer, 500000};
//    std::vector<Renderable>* renderables;

    // GETTERS
    vk::RenderPass getRenderPass(){return renderPass;}
    vk::Extent2D getExtent(){return swapchainExtent;}

    // RUN
    int init();
	void draw(std::vector<Renderable>* renderables);
	void cleanUp();
    void loadMeshes(std::vector<Renderable>* renderables);

private:

    // DRAWING
//    std::vector<vk::Pipeline> pipelines;
    vk::RenderPass renderPass;

    // MEMORY MANAGEMENT
    void getPhysicalMemoryProperties();

    // INIT
    void createWindow();
	void createInstance();
    void createMemoryAllocator();
    void createLogicalDevice();
    void createQueues();
    void createSwapchain();
    void createGraphicsCommandBuffer();
    void createVertexBuffer();

    // SETTERS
    void setPhysicalDevice();
    void setSurface();
    void setQueueFamilyIndices();
    void setSwapchainExtent(const vk::SurfaceCapabilitiesKHR& surfaceCapabilities);

    // GETTERS
    SwapchainDetails getSwapchainDetails(vk::PhysicalDevice pPhysicalDevice);
    vk::PresentModeKHR getPresentationMode(const vector<vk::PresentModeKHR>& presentationModes);
    vk::SurfaceFormatKHR getSurfaceFormat(const vector<vk::SurfaceFormatKHR>& formats);

    // CHECKERS
    bool checkInstanceExtensionSupport(const std::vector<const char*>& checkExtensions);
    bool checkDeviceSuitable(vk::PhysicalDevice physicalDevice);
    bool checkDeviceExtensionSupport(vk::PhysicalDevice pPhysicalDevice);


    void createSynchronisation();

    void createRenderPass();

    void createFramebuffers();

    // TO REFACTOR
    vk::ImageView createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags);
};

