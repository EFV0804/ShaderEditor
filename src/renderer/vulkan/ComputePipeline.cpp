//
// Created by elise on 09/05/2023.
//
#include "ComputePipeline.h"
#include "VKRenderer.h"


ComputePipeline::ComputePipeline(vk::PipelineShaderStageCreateInfo &stage, uint32_t pSize, WorkGroups pWorkGroups, int numElements):
inBuffer{vk::BufferUsageFlagBits::eStorageBuffer, pSize, vk::SharingMode::eExclusive},
outBuffer{vk::BufferUsageFlagBits::eStorageBuffer, pSize, vk::SharingMode::eExclusive}
{
//    inBuffer{vk::BufferUsageFlagBits::eStorageBuffer, pSize}
    createComputePipeline(stage, pSize, pWorkGroups, numElements);
    int index = VKRenderer::Get().queueFamilyIndices.computeFamily;
    inBuffer.init(index);
    outBuffer.init(index);

}

void ComputePipeline::createComputePipeline(vk::PipelineShaderStageCreateInfo &stage, uint32_t pSize, WorkGroups workGroups, int numElements) {

    //DESCRIPTOR SET LAYOUT
    const std::vector<vk::DescriptorSetLayoutBinding> DescriptorSetLayoutBinding = {
            {0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute},
            {1, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute} };

    //DescriptorSet layout info
    vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutInfo(
            vk::DescriptorSetLayoutCreateFlags(),
            DescriptorSetLayoutBinding);

    SetDescriptorSetLayout(VKRenderer::Get().device.createDescriptorSetLayout(descriptorSetLayoutInfo));


    //PIPELINE LAYOUT
    vk::PipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = vk::StructureType::ePipelineLayoutCreateInfo;
    layoutInfo.pNext = nullptr;
    layoutInfo.setLayoutCount = 1;
    layoutInfo.pSetLayouts = &getDescriptorSetLayout();

//    //TODO: should come from compute shader
//    vk::PipelineShaderStageCreateInfo pipelineShaderStageInfo = {};
//    pipelineShaderStageInfo.flags = vk::PipelineShaderStageCreateFlags(),
//    pipelineShaderStageInfo.stage = vk::ShaderStageFlagBits::eCompute,
//    pipelineShaderStageInfo.pName = "main";


    SetPipelineLayout(VKRenderer::Get().device.createPipelineLayout(layoutInfo));
    vk::PipelineCache pipelineCache = VKRenderer::Get().device.createPipelineCache(vk::PipelineCacheCreateInfo());

    vk::ComputePipelineCreateInfo computePipelineInfo = {};
            computePipelineInfo.sType = vk::StructureType::eComputePipelineCreateInfo;
            computePipelineInfo.stage = stage;
            &getPipelineLayout();
    vk::Pipeline computePipeline = VKRenderer::Get().device.createComputePipeline(pipelineCache, computePipelineInfo).value; //.value to avoid implicit conversion to vk::ResultValue https://github.com/KhronosGroup/Vulkan-Hpp#return-values-error-codes--exceptions


    //Descriptor pool
    vk::DescriptorPoolSize descriptorPoolSize(vk::DescriptorType::eStorageBuffer, 2);
    vk::DescriptorPoolCreateInfo DescriptorPoolInfo(vk::DescriptorPoolCreateFlags(), 1, descriptorPoolSize);
    vk::DescriptorPool descriptorPool = VKRenderer::Get().device.createDescriptorPool(DescriptorPoolInfo);

    //Allocate descriptor sets
    vk::DescriptorSetAllocateInfo descriptorAllocateInfo(descriptorPool, 1, &getDescriptorSetLayout());
    const std::vector<vk::DescriptorSet> descriptorSets = VKRenderer::Get().device.allocateDescriptorSets(descriptorAllocateInfo);
    vk::DescriptorSet descriptorSet = descriptorSets.front();
    vk::DescriptorBufferInfo inBufferInfo(inBuffer.getBuffer(), 0, numElements * size); //TODO
    vk::DescriptorBufferInfo outBufferInfo(outBuffer.getBuffer(), 0, numElements * size); //TODO

    const std::vector<vk::WriteDescriptorSet> writeDescriptorSets = {
            {descriptorSet, 0, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &inBufferInfo},
            {descriptorSet, 1, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &outBufferInfo}
    };
    VKRenderer::Get().device.updateDescriptorSets(writeDescriptorSets, {});
}

void ComputePipeline::cleanUp() const {
    Pipeline::cleanUp();
}