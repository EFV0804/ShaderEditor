//
// Created by elise on 27/09/2022.
//

#include "Buffer.h"
#include "Renderer.h"
#include <iostream>

Buffer::Buffer(vk::BufferUsageFlags pUsage, uint64_t pSize, vk::SharingMode pSharingMode) :
        usage{pUsage},
        size{pSize},
        sharingMode{pSharingMode}{

}

void Buffer::init(uint32_t queueFamilyIndex) {

    buffer = Renderer::Get().device.createBuffer(getBufferCreateInfo(queueFamilyIndex));
    SD_RENDERER_DEBUG("Initilised buffer");
}

void Buffer::map( int offset, uint64_t dataSize) {

    SD_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, state == BufferState::Allocated,
                                "Buffer memory not allocated, can't map unallocated memory.");
    SD_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, state != BufferState::Mapped, "Buffer memory is already mapped")
    bufferStart = static_cast<float *>(Renderer::Get().device.mapMemory(bufferMemory, offset, dataSize));
    state = BufferState::Mapped;

}

void Buffer::unMap() {
    SD_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, state == BufferState::Mapped,
                                "Trying to unmap memory that was is not mapped.")
    Renderer::Get().device.unmapMemory(bufferMemory);
    state = BufferState::Allocated;
}

void Buffer::allocate(uint32_t memoryTypeIndex) {
    vk::MemoryRequirements memRequirements;
    Renderer::Get().device.getBufferMemoryRequirements(buffer, &memRequirements);

    vk::MemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType = vk::StructureType::eMemoryAllocateInfo;
    memoryAllocateInfo.allocationSize = memRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;
    memoryAllocateInfo.pNext = nullptr;

    bufferMemory = Renderer::Get().device.allocateMemory(memoryAllocateInfo);
    state = BufferState::Allocated;
}

void Buffer::bind(int memoryOffset) {
    Renderer::Get().device.bindBufferMemory(buffer, bufferMemory, memoryOffset);
}

void Buffer::copy(const void *src, uint64_t dataSize) {
    memcpy(bufferStart, src, dataSize);
}

void Buffer::destroy() {
    Renderer::Get().device.destroyBuffer(buffer);
    Renderer::Get().device.freeMemory(bufferMemory);
    state = BufferState::Destroyed;
}

vk::BufferCreateInfo Buffer::getBufferCreateInfo(const uint32_t queueFamilyIndices) {
    vk::BufferCreateInfo bufferInfo{};
    bufferInfo.sType = vk::StructureType::eBufferCreateInfo;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.pQueueFamilyIndices = &queueFamilyIndices;
    bufferInfo.queueFamilyIndexCount = 1;
    bufferInfo.sharingMode = sharingMode;
    bufferInfo.pNext = nullptr;

    return bufferInfo;
}