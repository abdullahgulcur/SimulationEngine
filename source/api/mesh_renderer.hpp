#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <stddef.h>
#include <string>

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

    std::string name;

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    MeshRenderer();

    MeshRenderer(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::string name);

    void initBuffers();

    void initEmtyBuffers();
};