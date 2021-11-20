#include "mesh.hpp"
#include "filesystem.hpp"

Mesh::MeshFile::MeshFile() {

    fileAddr = NULL;
    this->indiceSize = 0;
    Mesh::MeshFile::initEmtyBuffers();
}

Mesh::MeshFile::MeshFile(std::vector<Mesh::Vertex>& vertices, std::vector<unsigned int>& indices, File* file) {

    fileAddr = file;
    indiceSize = indices.size();
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