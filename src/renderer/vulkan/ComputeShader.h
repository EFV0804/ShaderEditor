//
// Created by elise on 09/05/2023.
//

#pragma once

#include "Shader.h"
#include "ComputePipeline.h"

struct WorkGroups{
    int x{1};
    int y{1};
    int z{1};
};

class ComputeShader:
        public Shader
{
public:
    ComputeShader(std::string filename);
    ComputeShader& operator=(const ComputeShader&) = delete;
    ~ComputeShader() = default;

    const WorkGroups getWorkGroups() const {return workGroups;}

    const ComputePipeline loadPipeline();
private:
    ComputePipeline pipeline;
    uint32_t sizeDateElement;
    int numElements;
    WorkGroups workGroups;
//    WorkGroups workGroups;
    std::string filename;

};
