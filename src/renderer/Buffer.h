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

class Buffer {
public:
    Buffer(vk::BufferUsageFlags pUsage, uint64_t pSize);
    Buffer() = delete;
    Buffer(const Buffer&) = delete;
    ~Buffer() = default;

    void load(Renderer* renderer);
    void map(Renderer *renderer, int offset, uint64_t dataSize);
    void unMap(Renderer *renderer);
    void allocate(uint32_t memoryTypeIndex, Renderer *renderer);
    void bind(Renderer *renderer, int memoryOffset = 0);
    void copy(const void* src, uint64_t dataSize);
    void destroy(vk::Device& device);


    vk::BufferCreateInfo getBufferCreateInfo(uint32_t queueFamilyIndices);
    vk::Buffer& getBuffer(){return buffer;}

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
