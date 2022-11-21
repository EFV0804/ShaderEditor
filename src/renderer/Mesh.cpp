////
//// Created by elise on 26/09/2022.
////
#define TINYOBJLOADER_IMPLEMENTATION
#include "Mesh.h"
#include "Buffer.h"
#include "tiny_obj_loader.h"

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

void Mesh::upload(Buffer* vertexBuffer) {

    // TODO copy vertices into given vertex buffer (vertex buffer is owned by renderer)

    // use abstraction
    // call Buffer copy or fill or relevant Buffer func



//    void* data;
//    vertexBuffer->map(allocator, &data);
//    memcpy(data, vertices.data(), getSize());
//    vertexBuffer->unMap(allocator);

}

bool Mesh::loadFromFile() {
    //attrib will contain the vertex arrays of the file
    tinyobj::attrib_t attrib;
    //shapes contains the info for each separate object in the file
    std::vector<tinyobj::shape_t> shapes;
    //materials contains the information about the material of each shape, but we won't use it.
    std::vector<tinyobj::material_t> materials;

    //error and warning output from the load function
    std::string warn;
    std::string err;

    //load the OBJ file
    tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, R"(D:\repos\ShaderEditor\assets\models)");
    //make sure to output the warnings to the console, in case there are issues with the file
    if (!warn.empty()) {
        SD_RENDERER_WARN(warn);
    }
    //if we have any error, print it to the console, and break the mesh loading.
    //This happens if the file can't be found or is malformed
    if (!err.empty()) {
        SD_RENDERER_ERROR(err);
        return false;
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};


//            indices.push_back(indices.size());

            vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
            };
            vertex.color = vertex.normal;

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }
            indices.push_back(uniqueVertices[vertex]);


        }
    }


    return true;

}