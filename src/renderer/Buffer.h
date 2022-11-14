//
// Created by elise on 27/09/2022.
//

#ifndef SHADEREDITOR_BUFFER_H
#define SHADEREDITOR_BUFFER_H
#include <Vulkan/vulkan.hpp>
 #include <memory>

class Renderer;

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
    Buffer(vk::BufferUsageFlags pUsage, uint64_t pSize);
    Buffer() = delete;
    Buffer(const Buffer&) = delete;
    ~Buffer() = default;

    /*!
     * \brief Initialises the vk::Buffer object
     * \param renderer
     */
    void init(uint32_t queueFamilyIndex, vk::Device &device);
    /*!
     * \brief Maps memory of the requested size at the right offset and store a pointer to that memory in BufferStart.
     *
     * \param device
     * \param offset
     * \param dataSize
     */
    void map(vk::Device &device, int offset, uint64_t dataSize);
    /*!
     * \brief Unmaps the buffer's map memory.
     *
     * \param device
     */
    void unMap(vk::Device &device);
    /*!
     * \brief Allocates memory for the buffer.
     * @param memoryTypeIndex
     * @param device
     */
    void allocate(uint32_t memoryTypeIndex, vk::Device &device);
    void bind(vk::Device &device, int memoryOffset = 0);
    void copy(const void* src, uint64_t dataSize);
    void destroy(vk::Device& device);


    vk::BufferCreateInfo getBufferCreateInfo(uint32_t queueFamilyIndices);
    vk::Buffer& getBuffer(){return buffer;}
    vk::DeviceMemory& getDeviceMemory(){return bufferMemory;}

private:
    float* bufferStart = nullptr;
    uint64_t size = 0;
    vk::BufferUsageFlags usage;
    vk::DeviceMemory bufferMemory{};
    vk::BufferCreateInfo info{};
    // TODO Use state to check safe buffer use
    BufferState state {BufferState::Empty};
    vk::Buffer buffer{};
    vk::MemoryAllocateInfo memoryAllocateInfo{};
};


#endif //SHADEREDITOR_BUFFER_H
