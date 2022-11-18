//
// Created by elise on 26/09/2022.
//

#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include <vector>
#include <cstddef>

class Buffer;

struct VertexInputDescription {
    std::vector<vk::VertexInputBindingDescription> bindingsDescriptions;
    std::vector<vk::VertexInputAttributeDescription> attributesDescriptions;
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    static VertexInputDescription getVertexInputDescription();
};

class Mesh {
public:
    Mesh() = default;
//    Mesh(const Mesh&) = delete;
    Mesh &operator=(const Mesh&) = delete;
    ~Mesh() = default;

//    const int numElements = 3;

    const std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f, 0.f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, -0.5f, 0.f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f, 0.5f, 0.f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.f}, {1.0f, 1.0f, 1.0f}}
    };
    std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};

    uint64_t getSize() const {return vertices.size() * sizeof(Vertex);}
    void upload( Buffer* vertexBuffer);

};
