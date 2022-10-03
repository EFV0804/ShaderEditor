//
// Created by elise on 26/09/2022.
//

#ifndef SHADEREDITOR_MESH_H
#define SHADEREDITOR_MESH_H
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include <vector>
#include <stddef.h>

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
    ~Mesh() = default;

    const uint32_t numElements = 3;

    const std::vector<Vertex> vertices = {
            {{0.0, -0.4, 0.0}, {1.0, 0.0, 0.0}},
            {{0.4, 0.4, 0.0}, {0.0, 1.0, 0.0}},
            {{-0.4, 0.4, 0.0}, {0.0, 0.0, 1.0}}
    };
    uint64_t getSize(){return vertices.size() * sizeof(Vertex);}
    void upload( Buffer* vertexBuffer);

};


#endif //SHADEREDITOR_MESH_H
