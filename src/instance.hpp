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
#include "camera.hpp"
#include "texture.hpp"
#include "light.hpp"


class Instance
{
public:
	Instance(WavefrontObj &obj, Texture &tex, GLuint program_id, Light &light, Camera &camera);
	virtual ~Instance();

	void setUniforms();
	void render();

private:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	WavefrontObj &obj;
	Texture &tex;
	GLuint program_id;
	Light &light;
	Camera &camera;
};

#endif
