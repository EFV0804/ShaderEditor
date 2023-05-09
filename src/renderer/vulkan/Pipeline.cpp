//
// Created by elise on 09/05/2023.
//
#include "Pipeline.h"
#include "VKRenderer.h"

Pipeline::Pipeline():
pipelineLayout(),
pipeline()
{

}

void Pipeline::cleanUp() const {
    VKRenderer::Get().device.destroyDescriptorSetLayout(descriptorSetLayout);
    VKRenderer::Get().device.destroyDescriptorSetLayout(descriptorSetLayout);
    VKRenderer::Get().device.destroyPipelineLayout(pipelineLayout);
    VKRenderer::Get().device.destroyPipeline(pipeline);
}

