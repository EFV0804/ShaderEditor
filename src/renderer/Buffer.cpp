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

Buffer::~Buffer() {
//    renderer->allocator.destroyBuffer(buffer, allocation);
}

void Buffer::load(vma::Allocator* allocator) {


    info = getBufferCreateInfo();
//    createBuffer();

    vma::AllocationCreateInfo vmaAllocCreateInfo = {};
    vmaAllocCreateInfo.usage = vma::MemoryUsage::eCpuToGpu;
//    vmaAllocCreateInfo.requiredFlags = vk::MemoryPropertyFlagBits::eHostVisible;


//    vma::AllocationInfo allocInfo = {};
//    allocInfo.deviceMemory = renderer->device;
//    allocInfo.memoryType = nullptr;

//    renderer->allocator.getAllocationInfo(allocation);

//    vmaGetAllocatorInfo(renderer->allocator, renderer->allocator.getAllocatorInfo());
    vma::AllocationInfo allocInfo = {};
    allocator->createBuffer(info, allocInfo);
//    allocator->createBuffer( &info,
//                                      &vmaAllocInfo,
//                                      &buffer,
//                                      &allocation,
//                                      nullptr);

}

void Buffer::map(vma::Allocator* allocator, void* data){
//    allocator->mapMemory(allocation, &data);
    allocator->mapMemory(allocation);

}

void Buffer::unMap(vma::Allocator* allocator){
    allocator->unmapMemory(allocation);
}
//void Buffer::copyTo() {
//
//}
//
//void Buffer::bind() {
//
//}

//void Buffer::lockMem() {
//
//}
//
//void Buffer::unlockMem() {
//
//}

//void Buffer::allocate(vk::MemoryAllocateInfo memoryAllocateInfo) {
//    bufferMemory = renderer->device.allocateMemory(memoryAllocateInfo);
//}
vk::BufferCreateInfo Buffer::getBufferCreateInfo(){
    vk::BufferCreateInfo bufferInfo = {};
    bufferInfo.sType = vk::StructureType::eBufferCreateInfo;
    bufferInfo.size = size;
    bufferInfo.usage = usage;

    return bufferInfo;
}

//void Buffer::createBuffer() {
//    buffer = renderer->device.createBuffer(info);
//}
