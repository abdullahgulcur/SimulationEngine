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
	glm::mat4 projectionViewMatrix;
	glm::vec3 direction;

	glm::vec4 planes[6];

	unsigned int textureBuffer;
	unsigned int FBO;
	unsigned int RBO;

	Camera();

	void frustum(glm::mat4& view_projection);

	bool intersectsAABB(glm::vec3 start, glm::vec3 end);

	virtual glm::vec3 getPosition() = 0;

};