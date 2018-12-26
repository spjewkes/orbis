#include "camera.hpp"

Camera::Camera(glm::vec3 pos, float fov, float ratio) : pos(pos)
{
	orientation = glm::vec3(0, 1, 0);
	proj_mat = glm::perspective(fov, ratio, 0.1f, 100.0f);
	view_mat = glm::lookAt(pos, glm::vec3(0, 0, 0), orientation);
}

Camera::~Camera()
{
}

void Camera::setLookAt(glm::vec3 &lookAt)
{
	view_mat = glm::lookAt(pos, lookAt, orientation);
}

void Camera::move(float translate, float rotate_x, float rotate_y)
{
	view_mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -translate));
	view_mat = glm::rotate(view_mat, rotate_y, glm::vec3(-1.0f, 0.0f, 0.0f));
	view_mat = glm::rotate(view_mat, rotate_x, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::setUniform(GLuint program_id, const char *name)
{
	GLuint id = glGetUniformLocation(program_id, name);
	glUniform3fv(id, 1, &pos[0]);
}

