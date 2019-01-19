#include <cassert>
#include "blockinstance.hpp"

#define NELEMS(x) (sizeof(x) / sizeof(x[0]))

BlockInstance::BlockInstance(Texture &texture, GLuint program_id, World &world) :
	bits(BLOCK_WIDTH * BLOCK_DEPTH * BLOCK_HEIGHT, Block::Empty), texture(texture), program_id(program_id), world(world)
{
	pos = glm::vec3(0, 0, 0);
	rot = glm::vec3(0, 0, 0);
	sca = glm::vec3(1, 1, 1);
}

BlockInstance::~BlockInstance()
{
}

void BlockInstance::setBit(int x, int y, int z, Block type)
{
	assert(x >= 0 && x < BLOCK_WIDTH);
	assert(y >= 0 && y < BLOCK_HEIGHT);
	assert(z >= 0 && z < BLOCK_DEPTH);

	bits[(z * BLOCK_WIDTH * BLOCK_HEIGHT) + (y * BLOCK_WIDTH) + x] = type;
}

void BlockInstance::resetBit(int x, int y, int z)
{
	setBit(x, y, z, Block::Empty);
}

void BlockInstance::addFace(Face face, int texsel, float xoffset, float yoffset, float zoffset)
{
	for (size_t i=0; i<NumVertices; i++)
	{
			m_vertices.push_back(vertices[face][i*3+0] + xoffset);
			m_vertices.push_back(vertices[face][i*3+1] + yoffset);
			m_vertices.push_back(vertices[face][i*3+2] + zoffset);

			m_normals.push_back(normals[face][0]);
			m_normals.push_back(normals[face][1]);
			m_normals.push_back(normals[face][2]);
	}

	float texx_scale = 1.0f / 16.0f;
	float texy_scale = 1.0f / 16.0f;
	float texx = static_cast<float>(texsel % 16) * texx_scale;
	float texy = static_cast<float>(texsel / 16) * texy_scale;

	for (size_t i=0; i<NELEMS(textures); i+=2)
	{
		m_tex_coords.push_back(textures[i] * texx_scale + texx);
		m_tex_coords.push_back(1.0f - (textures[i+1] * texy_scale + texy));
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

	int offset = 0;
	for (int z=0; z<BLOCK_DEPTH; z++)
	{
		for (int y=0; y<BLOCK_HEIGHT; y++)
		{
			for (int x=0; x<BLOCK_WIDTH; x++)
			{
				Block blockType = bits[offset];

				if (blockType != Block::Empty)
				{
					float xoffset = static_cast<float>(x);
					float yoffset = static_cast<float>(y);
					float zoffset = static_cast<float>(z);

					// Set up arrays
					addFace(FaceTop, blockIndices[blockType][0], xoffset, yoffset, zoffset);
					addFace(FaceBottom, blockIndices[blockType][1], xoffset, yoffset, zoffset);
					addFace(FaceBack, blockIndices[blockType][2], xoffset, yoffset, zoffset);
					addFace(FaceFront, blockIndices[blockType][3], xoffset, yoffset, zoffset);
					addFace(FaceLeft, blockIndices[blockType][4], xoffset, yoffset, zoffset);
					addFace(FaceRight, blockIndices[blockType][5], xoffset, yoffset, zoffset);
				}

				offset++;
			}
		}
	}

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

