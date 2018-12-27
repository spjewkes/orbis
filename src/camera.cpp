#include "camera.hpp"

Camera::Camera(glm::vec3 pos,  glm::vec2 pitch_yaw, float fov, float ratio) : pos(pos), pitch_yaw(pitch_yaw)
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
	pos.z += translate;
	pitch_yaw.x += rotate_x;
	pitch_yaw.y += rotate_y;

    float cos_x = cos(pitch_yaw.x);
    float sin_x = sin(pitch_yaw.x);
    float cos_y = cos(pitch_yaw.y);
    float sin_y = sin(pitch_yaw.y);

	glm::vec3 x_axis = { cos_y, 0, -sin_y };
	glm::vec3 y_axis = { sin_y * sin_x, cos_x, cos_y * sin_x };
	glm::vec3 z_axis = { sin_y * cos_x, -sin_x, cos_x * cos_y };
 
	view_mat = glm::mat4(glm::vec4( x_axis.x, y_axis.x, z_axis.x, 0 ),
						 glm::vec4( x_axis.y, y_axis.y, z_axis.y, 0 ),
						 glm::vec4( x_axis.z, y_axis.z, z_axis.z, 0 ),
						 glm::vec4( -glm::dot( x_axis, pos ), -glm::dot( y_axis, pos ), -glm::dot( z_axis, pos ), 1 ));
     
}

void Camera::setUniform(GLuint program_id, const char *name)
{
	GLuint id = glGetUniformLocation(program_id, name);
	glUniform3fv(id, 1, &pos[0]);
}

