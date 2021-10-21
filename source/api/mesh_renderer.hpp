#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <stddef.h>

struct Vertex {

    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

class MeshRenderer {

private:

    

public:

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    MeshRenderer(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

    void initBuffers();
};