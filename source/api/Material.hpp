#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include "PxPhysicsAPI.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

struct File;
class MeshRenderer;
class Collider;

using namespace physx;

namespace Material {

	class MaterialFile {

	public:

		// Real Material Address

		File* fileAddr;
		File* vertShaderFileAddr;
		File* fragShaderFileAddr;

		std::vector<MeshRenderer*> meshRendererCompAddrs;
		std::vector<File*> textureUnitFileAddrs;
		std::vector<unsigned int> textureUnits;

		std::vector<float> floatUnits;

		unsigned int programID;

		MaterialFile();

		MaterialFile(const char* vertex_file_path, const char* fragment_file_path);

		MaterialFile(File* file, File* vertShaderFileAddr, File* fragShaderFileAddr, const char* vertex_file_path, const char* fragment_file_path, int dirLightCount, int pointLightCount);

		void compileShaders(const char* vertex_file_path, const char* fragment_file_path, int dirLightCount, int pointLightCount);

		void deleteProgram();
	};

	class PhysicMaterialFile {

	public:

		File* fileAddr;
		PxMaterial* pxmat;
		std::vector<Collider*> colliderCompAddrs;

		PhysicMaterialFile();

		PhysicMaterialFile(PxPhysics* gPhysics);

		PhysicMaterialFile(File* file, PxPhysics* gPhysics);

		void destroy();
	};
}

