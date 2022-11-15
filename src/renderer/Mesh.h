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
    Mesh(const Mesh&) = delete;
    Mesh &operator=(const Mesh&) = delete;
    ~Mesh() = default;

//    const int numElements = 3;

    const std::vector<Vertex> vertices = {
            {{0.0, -0.4, 0.0}, {1.0, 0.0, 0.0}},
            {{0.4, 0.4, 0.0}, {0.0, 1.0, 0.0}},
            {{-0.4, 0.4, 0.0}, {0.0, 0.0, 1.0}}
    };
    uint64_t getSize(){return vertices.size() * sizeof(Vertex);}
    void upload( Buffer* vertexBuffer);

};
