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
	float aspectRatio = (float)1920 / 1080;

	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;

	Camera();

	virtual glm::vec3 getPosition() = 0;

};