//
// Created by elise.vidal.
//

#pragma once
#include "vulkan/vulkan.hpp"

class CommandBuffer {
public:
    CommandBuffer() = default;

    ~CommandBuffer() = default;

//    CommandBuffer(const CommandBuffer &) = delete;

//    CommandBuffer &operator=(const CommandBuffer &) = delete;

    const vk::CommandBuffer& getCommandBuffer() const {return cmd;}

    void begin(vk::CommandBufferUsageFlagBits flags);

    void end();

    void reset();

    void allocate(const vk::CommandPool& pool, bool isPrimary);

    void cleanUp();
private:

    vk::CommandBuffer cmd;

};
