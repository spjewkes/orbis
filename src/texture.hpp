#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

// Include GLEW. Always include it before gl.h and glfw.h, since it's a bit magic.
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

class Texture
{
public:
	Texture(const char *filename, GLuint unit);
	virtual ~Texture();
	
	void setUniform(GLuint program_id, const char *name);
	void bind();

private:
	GLuint load_png(const char*filename);

	GLuint unit;
	GLuint id;
	GLuint uniform_id;
};

#endif
