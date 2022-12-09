//
// Created by elise.vidal.
//

#include "CommandBuffer.h"
#include "VKRenderer.h"
void CommandBuffer::begin(vk::CommandBufferUsageFlagBits flags){
    vk::CommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
    commandBufferBeginInfo.flags = flags;
    cmd.begin(commandBufferBeginInfo);
}

void CommandBuffer::end(){
    cmd.end();
}

void CommandBuffer::reset(){
    cmd.reset();
}

void CommandBuffer::allocate(const vk::CommandPool& pool, bool isPrimary){
    vk::CommandBufferLevel level;

    if(isPrimary){
        level= vk::CommandBufferLevel::ePrimary;
    }
    else{
        level =vk::CommandBufferLevel::eSecondary;
    }

    vk::CommandBufferAllocateInfo commandBufferAllocInfo = {};
    commandBufferAllocInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
    commandBufferAllocInfo.commandPool = pool;
    commandBufferAllocInfo.commandBufferCount = 1;
    commandBufferAllocInfo.level = level;

    cmd = VKRenderer::Get().device.allocateCommandBuffers(commandBufferAllocInfo).at(0);
}

void CommandBuffer::cleanUp(){

}