//
// Created by elise on 09/05/2023.
//
#pragma once
#include "Pipeline.h"
#include "Buffer.h"
#include "glm/glm.hpp"
#include "ComputeShader.h"

class VKRenderer;

//struct WorkGroups{
//    int x{1};
//    int y{1};
//    int z{1};
//};

class ComputePipeline:
        public Pipeline
{
public:
    ComputePipeline(vk::PipelineShaderStageCreateInfo &stage, uint32_t pSize, WorkGroups pWorkGroups, int numElements);
//    virtual ComputePipeline &operator=(const Pipeline &) = delete;
    ~ComputePipeline() = default;

//    const WorkGroups& getWorkGroups() const {return workGroups;}

    void cleanUp() const override;

private:
    void createComputePipeline(vk::PipelineShaderStageCreateInfo &stage, uint32_t pSize, WorkGroups workGroups, int numElements);
    Buffer inBuffer;
    Buffer outBuffer;
    uint32_t size;
//    WorkGroups workGroups;

};

