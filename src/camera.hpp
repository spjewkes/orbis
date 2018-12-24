#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

// Include GLEW. Always include it before gl.h and glfw.h, since it's a bit magic.
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

class Camera
{
public:
	Camera(glm::vec3 pos, float fov, float ratio);
	virtual ~Camera();

	glm::mat4 getLookAt(glm::vec3 &lookAt);
	void setUniform(GLuint id);

	glm::vec3 &position() { return pos; }
	glm::mat4 &projection() { return proj; }

private:
	glm::vec3 pos;
	glm::vec3 orientation;
	glm::mat4 proj;
};

#endif
