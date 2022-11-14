//
// Created by elise on 27/09/2022.
//

#include "Buffer.h"
#include "Renderer.h"
#include <iostream>

Buffer::Buffer(vk::BufferUsageFlags pUsage, uint64_t pSize) :
        usage(pUsage),
        size(pSize) {

}

void Buffer::init(uint32_t queueFamilyIndex, vk::Device &device) {

    buffer = device.createBuffer(getBufferCreateInfo(queueFamilyIndex));
    SD_RENDERER_DEBUG("Initilised buffer");
}

void Buffer::map(vk::Device &device, int offset, uint64_t dataSize) {

    SD_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, state == BufferState::Allocated,
                                "Buffer memory not allocated, can't map unallocated memory.");
    SD_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, state == BufferState::Mapped, "Buffer memory is already mapped")
    bufferStart = static_cast<float *>(device.mapMemory(bufferMemory, offset, dataSize));
    state = BufferState::Mapped;

}

void Buffer::unMap(vk::Device &device) {
    SD_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, state == BufferState::Mapped,
                                "Trying to unmap memory that was is not mapped.")
    device.unmapMemory(bufferMemory);
    state = BufferState::Allocated;
}

void Buffer::allocate(uint32_t memoryTypeIndex, vk::Device &device) {
    vk::MemoryRequirements memRequirements;
    device.getBufferMemoryRequirements(buffer, &memRequirements);

    vk::MemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType = vk::StructureType::eMemoryAllocateInfo;
    memoryAllocateInfo.allocationSize = memRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;
    memoryAllocateInfo.pNext = nullptr;

    bufferMemory = device.allocateMemory(memoryAllocateInfo);
    state = BufferState::Allocated;
}

void Buffer::bind(vk::Device &device, int memoryOffset) {
    device.bindBufferMemory(buffer, bufferMemory, memoryOffset);
}

void Buffer::copy(const void *src, uint64_t dataSize) {
    memcpy(bufferStart, src, dataSize);
}

void Buffer::destroy(vk::Device &device) {
    device.destroyBuffer(buffer);
    device.freeMemory(bufferMemory);
    state = BufferState::Destroyed;
}

vk::BufferCreateInfo Buffer::getBufferCreateInfo(const uint32_t queueFamilyIndices) {
    vk::BufferCreateInfo bufferInfo{};
    bufferInfo.sType = vk::StructureType::eBufferCreateInfo;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.pQueueFamilyIndices = &queueFamilyIndices;
    bufferInfo.queueFamilyIndexCount = 1;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;
    bufferInfo.pNext = nullptr;

    return bufferInfo;
}