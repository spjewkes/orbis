#include "camera.hpp"

Camera::Camera(glm::vec3 pos,  glm::vec2 pitch_yaw, float fov, float ratio) : pos(pos), pitch_yaw(pitch_yaw)
{
	orientation = glm::vec3(0, 1, 0);
	proj_mat = glm::perspective(fov, ratio, 0.1f, 100.0f);
	move(0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
}

Camera::~Camera()
{
}

void Camera::setLookAt(glm::vec3 &lookAt)
{
	view_mat = glm::lookAt(pos, lookAt, orientation);
}

void Camera::move(float move_x, float move_y, float move_z, float rotate_x, float rotate_y)
{
	glm::vec3 strafe(view_mat[0][0], view_mat[1][0], view_mat[2][0]);
	glm::vec3 height(view_mat[0][1], view_mat[1][1], view_mat[1][2]);
	glm::vec3 forward(view_mat[0][2], view_mat[1][2], view_mat[2][2]);
	pos += move_x * strafe + move_y * height + move_z * forward;

	pitch_yaw.x += rotate_x;
	pitch_yaw.y += rotate_y;

	glm::mat4 pitch = glm::mat4(1.0f);
	pitch = glm::rotate(pitch, pitch_yaw.x, glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 yaw = glm::mat4(1.0f);
	yaw = glm::rotate(yaw, pitch_yaw.y, glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, -pos);

	view_mat = pitch * yaw * trans;
}

void Camera::setUniform(GLuint program_id, const char *name)
{
	GLuint id = glGetUniformLocation(program_id, name);
	glUniform3fv(id, 1, &pos[0]);
}

