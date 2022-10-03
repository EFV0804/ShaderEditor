////
//// Created by elise on 26/09/2022.
////
#include "Mesh.h"
#include "Buffer.h"

VertexInputDescription Vertex::getVertexInputDescription(){
    VertexInputDescription description;

    vk::VertexInputBindingDescription bindingDescription {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = vk::VertexInputRate::eVertex;

    description.bindingsDescriptions.emplace_back(bindingDescription);

    vk::VertexInputAttributeDescription positionAttribDescription {};
    positionAttribDescription.binding = 0;
    positionAttribDescription.location = 0;
    positionAttribDescription.format = vk::Format::eR32G32B32Sfloat;
    positionAttribDescription.offset = offsetof(Vertex, pos);

    vk::VertexInputAttributeDescription colorAttribDescription {};
    colorAttribDescription.binding = 0;
    colorAttribDescription.location = 1;
    colorAttribDescription.format = vk::Format::eR32G32B32Sfloat;
    colorAttribDescription.offset = offsetof(Vertex, color);


    description.attributesDescriptions.emplace_back(positionAttribDescription);
    description.attributesDescriptions.emplace_back(colorAttribDescription);

    return description;
}

void Mesh::upload(vma::Allocator* allocator, Buffer* vertexBuffer) {

    // TODO copy vertices into given vertex buffer (vertex buffer is owned by renderer)
    void* data;
    vertexBuffer->map(allocator, &data);
    memcpy(data, vertices.data(), getSize());
    vertexBuffer->unMap(allocator);

}
