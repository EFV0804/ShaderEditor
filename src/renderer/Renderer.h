#pragma once
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include <vulkan/vulkan.hpp>
#include <stdexcept>
#include <vector>
#include <array>
#include <deque>
#include <functional>
#include <memory>
#include "Logger.h"
#include "Buffer.h"
//#include "VkUtilities.h"

class Renderable;

/*!
 * \brief Struct describing a Deletion queue.
 *
 * The deletion queue is used to safely manage Vulkan object destruction. Everytime a Vulkan object that requires manual
 * destruction is created, the function call used to destroy it should be added to the deletion queue.
 */
struct DeletionQueue {
    std::deque<std::function<void()>> deletors;

    /*!
     * \brief Adds function to the queue.
     *
     * \param function the function be added to the queue.
     */
    void push_function(std::function<void()> &&function) {
        deletors.push_back(function);
    }

    /*!
     * \brief calls all functions added to the queue so far
     */
    void flush() {
        // reverse iterate the deletion queue to execute all the functions
        for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
            (*it)(); //call functors
        }

        deletors.clear();
    }
};
struct CameraBuffer{
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 viewproj;
};
/**
 * \brief Renderer class used to initialise Vulkan and do draw calls
 */

class Renderer {
public:
    Renderer() = default;
    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;
    ~Renderer() = default;

    static Renderer& Get(){
        static Renderer instance;
        return instance;
    }

    bool isInit = false;
    /**
     * Point to the GLFW window.
     */
    GLFWwindow *window = nullptr;
    /**
     * A Vulkan logical device, i.e a description of which physical device features are being used.
     */
    vk::Device device;
    /*!
*  \brief Initialises the renderer by calling the initialisations functions of required components.
*  \return int for SUCCESS or FAILURE
*/
    int init();
    /*!
     * \brief Draws a frame to screen.
     *
     * Records and submit commands to the queue, begins and ends render pass, and signals appropriate semaphores and fence to trigger next frame render.
     *
     * \param [in] renderables a vector of renderable objects to be drawn on screen.
     */
    void draw(std::vector<Renderable> *renderables);
    /*!
     * \brief Draws a frame to screen.
     *
     * Records and submit commands to the queue, begins and ends render pass, and signals appropriate semaphores and fence to trigger next frame render.
     */
    void draw();
    /*!
     * \brief Draws renderables to screen.
     *
     * In charge of binding the pipeline associated with the renderable's material, binding the vertex buffer, and making the draw call.
     *
     * \param [in] renderables a pointer to a vector of renderable objects.
     */
    void drawRenderables(std::vector<Renderable> *renderables);
    /*!
     * \brief Destroys all Vulkan objects when Renderer object destructor is called.
     *
     * Waits for the device to be idle before destroying objects. Uses the deletion queue to detroy objects.
     */
    void cleanUp();
    /*!
     * \brief copies mesh vertex data to the vertex buffer.
     *
     * \param [in] a pointer to a vector of renderable objects.
     */
    void loadMeshes(std::vector<Renderable> *renderables);
    void updateCameraBuffer(const CameraBuffer& camData);
    /*!
     * \brief Getter for the swapchain extent
     * \return vk::Extent2D object
     */
    const vk::Extent2D& getSwapchainExtent() const {return swapchainExtent;}
    /*
 * \brief Returns the render pass
 */
    const vk::RenderPass& getRenderPass() const { return renderPass; }
    const vk::DescriptorSetLayout& getCameraDescriptorLayout() const {return cameraDescriptorLayout;}
    /**
 * Deletion queue used to ensure destruction of Vulkan entities. Uses FIFO logic.
 */
    DeletionQueue mainDeletionQueue;
    /**
* indicates the number corresponding to the current frame. Equivalent to currentFrame % MAX_FRAME_DRAWS.
*/
    int currentFrame = 0;
private:
    /**
    * Vulkan instance
    */
    vk::Instance instance;
    /*
     * Describes how the rendering process should go. Manages the relationship between attachments and subpasses.
     */
    vk::RenderPass renderPass;
    /**
 * a Vulkan surface, i.e an abstraction of the GLFW window than Vulkan can interact with.
 */
    vk::SurfaceKHR surface;
    /**
     * Struct to store and organise the image and image view that constitute the swapchain images.
     */
    struct SwapchainImage {
        VkImage image; /**< An image :) */
        VkImageView imageView; /**< A specific part of an image to be used to render. */
    };
    /**
     * The format used for the swapchain images. Defaults to 32bits unsigned normalised.
     */
    vk::Format swapchainImageFormat{vk::Format::eB8G8R8A8Unorm};
    /**
     * The swapchain stores images before they are presented.
     */
    vk::SwapchainKHR swapchain;
    /**
     * The extent of the swapchain, a width and height.
     */
    vk::Extent2D swapchainExtent{800,600};
    /**
     * A vector of swapchain images, that are used as attachments for framebuffers.
     */
    std::vector<SwapchainImage> swapchainImages;
    /**
     * \brief Provides information about the swapchain.
     *
     * \var vk::SurfaceCapabilitesKHR : surfaceCapabilities
     * \var std::vector<vk::SurfaceFormats> : supportedFormats
     * \var std::vector<vk::PresentModeKHR> : supportedPresentationModes
     */
    struct SwapchainDetails {
        vk::SurfaceCapabilitiesKHR surfaceCapabilities; /**< Store the surface's capabilities. */
        std::vector<vk::SurfaceFormatKHR> supportedFormats; /**< A vector of the surface's supported formats. */
        std::vector<vk::PresentModeKHR> supportedPresentationModes; /**< A vector the surface's supported presentation modes. */
    };
    /**
     * A Vulkan representation of a chosen GPU.
     */
    vk::PhysicalDevice physicalDevice;
    /**
 * The available memory types and properties available on the current physical device.
 */
    vk::PhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
    /**
     * Struct containing the queue family indices used to select and create the various queues.
     */
    struct {
        uint32_t graphicsFamily = -1;
        uint32_t presentationFamily = -1;
        uint32_t computeFamily = -1;

        /**
         * \brief Checks validity of all queue family indices.
         *
         *  Checks if all queue family indices have been changed, meaning they are available on current physical device.
         */
        bool isValid() {
            return graphicsFamily >= 0 && presentationFamily >= 0 && computeFamily >= 0;
        }
    } queueFamilyIndices;
    /**
     * Compute is used to submit compute shader instructions.
     */
    vk::Queue computeQueue;
    /**
     * Graphics queue is used to submit graphics instructions.
     */
    vk::Queue graphicsQueue;
    /**
     * Presentation queue where instruction relative to displaying rendered images are sent.
     */
    vk::Queue presentationQueue;
    /**
     * Maximum number of frames to draw. Allows for double buffering.
     */
    static constexpr int MAX_FRAME_DRAWS = 2;
    /**
     * Data structure for a frame.
     */
    struct FrameData {
        vk::Semaphore presentSemaphore;   /**< Signals when frame is ready to be displayed. */
        vk::Semaphore renderSemaphore;    /**< Signals when frame is ready to be rendered */
        vk::Fence renderFence;            /**< Signals when frame has been submitted to graphics queue. */
        vk::CommandPool commandPool;      /**< A pool associated with specific queue family, from which command buffers are allocated. */
        vk::CommandBuffer commandBuffer;  /**< Buffer where commands are recorded before being submitted to the queue.*/
        Buffer cameraBuffer{vk::BufferUsageFlagBits::eUniformBuffer, 50000, vk::SharingMode::eExclusive}; /**< Uniform buffer containing proj view matrices and camera position */
        vk::DescriptorSet cameraDescriptorSet;
    };
    /*
     * Vector a frame data describing all the frames in the process of being rendered/presented.
     */
    std::vector<FrameData> frames;
    /*
     * A vector of framebuffers. A framebuffer references an image view for the swapchain that are used for color, depth and stencils.
     */
    std::vector<vk::Framebuffer> swapchainFramebuffers;
    /*
     * a Buffer object destined to store vertices to be rendered.
     */
    Buffer vertexBuffer{vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, 50000, vk::SharingMode::eExclusive};
    Buffer indexBuffer{vk::BufferUsageFlagBits::eVertexBuffer, 50000, vk::SharingMode::eExclusive};
    Buffer stagingBuffer{vk::BufferUsageFlagBits::eTransferSrc, 50000, vk::SharingMode::eExclusive};
    vk::DescriptorSetLayoutBinding cameraDescriptorBinding;
    vk::DescriptorSetLayout cameraDescriptorLayout;
    vk::DescriptorPool descriptorPool;
    /*!
     * \brief Creates a GLFW window.
     */
    void initWindow();
    /*!
     * \brief initialises the Vulkan instance.
     */
    void initInstance();
    /*!
     * \brief initialises logical device
     */
    void initLogicalDevice();
    /*!
     * \brief initialises compute, graphics and presentation queues.
     */
    void initQueues();
    /*!
     * \brief initialises swapchain.
     */
    void initSwapchain();
    /*!
     * \brief initialises command pools and buffers.
     */
    void initCommandBuffers();

    void initCameraBuffers();

    void initCameraDescriptors();
    /*!
     * \brief initialises semaphores and fence for each frame.
     */
    void createSynchronisation();
    /*!
     * \brief initialises render pass.
     */
    void initRenderPass();
    /*!
     * \brief initialises framebuffers.
     */
    void initFramebuffers();
    /*!
     * \brief initialises vertex buffer.
     */
    void initVertexBuffer();
    void initIndexBuffer(std::vector<vk::MemoryPropertyFlagBits> flags, vk::DeviceSize size,
                         std::vector<uint16_t> indices);
    /*!
     * \brief initialises physical device.
     */
    void initPhysicalDevice();
    /*!
     * \brief initialises surface.
     */
    void initSurface();
    /*!
     * \brief Sets the queue family indices .
     *
     * Retrieves appropriate index for queue based on the GPU's properties and the requested queues.
     */
    void setQueueFamilyIndices();
    /*!
     * \brief Sets the extent of the swapchain.
     *
     * Sets the extent of the swapchain by getting the width and height of the GLFW window.
     */
    void setSwapchainExtent(const vk::SurfaceCapabilitiesKHR &surfaceCapabilities);
    /*!
     * \brief Gets details needed for swapchain construction.
     *
     * Retrieves the surface's capabilities, supported format, supported presentation modes.
     */
    SwapchainDetails getSwapchainDetails(vk::PhysicalDevice pPhysicalDevice) const;
    /*!
     * \brief Selects the requested presentation mode if available in presentation modes passed.
     *
     * \param presentationModes pressetion modes supported by the surface
     * \param mode the requested presentation mode of type vk::PresentModeKHR
     *
     * \return the requested vk::PresentMode enum value or if unavailable, defaults back to FIFO presentation mode.
     */
    vk::PresentModeKHR
    selectPresentationMode(const std::vector<vk::PresentModeKHR> &presentationModes, vk::PresentModeKHR mode);
    /*!
     * \brief get the specific format from a vector of formats supported by the Vulkan surface.
     *
     * \param formats supported formats by the surface.
     *
     * \return returns the colorspace SrgbNonLinear and 32bit unsigned normalized color format (8R8G8Bunorm).
     */
    vk::SurfaceFormatKHR getSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &formats);
    /*!
     * \brief Checks if requested instance extensions are supported by GPU
     *
     * \param checkExtensions
     *
     * \return Returns true if all extensions are supported, false else.
     */
    bool checkInstanceExtensionSupport(const std::vector<const char *> &checkExtensions);
    /*!
     * \brief Checks if requested instance extensions are supported by GPU
     *
     * \param checkExtensions
     *
     * \return Returns true if all extensions are supported, false else.
     */
    bool checkDeviceSuitable(vk::PhysicalDevice physicalDevice);
    /*!
     * \brief Checks if requested extensions are supported by the device.
     *
     * \param pPhysicalDevice
     *
     * \return Returns true if they are supported false if they aren't.
     */
    bool checkDeviceExtensionSupport(vk::PhysicalDevice pPhysicalDevice);
    /*!
     * \brief Creates an image view object, and adds it to the deletion queue.
     *
     * \param image the image to use to make the image view
     * \param format
     * \param aspectFlags
     *
     * \return Returns the created image view object.
     */
    vk::ImageView createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags);
    /*!
 * \brief Gets the appropriate memory type index for the requested memory usage.
 *
 * \param [in] NOT IMPLEMENTED, see function definition. memory property flag determines the type of memory for which to return the index.
 *
 * \return the index of the requested memory type
 */
    uint32_t getMemoryTypeIndex(const std::vector<vk::MemoryPropertyFlagBits>& flags);
    /*!
 * \brief  Returns the current frame for easy access
 *
 * \return Pointer to vector element of current frame.
 */
    FrameData *getCurrentFrame() { return &frames.at(currentFrame % MAX_FRAME_DRAWS); }
};