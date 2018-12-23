#ifndef __WAVEFRONT_OBJ_HPP__
#define __WAVEFRONT_OBJ_HPP__

#include <vector>
#include <string>

// Include GLEW. Always include it before gl.h and glfw.h, since it's a bit magic.
#include <GL/glew.h>

/**
 * Class for wavefront object type.
 */
class WavefrontObj
{
public:
	/// Constructors.
	WavefrontObj(const char *filename) : m_filename(filename) { generate_data(); }

	/// Destructors.
	~WavefrontObj() {}

	void dump();
	size_t num_vertices() const { return m_vertices.size(); }

	// Create GL buffers
	GLuint create_vertex_buffer();
	GLuint create_tex_coord_buffer();
	GLuint create_normal_buffer();

	// Get scale value
	float get_scaler();
	
private:
	/// Generate data from file
	void generate_data();

	/// Instance variables
	const char *m_filename;
	std::vector<float> m_vertices;
	std::vector<float> m_tex_coords;
	std::vector<float> m_normals;
};

#endif // __WAVEFRONT_OBJ_HPP__
