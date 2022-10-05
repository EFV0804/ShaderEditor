//
// Created by elise on 27/09/2022.
//

#include "Buffer.h"
#include "Renderer.h"
#include <iostream>

Buffer::Buffer(vk::Device* pDevice, vk::BufferUsageFlags pUsage, uint64_t pSize):
    device{pDevice},
    usage(pUsage),
    size(pSize){

}

Buffer::~Buffer() {

}

void Buffer::load(Renderer* renderer, uint32_t memoryTypeIndex) {

    info = getBufferCreateInfo(renderer);
    buffer = renderer->device.createBuffer(info);



//    createBuffer();


}

void Buffer::map(Renderer *renderer, int offset) {

    bufferStart = static_cast<float*>(renderer->device.mapMemory(bufferMemory, offset, size));

//    allocator->mapMemory(allocation, &data);
//    allocator->mapMemory(allocation);

}

void Buffer::unMap(Renderer *renderer) {
    renderer->device.unmapMemory(bufferMemory);
}

void Buffer::write() {
//    memcpy(dst, src, size);
//    unMap();
}

void Buffer::bind(Renderer *renderer, int memoryOffest) {
    renderer->device.bindBufferMemory(buffer, bufferMemory, memoryOffest);
}

void Buffer::allocate(uint32_t memoryTypeIndex, Renderer* renderer) {
    vk::MemoryRequirements memRequirements;
    renderer->device.getBufferMemoryRequirements(buffer, &memRequirements);

    vk::MemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType = vk::StructureType::eMemoryAllocateInfo;
    memoryAllocateInfo.allocationSize = memRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;
    memoryAllocateInfo.pNext = nullptr;

    bufferMemory = renderer->device.allocateMemory(memoryAllocateInfo);
}

vk::BufferCreateInfo Buffer::getBufferCreateInfo(Renderer* renderer){
    vk::BufferCreateInfo bufferInfo = {};
    bufferInfo.sType = vk::StructureType::eBufferCreateInfo;
    bufferInfo.size = size;
    bufferInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;
    bufferInfo.pQueueFamilyIndices = &renderer->queueFamilyIndices.graphicsFamily;
    bufferInfo.queueFamilyIndexCount = 1;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;
    bufferInfo.pNext = nullptr;

    return bufferInfo;
}

void Buffer::createBuffer() {
    buffer  = device->createBuffer(info);
//    buffer = std::unique_ptr<vk::Buffer>(&temp);
}

