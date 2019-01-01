#ifndef __INSTANCE_HPP__
#define __INSTANCE_HPP__

// Include GLEW. Always include it before gl.h and glfw.h, since it's a bit magic.
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "wavefront_obj.hpp"
#include "texture.hpp"
#include "world.hpp"

class Instance
{
public:
	Instance(WavefrontObj &obj, Texture &tex, GLuint program_id, World &world);
	virtual ~Instance();

	void setUniforms();
	void render();

	glm::vec3 &position() { return pos; }
	glm::vec3 &rotation() { return rot; }
	glm::vec3 &scale() { return sca; }

private:
	glm::vec3 pos;
	glm::vec3 rot;
	glm::vec3 sca;

	WavefrontObj &obj;
	Texture &tex;
	GLuint program_id;
	World &world;
};

#endif
