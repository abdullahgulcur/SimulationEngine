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
};

class FileSystem;

class Mesh {

private:

public:

    unsigned int VAO;
    unsigned int indiceSize;

    Mesh();

    Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, char* name, FileSystem* fileSystem);

    void initBuffers(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, char* name, FileSystem* fileSystem);

    void initEmtyBuffers();
};