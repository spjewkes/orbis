#include "light.hpp"

Light::Light(glm::vec3 pos, glm::vec3 col) : pos(pos), col(col)
{
}

Light::~Light()
{
}

void Light::setUniforms(GLuint program_id, const char *pos_name, const char *col_name)
{
	GLuint pos_id = glGetUniformLocation(program_id, pos_name);
	glUniform3fv(pos_id, 1, &pos[0]);

	GLuint col_id = glGetUniformLocation(program_id, col_name);
	glUniform3fv(col_id, 1, &col[0]);
}

