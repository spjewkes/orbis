#ifndef __LIGHT_HPP__
#define __LIGHT_HPP__

// Include GLEW. Always include it before gl.h and glfw.h, since it's a bit magic.
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

class Light
{
public:
	Light(glm::vec3 pos, glm::vec3 col);
	virtual ~Light();

	void setUniforms(GLuint program_id, const char *pos_name, const char *col_name);

	glm::vec3 &position() { return pos; }
	glm::vec3 &color() { return col; }

private:
	glm::vec3 pos;
	glm::vec3 col;
};

#endif
