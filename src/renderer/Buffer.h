//
// Created by elise on 27/09/2022.
//

#ifndef SHADEREDITOR_BUFFER_H
#define SHADEREDITOR_BUFFER_H
//#include <Vulkan/vulkan.hpp>
//#include <vk_mem_alloc.hpp>
#include "Allocator.h"
#include <memory>

class Renderer;

namespace vma{
    class Allocation;
}

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
    void load(Renderer* renderer);
    void map(Renderer* renderer, void* data);
    void unMap(Renderer* renderer);
//    void allocate(vk::MemoryAllocateInfo memoryAllocateInfo);
//    void copyTo();
//    void bind();
//    void lockMem();
//    void unlockMem();
    vk::BufferCreateInfo getBufferCreateInfo();
    vk::Buffer& getBuffer(){return *buffer;}

private:
//    Renderer* renderer;
    vk::BufferUsageFlags usage;
    vk::BufferCreateInfo info;
    BufferState state {BufferState::Empty};
    uint64_t size;
    std::unique_ptr<vk::Buffer> buffer;
    vk::DeviceMemory bufferMemory;
    std::unique_ptr<vma::Allocation> allocation;
};


#endif //SHADEREDITOR_BUFFER_H
