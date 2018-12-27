#include "instance.hpp"

Instance::Instance(WavefrontObj &obj, Texture &tex, GLuint program_id, Light &light, Camera &camera) :
	obj(obj), tex(tex), program_id(program_id), light(light), camera(camera)
{
	pos = glm::vec3(0, 0, 0);
	rot = glm::vec3(0, 0, 0);
	sca = glm::vec3(1, 1, 1);
}

Instance::~Instance()
{
}

void Instance::setUniforms()
{
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(pos) *
		glm::rotate(model, rot.x, glm::vec3(1.0, 0.0, 0.0)) *
		glm::rotate(model, rot.y, glm::vec3(0.0, 1.0, 0.0)) *
		glm::rotate(model, rot.z, glm::vec3(0.0, 0.0, 1.0)) *
		glm::scale(model, sca);
	
	glm::mat4 mvp = camera.projection() * camera.view() * model;

	glUseProgram(program_id);

	camera.setUniform(program_id, "Camera_Pos");
	tex.setUniform(program_id, "Tex_Cube");
	light.setUniforms(program_id, "Light_Pos", "Light_Col");

	GLuint mvp_id = glGetUniformLocation(program_id, "MVP");
	glUniformMatrix4fv(mvp_id, 1, GL_FALSE, &mvp[0][0]);

	GLuint m_id = glGetUniformLocation(program_id, "M");
	glUniformMatrix4fv(m_id, 1, GL_FALSE, &model[0][0]);

	GLuint v_id = glGetUniformLocation(program_id, "V");
	glUniformMatrix4fv(v_id, 1, GL_FALSE, &camera.view()[0][0]);
}

void Instance::render()
{
	tex.bind();
	obj.bindBuffers();

	glDrawArrays(GL_TRIANGLES, 0, obj.numVertices());
	glBindVertexArray(0);
}

