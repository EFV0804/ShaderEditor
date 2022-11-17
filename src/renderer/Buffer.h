//
// Created by elise on 27/09/2022.
//

#ifndef SHADEREDITOR_BUFFER_H
#define SHADEREDITOR_BUFFER_H
#include <Vulkan/vulkan.hpp>
 #include <memory>

class Renderer;
/*
 * enum buffer's possible states: Empty, Allocated, Mapped, Destroyed
 */
enum BufferState{
    Empty,
    Allocated,
    Mapped,
    Destroyed
};
/*
 * Buffer class serves as a wrapper for the vk::Buffer class
 */
class Buffer {
public:
    /*!
     * \brief Constructor initialises the usage and size variables.
     *
     * \param pUsage
     * \param pSize
     *
     */
    Buffer(vk::BufferUsageFlags pUsage, uint64_t pSize, vk::SharingMode sharingMode);
    Buffer() = delete;
//    Buffer(const Buffer&) = delete;
    Buffer &operator=(const Buffer&) = delete;
    ~Buffer() = default;

    /*!
     * \brief Initialises the vk::Buffer object
     * \param renderer
     */
    void init(uint32_t queueFamilyIndex);
    /*!
     * \brief Maps memory of the requested size at the right offset and store a pointer to that memory in BufferStart.
     *
     * \param device
     * \param offset
     * \param dataSize
     */
    void map(int offset, uint64_t dataSize);
    /*!
     * \brief Unmaps the buffer's map memory.
     *
     * \param device
     */
    void unMap();
    /*!
     * \brief Allocates memory for the buffer.
     * \param memoryTypeIndex
     * \param device
     */
    void allocate(uint32_t memoryTypeIndex);
    /*!
     * \brief Binds memory to the buffer
     *
     * \param device
     * \param memoryOffset
     */
    void bind(int memoryOffset = 0);
    /*!
     * \brief copies memory content from src pointer to buffer's start pointer. Uses memcpy.
     * \param src a pointer to the beginning of the memory of th source data to copy
     * \param dataSize the size of the data.
     */
    void copy(const void* src, uint64_t dataSize);
    /*!
     * \brief Destroy vk::Buffer object, frees memory and set Buffer object state to BufferState::Destroyed
     * \param device
     */
    void destroy();
    /*!
     * \brief util getter for vk::Buffer member
     * \return member buffer
     */
     const BufferState& getState() const {return state;}
     const vk::Buffer& getBuffer() const {return buffer;}
     /*!
      * \brief utility getter for buffer memory.
      * @return member vk::DeviceMemory bufferMemory
      */
     const vk::DeviceMemory& getDeviceMemory() const {return bufferMemory;}

private:
    vk::SharingMode sharingMode;
    /*
     * Used to store pointer for mapping memory
     */
    float* bufferStart = nullptr;
    /*
     * Size of the memory required for the buffer
     */
    uint64_t size = 0;
    /*
     * The buffer's usage.
     */
    vk::BufferUsageFlags usage;
    /*
     * The buffer memory that will be allocated.
     */
    vk::DeviceMemory bufferMemory{};
    /*
     * The state of the Buffer object
     */
    BufferState state {BufferState::Empty};
    /*
     * The Vulkan buffer object.
     */
    vk::Buffer buffer{};
    /*!
     * \brief Return the info for buffer creation.
     * \param queueFamilyIndices
     * \return a vk::BufferCreateInfo object
     */
    vk::BufferCreateInfo getBufferCreateInfo(uint32_t queueFamilyIndices);
};


#endif //SHADEREDITOR_BUFFER_H
