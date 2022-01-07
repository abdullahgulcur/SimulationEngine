#include "mesh.hpp"
#include "filesystem.hpp"
#include "meshrenderer.hpp"

Mesh::MeshFile::MeshFile() {

    fileAddr = NULL;
    this->indiceSize = 0;
    Mesh::MeshFile::initEmtyBuffers();
}

Mesh::MeshFile::MeshFile(std::vector<Mesh::Vertex>& vertices, std::vector<unsigned int>& indices, File* file) {

    fileAddr = file;
    indiceSize = indices.size();
    this->vertices = vertices;
    this->indices = indices;
    Mesh::MeshFile::initBuffers(vertices, indices);
}

void Mesh::MeshFile::initBuffers(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {

    glGenVertexArrays(1, &VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

    glBindVertexArray(0);
}

void Mesh::MeshFile::initEmtyBuffers() {

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindVertexArray(0);
}

void Mesh::MeshFile::getBoxGeometryBoundaries(glm::vec3& min, glm::vec3& max) {

    min.x = 999999.f;
    min.y = 999999.f;
    min.z = 999999.f;

    max.x = -999999.f;
    max.y = -999999.f;
    max.z = -999999.f;

    for (auto& it : vertices) {

        if (it.position.x < min.x)
            min.x = it.position.x;

        if (it.position.y < min.y)
            min.y = it.position.y;

        if (it.position.z < min.z)
            min.z = it.position.z;

        if (it.position.x > max.x)
            max.x = it.position.x;

        if (it.position.y > max.y)
            max.y = it.position.y;

        if (it.position.z > max.z)
            max.z = it.position.z;
    }
}