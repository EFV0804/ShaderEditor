//
// Created by elise on 27/09/2022.
//

#ifndef SHADEREDITOR_BUFFER_H
#define SHADEREDITOR_BUFFER_H
#include <Vulkan/vulkan.hpp>
#include <vk_mem_alloc.hpp>

class Renderer;

enum BufferState{
    Empty,
    Allocated,
    Locked,
    Destroyed
};

class Buffer {
public:
    Buffer(vk::BufferUsageFlags pUsage, uint64_t pSize);
    Buffer() = default;
    ~Buffer();
//    void createBuffer();
    void load(vma::Allocator* allocator);
    void map(vma::Allocator* allocator, void* data);
    void unMap(vma::Allocator* allocator);
//    void allocate(vk::MemoryAllocateInfo memoryAllocateInfo);
//    void copyTo();
//    void bind();
//    void lockMem();
//    void unlockMem();
    vk::BufferCreateInfo getBufferCreateInfo();
    vk::Buffer getBuffer(){return buffer;}

private:
//    Renderer* renderer;
    vk::BufferUsageFlags usage;
    vk::BufferCreateInfo info;
    BufferState state {BufferState::Empty};
    uint64_t size;
    vk::Buffer buffer;
    vk::DeviceMemory bufferMemory;
    vma::Allocation allocation;
};


#endif //SHADEREDITOR_BUFFER_H
