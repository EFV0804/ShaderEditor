#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <stdexcept>
#include <vector>
#include <array>
#include <deque>
#include <functional>
#include "VkUtilities.h"
#include "Buffer.h"

class Renderable;

struct DeletionQueue
{
    std::deque<std::function<void()>> deletors;

    void push_function(std::function<void()>&& function) {
        deletors.push_back(function);
    }

    void flush() {
        // reverse iterate the deletion queue to execute all the functions
        for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
            (*it)(); //call functors
        }

        deletors.clear();
    }
};

class Renderer
{
public:
	Renderer();
	~Renderer();


    vk::Instance instance;

    DeletionQueue mainDeletionQueue;

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
//    struct {
//        vector<vk::Semaphore> imageAvailable;
//        vector<vk::Semaphore> renderFinished;
//        vector<vk::Semaphore> graphicsWaitSemaphores;
//        vector<vk::Semaphore> graphicsSignalSemaphores;
//
//        //graphics semaphore used to signal the graphics process is done
//        vk::Semaphore graphics;
//
//    } semaphores;
//    vector<vk::Fence> drawFences;


    // COMMANDS
    int currentFrame = 0;
    struct FrameData{
        vk::Semaphore presentSemaphore;
        vk::Semaphore renderSemaphore;
        vk::Fence renderFence;
        vk::CommandPool commandPool;
        vk::CommandBuffer commandBuffer;
    };
    static constexpr int MAX_FRAME_DRAWS = 2;
    std::vector<FrameData> frames;
    FrameData getCurrentFrame();

    std::vector<vk::Framebuffer> swapchainFramebuffers;
    Buffer vertexBuffer;

    // GETTERS
    vk::RenderPass getRenderPass(){return renderPass;}
    vk::Extent2D getExtent(){return swapchainExtent;}

    // RUN
    int init();
	void draw(std::vector<Renderable>* renderables);
    void drawRenderables(std::vector<Renderable>* renderables);
	void cleanUp();
    void loadMeshes(std::vector<Renderable>* renderables);

    uint32_t getMemoryTypeIndex();

private:

    // DRAWING
//    std::vector<vk::Pipeline> pipelines;
    vk::RenderPass renderPass;

    // MEMORY MANAGEMENT
    void getPhysicalMemoryProperties();

    // INIT
    void createWindow();
	void createInstance();
//    void createMemoryAllocator();
    void createLogicalDevice();
    void createQueues();
    void createSwapchain();
    void createCommandBuffers();
    void createSyncStructures();
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

