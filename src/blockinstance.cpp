#include <cassert>
#include "blockinstance.hpp"

#define NELEMS(x) (sizeof(x) / sizeof(x[0]))

BlockInstance::BlockInstance(Texture &texture, GLuint program_id, World &world) :
	texture(texture), program_id(program_id), world(world)
{
	pos = glm::vec3(0, 0, 0);
	rot = glm::vec3(0, 0, 0);
	sca = glm::vec3(1, 1, 1);
}

BlockInstance::~BlockInstance()
{
}

void BlockInstance::setBit(int x, int y, int z, bool value)
{
	bit(x, y, z, value);
}

void BlockInstance::resetBit(int x, int y, int z, bool value)
{
	bit(x, y, z, value);
}

void BlockInstance::bit(int x, int y, int z, bool value)
{
	assert(x >= 0 && x < BLOCK_WIDTH);
	assert(y >= 0 && y < BLOCK_HEIGHT);
	assert(z >= 0 && z < BLOCK_DEPTH);

	bits[(z * BLOCK_WIDTH * BLOCK_HEIGHT) + (y * BLOCK_WIDTH) + x] = value;
}

void BlockInstance::addTop()
{
	const float vert[] = { -0.5,  0.5, -0.5,
						   -0.5,  0.5,  0.5,
						    0.5,  0.5, -0.5,
						    0.5,  0.5, -0.5,
						   -0.5,  0.5,  0.5,
						    0.5,  0.5,  0.5 };
	const float tex[] = { 0.0, 0.0,
						  0.0, 1.0,
						  1.0, 0.0,
						  1.0, 0.0,
						  0.0, 1.0,
						  1.0, 1.0 };

	for (size_t i=0; i<NELEMS(vert); i++)
	{
		m_vertices.push_back(vert[i]);

		// Point up
		m_normals.push_back(0.0);
		m_normals.push_back(1.0);
		m_normals.push_back(0.0);
	}

	for (size_t i=0; i<NELEMS(tex); i++)
	{
		m_tex_coords.push_back(tex[i]);
	}
}

void BlockInstance::addBottom()
{
	const float vert[] = { -0.5, -0.5,  0.5,
						   -0.5, -0.5, -0.5,
						    0.5, -0.5,  0.5,
						    0.5, -0.5,  0.5,
						   -0.5, -0.5, -0.5,
						    0.5, -0.5, -0.5 };
	const float tex[] = { 0.0, 0.0,
						  0.0, 1.0,
						  1.0, 0.0,
						  1.0, 0.0,
						  0.0, 1.0,
						  1.0, 1.0 };

	for (size_t i=0; i<NELEMS(vert); i++)
	{
		m_vertices.push_back(vert[i]);

		// Point down
		m_normals.push_back(0.0);
		m_normals.push_back(-1.0);
		m_normals.push_back(0.0);
	}

	for (size_t i=0; i<NELEMS(tex); i++)
	{
		m_tex_coords.push_back(tex[i]);
	}
}

void BlockInstance::addBackside()
{
	const float vert[] = {  0.5,  0.5, -0.5,
						    0.5, -0.5, -0.5,
						   -0.5,  0.5, -0.5,
						   -0.5,  0.5, -0.5,
						    0.5, -0.5, -0.5,
						   -0.5, -0.5, -0.5 };
	const float tex[] = { 0.0, 0.0,
						  0.0, 1.0,
						  1.0, 0.0,
						  1.0, 0.0,
						  0.0, 1.0,
						  1.0, 1.0 };

	for (size_t i=0; i<NELEMS(vert); i++)
	{
		m_vertices.push_back(vert[i]);

		// Point away
		m_normals.push_back(0.0);
		m_normals.push_back(0.0);
		m_normals.push_back(-1.0);
	}

	for (size_t i=0; i<NELEMS(tex); i++)
	{
		m_tex_coords.push_back(tex[i]);
	}
}

void BlockInstance::addFrontside()
{
	const float vert[] = { -0.5,  0.5,  0.5,
						   -0.5, -0.5,  0.5,
						    0.5,  0.5,  0.5,
						    0.5,  0.5,  0.5,
						   -0.5, -0.5,  0.5,
						    0.5, -0.5,  0.5 };
	const float tex[] = { 0.0, 0.0,
						  0.0, 1.0,
						  1.0, 0.0,
						  1.0, 0.0,
						  0.0, 1.0,
						  1.0, 1.0 };

	for (size_t i=0; i<NELEMS(vert); i++)
	{
		m_vertices.push_back(vert[i]);

		// Point towards
		m_normals.push_back(0.0);
		m_normals.push_back(0.0);
		m_normals.push_back(1.0);
	}

	for (size_t i=0; i<NELEMS(tex); i++)
	{
		m_tex_coords.push_back(tex[i]);
	}
}

void BlockInstance::addLeftside()
{
	const float vert[] = { -0.5,  0.5, -0.5,
						   -0.5, -0.5, -0.5,
						   -0.5,  0.5,  0.5,
						   -0.5,  0.5,  0.5,
						   -0.5, -0.5, -0.5,
						   -0.5, -0.5,  0.5 };
	const float tex[] = { 0.0, 0.0,
						  0.0, 1.0,
						  1.0, 0.0,
						  1.0, 0.0,
						  0.0, 1.0,
						  1.0, 1.0 };

	for (size_t i=0; i<NELEMS(vert); i++)
	{
		m_vertices.push_back(vert[i]);

		// Point left
		m_normals.push_back(-1.0);
		m_normals.push_back(0.0);
		m_normals.push_back(0.0);
	}

	for (size_t i=0; i<NELEMS(tex); i++)
	{
		m_tex_coords.push_back(tex[i]);
	}
}

void BlockInstance::addRightside()
{
	const float vert[] = {  0.5,  0.5,  0.5,
						    0.5, -0.5,  0.5,
						    0.5,  0.5, -0.5,
						    0.5,  0.5, -0.5,
						    0.5, -0.5,  0.5,
						    0.5, -0.5, -0.5 };
	const float tex[] = { 0.0, 0.0,
						  0.0, 1.0,
						  1.0, 0.0,
						  1.0, 0.0,
						  0.0, 1.0,
						  1.0, 1.0 };

	for (size_t i=0; i<NELEMS(vert); i++)
	{
		m_vertices.push_back(vert[i]);

		// Point right
		m_normals.push_back(1.0);
		m_normals.push_back(0.0);
		m_normals.push_back(0.0);
	}

	for (size_t i=0; i<NELEMS(tex); i++)
	{
		m_tex_coords.push_back(tex[i]);
	}
}

void BlockInstance::generateBlock()
{
	// Clear arrays and buffers
	glDeleteVertexArrays(1, &vertex_array_id);
	glDeleteBuffers(3, buffers);

	m_vertices.clear();
	m_tex_coords.clear();
	m_normals.clear();

	// Set up arrays
	addTop();
	addBottom();
	addBackside();
	addFrontside();
	addLeftside();
	addRightside();

	// Create OpenGL buffers
	glGenVertexArrays(1, &vertex_array_id);
	glBindVertexArray(vertex_array_id);

	glGenBuffers(3, buffers);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, m_tex_coords.size() * sizeof(float), m_tex_coords.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
	glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(float), m_normals.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);

	// Second attribute buffer: texture coords
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_TRUE,
		0,
		(void*)0
		);

	// Third attribute buffer: normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
	glVertexAttribPointer(
		2,
		3,
		GL_FLOAT,
		GL_TRUE,
		0,
		(void*)0
		);
}

void BlockInstance::setUniforms()
{
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(pos) *
		glm::rotate(model, rot.x, glm::vec3(1.0, 0.0, 0.0)) *
		glm::rotate(model, rot.y, glm::vec3(0.0, 1.0, 0.0)) *
		glm::rotate(model, rot.z, glm::vec3(0.0, 0.0, 1.0)) *
		glm::scale(model, sca);
	
	glm::mat4 mvp = world.camera().projection() * world.camera().view() * model;

	glUseProgram(program_id);

	world.camera().setUniform(program_id, "Camera_Pos");
	texture.setUniform(program_id, "Tex_Cube");
	world.light().setUniforms(program_id, "Light_Pos", "Light_Col");

	GLuint mvp_id = glGetUniformLocation(program_id, "MVP");
	glUniformMatrix4fv(mvp_id, 1, GL_FALSE, &mvp[0][0]);

	GLuint m_id = glGetUniformLocation(program_id, "M");
	glUniformMatrix4fv(m_id, 1, GL_FALSE, &model[0][0]);

	GLuint v_id = glGetUniformLocation(program_id, "V");
	glUniformMatrix4fv(v_id, 1, GL_FALSE, &world.camera().view()[0][0]);
}

void BlockInstance::render()
{
	texture.bind();
	glBindVertexArray(vertex_array_id);

	glDrawArrays(GL_TRIANGLES, 0, m_vertices.size() / 3);
	glBindVertexArray(0);
}

