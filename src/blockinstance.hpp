#ifndef __BLOCK_INSTANCE_HPP__
#define __BLOCK_INSTANCE_HPP__

#include <vector>
#include <bitset>

// Include GLEW. Always include it before gl.h and glfw.h, since it's a bit magic.
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "texture.hpp"
#include "world.hpp"

constexpr int BLOCK_WIDTH = 16;
constexpr int BLOCK_DEPTH = 16;
constexpr int BLOCK_HEIGHT = 16;

using namespace std;

class BlockInstance
{
public:
	BlockInstance(Texture &texture, GLuint program_id, World &world);
	virtual ~BlockInstance();

	void setBit(int x, int y, int z, bool value = true);
	void resetBit(int x, int y, int z, bool value = false);
	void generateBlock();

	void setUniforms();
	void render();

	glm::vec3 &position() { return pos; }
	glm::vec3 &rotation() { return rot; }
	glm::vec3 &scale() { return sca; }

private:
	void bit(int x, int y, int z, bool value);
	void addTop();
	void addBottom();
	void addBackside();
	void addFrontside();
	void addLeftside();
	void addRightside();

	bitset<BLOCK_WIDTH * BLOCK_DEPTH * BLOCK_HEIGHT> bits;

	glm::vec3 pos;
	glm::vec3 rot;
	glm::vec3 sca;

	Texture &texture;
	GLuint program_id;
	World &world;

	vector<float> m_vertices;
	vector<float> m_tex_coords;
	vector<float> m_normals;

	GLuint vertex_array_id = 0;
	GLuint buffers[3] = {0};
};

#endif
