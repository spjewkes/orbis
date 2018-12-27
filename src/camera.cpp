#include "camera.hpp"

Camera::Camera(glm::vec3 pos,  glm::vec3 rot, float fov, float ratio) : pos(pos), rot(rot)
{
	orientation = glm::vec3(0, 1, 0);
	proj_mat = glm::perspective(fov, ratio, 0.1f, 100.0f);

	glm::vec3 move_non(0, 0, 0);
	glm::vec3 rotate_non(0, 0, 0);
	move(move_non, rotate_non);
}

Camera::~Camera()
{
}

void Camera::setLookAt(glm::vec3 &lookAt)
{
	view_mat = glm::lookAt(pos, lookAt, orientation);
}

void Camera::move(glm::vec3 &move, glm::vec3 &rotate)
{
	glm::vec3 strafe(view_mat[0][0], view_mat[1][0], view_mat[2][0]);
	glm::vec3 height(view_mat[0][1], view_mat[1][1], view_mat[1][2]);
	glm::vec3 forward(view_mat[0][2], view_mat[1][2], view_mat[2][2]);
	pos += move.x * strafe + move.y * height + move.z * forward;

	rot.x += rotate.x;
	rot.y += rotate.y;
	rot.z += rotate.z;

	glm::mat4 pitch = glm::mat4(1.0f);
	pitch = glm::rotate(pitch, rot.x, glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 yaw = glm::mat4(1.0f);
	yaw = glm::rotate(yaw, rot.y, glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 roll = glm::mat4(1.0f);
	roll = glm::rotate(roll, rot.z, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, -pos);

	view_mat = roll * pitch * yaw * trans;
}

void Camera::setUniform(GLuint program_id, const char *name)
{
	GLuint id = glGetUniformLocation(program_id, name);
	glUniform3fv(id, 1, &pos[0]);
}

