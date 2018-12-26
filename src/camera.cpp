#include "camera.hpp"

Camera::Camera(glm::vec3 pos, float fov, float ratio) : pos(pos)
{
	orientation = glm::vec3(0, 1, 0);
	proj = glm::perspective(fov, ratio, 0.1f, 100.0f);
}

Camera::~Camera()
{
}

glm::mat4 Camera::getLookAt(glm::vec3 &lookAt)
{
	return glm::lookAt(pos, lookAt, orientation);
}

void Camera::setUniform(GLuint program_id, const char *name)
{
	GLuint id = glGetUniformLocation(program_id, name);
	glUniform3fv(id, 1, &pos[0]);
}

