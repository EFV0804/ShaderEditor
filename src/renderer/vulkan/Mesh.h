//
// Created by elise on 26/09/2022.
//

#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#include "glm/glm.hpp"
#include <vulkan/vulkan.hpp>
#include <vector>
#include <cstddef>
#include "Logger.h"
#include <unordered_map>
#include <functional>

class Buffer;

struct VertexInputDescription {
    std::vector<vk::VertexInputBindingDescription> bindingsDescriptions;
    std::vector<vk::VertexInputAttributeDescription> attributesDescriptions;
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec3 normal;

    static VertexInputDescription getVertexInputDescription();
    bool operator==(const Vertex& other) const {
        return pos == other.pos && color == other.color &&  normal == other.normal;
    }
};

template<> struct std::hash<Vertex> {
    size_t operator()(Vertex const& vertex) const {
        return ((std::hash<glm::vec3>()(vertex.pos) ^
                (std::hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^
                (std::hash<glm::vec3>()(vertex.color) << 1);
    }
};


class Mesh {
public:
    Mesh() = delete;
    Mesh(const char* pFilename):filename{pFilename}{};
//    Mesh(const Mesh&) = delete;
    Mesh &operator=(const Mesh&) = delete;
    ~Mesh() = default;

//    const int numElements = 3;

//    const std::vector<Vertex> vertices = {
//            {{-0.5f, -0.5f, 0.f}, {1.0f, 0.0f, 0.0f}},
//            {{0.5f, -0.5f, 0.f}, {0.0f, 1.0f, 0.0f}},
//            {{0.5f, 0.5f, 0.f}, {0.0f, 0.0f, 1.0f}},
//            {{-0.5f, 0.5f, 0.f}, {1.0f, 1.0f, 1.0f}}
//    };
//
//    const std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0};
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    uint64_t getSize() const {return vertices.size() * sizeof(Vertex);}
    void upload( Buffer* vertexBuffer);
    bool loadFromFile();
    const char* filename;

};
