//
// Created by elise on 27/09/2022.
//

#ifndef SHADEREDITOR_BUFFER_H
#define SHADEREDITOR_BUFFER_H
#include <Vulkan/vulkan.hpp>
//#include <vk_mem_alloc.hpp>
 #include <memory>

class Renderer;

//namespace vma{
//    class Allocation;
//}

enum BufferState{
    Empty,
    Allocated,
    Locked,
    Destroyed
};

class Buffer {
public:
    Buffer(vk::Device* pDevice, vk::BufferUsageFlags pUsage, uint64_t pSize);
    Buffer() = default;

    ~Buffer();
    void createBuffer();
    void load(Renderer* renderer, uint32_t memoryTypeIndex);
    void map(Renderer *renderer, int offset);
    void unMap(Renderer *renderer);
    void allocate(uint32_t memoryTypeIndex, Renderer *renderer);
    void write();
    void bind(Renderer *renderer, int memoryOffest = 0);
//    void lockMem();
//    void unlockMem();
    vk::BufferCreateInfo getBufferCreateInfo(Renderer* renderer);
    vk::Buffer& getBuffer(){return buffer;}
    vk::BufferUsageFlags usage;
    float* bufferStart = nullptr;
    uint64_t size;
    vk::DeviceMemory* getDeviceMemory(){return &bufferMemory;}
    vk::DeviceMemory bufferMemory;

private:
    vk::BufferCreateInfo info;
    BufferState state {BufferState::Empty};
    vk::Buffer buffer;
    vk::MemoryAllocateInfo memoryAllocateInfo;
    vk::Device* device;

//    std::unique_ptr<vma::Allocation> allocation;
};


#endif //SHADEREDITOR_BUFFER_H
