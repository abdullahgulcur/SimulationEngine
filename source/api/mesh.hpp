#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <stddef.h>
#include <string>

class File;

namespace Mesh {

    struct Vertex {

        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };


    class MeshFile {

    private:

    public:

        File* fileAddr;
        unsigned int VAO;
        unsigned int indiceSize;

        MeshFile();

        MeshFile(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, File* file);

        void initBuffers(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);

        void initEmtyBuffers();
    };
}