//
// Created by elise on 27/09/2022.
//

#include "Buffer.h"
#include "Renderer.h"
#include <iostream>

Buffer::Buffer(vk::BufferUsageFlags pUsage, uint64_t pSize):
    usage(pUsage),
    size(pSize){

}

void Buffer::load(Renderer* renderer) {

    uint32_t queueFamilyIndex = renderer->queueFamilyIndices.graphicsFamily;

    info = getBufferCreateInfo(queueFamilyIndex);
    buffer = renderer->device.createBuffer(info);
}

void Buffer::map(Renderer *renderer, int offset, uint64_t dataSize) {

    bufferStart = static_cast<float*>(renderer->device.mapMemory(bufferMemory, offset, dataSize));
    state = BufferState::Mapped;

}

void Buffer::unMap(Renderer *renderer) {
    renderer->device.unmapMemory(bufferMemory);
    state = BufferState::Allocated;
}

void Buffer::allocate(uint32_t memoryTypeIndex, Renderer* renderer) {
    vk::MemoryRequirements memRequirements;
    renderer->device.getBufferMemoryRequirements(buffer, &memRequirements);

    memoryAllocateInfo.sType = vk::StructureType::eMemoryAllocateInfo;
    memoryAllocateInfo.allocationSize = memRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;
    memoryAllocateInfo.pNext = nullptr;

    bufferMemory = renderer->device.allocateMemory(memoryAllocateInfo);
    state = BufferState::Allocated;
}

void Buffer::bind(Renderer *renderer, int memoryOffset) {
    renderer->device.bindBufferMemory(buffer, bufferMemory, memoryOffset);
}

void Buffer::copy(const void* src, uint64_t dataSize){
    memcpy(bufferStart, src, dataSize);
}

void Buffer::destroy(vk::Device& device) {
    device.destroyBuffer(buffer);
    device.freeMemory(bufferMemory);
    state = BufferState::Destroyed;
}

vk::BufferCreateInfo Buffer::getBufferCreateInfo(const uint32_t queueFamilyIndices){
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