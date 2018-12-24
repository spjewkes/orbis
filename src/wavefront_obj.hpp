#ifndef __WAVEFRONT_OBJ_HPP__
#define __WAVEFRONT_OBJ_HPP__

// Include GLEW. Always include it before gl.h and glfw.h, since it's a bit magic.
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <vector>
#include <string>

using namespace std;

/**
 * Class for wavefront object type.
 */
class WavefrontObj
{
public:
	/// Constructors.
	WavefrontObj(const char *filename) : m_filename(filename) { generateData(); createBuffers(); }

	/// Destructors.
	~WavefrontObj() {}

	void dump();
	size_t numVertices() const { return m_vertices.size(); }

	void bindBuffers();

private:
	/// Generate data from file
	void generateData();
	void createBuffers();

	/// Instance variables
	const char *m_filename;
	vector<float> m_vertices;
	vector<float> m_tex_coords;
	vector<float> m_normals;

	GLuint vertex_buffer;
	GLuint uv_buffer;
	GLuint normal_buffer;
};

#endif // __WAVEFRONT_OBJ_HPP__
