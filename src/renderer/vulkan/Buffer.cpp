//
// Created by elise on 27/09/2022.
//

#include "Buffer.h"
#include "VKRenderer.h"
#include <iostream>

Buffer::Buffer(vk::BufferUsageFlags pUsage, uint32_t pSize, vk::SharingMode pSharingMode) :
        usage{pUsage},
        size{pSize},
        sharingMode{pSharingMode}{

}

void Buffer::init(uint32_t queueFamilyIndex) {

    buffer = VKRenderer::Get().device.createBuffer(getBufferCreateInfo(queueFamilyIndex));
    SE_RENDERER_DEBUG("Initilised buffer");
}

void Buffer::map( int offset, uint64_t dataSize) {

    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, state == BufferState::Allocated,
                                "Buffer memory not allocated, can't map unallocated memory.");
    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, state != BufferState::Mapped, "Buffer memory is already mapped")
    bufferStart = static_cast<float *>(VKRenderer::Get().device.mapMemory(bufferMemory, offset, dataSize));
    state = BufferState::Mapped;

}

void Buffer::unMap() {
    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, state == BufferState::Mapped,
                                "Trying to unmap memory that was is not mapped.")
    VKRenderer::Get().device.unmapMemory(bufferMemory);
    state = BufferState::Allocated;
}

void Buffer::allocate(uint32_t memoryTypeIndex) {
    vk::MemoryRequirements memRequirements;
    VKRenderer::Get().device.getBufferMemoryRequirements(buffer, &memRequirements);

    vk::MemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType = vk::StructureType::eMemoryAllocateInfo;
    memoryAllocateInfo.allocationSize = memRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;
    memoryAllocateInfo.pNext = nullptr;


    bufferMemory = VKRenderer::Get().device.allocateMemory(memoryAllocateInfo);
    state = BufferState::Allocated;
}

void Buffer::bind(int memoryOffset) {
    VKRenderer::Get().device.bindBufferMemory(buffer, bufferMemory, memoryOffset);
}

void Buffer::copy(const void *src, uint64_t dataSize) {
    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, state = BufferState::Mapped, "Buffer memory is not mapped")
    memcpy(bufferStart, src, dataSize);
}

void Buffer::destroy() {
    VKRenderer::Get().device.destroyBuffer(buffer);
    VKRenderer::Get().device.freeMemory(bufferMemory);
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