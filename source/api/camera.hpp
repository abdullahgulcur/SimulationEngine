#include <glm/glm.hpp>
#include <math.hpp>
#include <transform.hpp>

class Camera {

private:

public:

	float nearClip = 0.1;
	float farClip = 10000;
	int projectionType = 0;
	int fovAxis = 0;
	float fov;
	float aspectRatio = 1.77f;

	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;

	unsigned int textureBuffer;
	unsigned int FBO;
	unsigned int RBO;

	Camera();

	virtual glm::vec3 getPosition() = 0;

};