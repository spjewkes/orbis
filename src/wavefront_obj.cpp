#include <fstream>
#include <iostream>
#include <sstream>
#include <limits>
#include "wavefront_obj.hpp"

using namespace std;

void WavefrontObj::generateData()
{
	ifstream file(m_filename, ifstream::in);
	string line;
	unsigned line_num = 0;

	vector<float> vertices;
	vector<float> tex_coords;
	vector<float> normals;

	while (file.good())
	{
		getline(file, line);
		line_num++;

		istringstream in(line);

		string type;
		in >> type;

		if (type == "#")
		{
			// Comment
			continue;
		}
		else if (type == "v")
		{
			// Vertex
			float x, y, z, w;
			x = y = z = 0.0f;
			w = 1.0f;
	  		in >> x >> y >> z >> w;
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
		}
		else if (type == "vt")
		{
			// Texture vertices
			float u, v, w;
			u = v = w = 0.0f;
			in >> u >> v >> w;
			tex_coords.push_back(u);
			tex_coords.push_back(v);
		}
		else if (type == "vn")
		{
			// Vertex normals
			float dx, dy, dz;
			dx = dy = dz = 0.0f;
			in >> dx >> dy >> dz;
			normals.push_back(dx);
			normals.push_back(dy);
			normals.push_back(dz);
		}
		else if (type == "f")
		{
			vector<unsigned> f;
			vector<unsigned> ft;
			vector<unsigned> fn;
			unsigned tmp;

			while (!in.eof())
			{
				// Vertex
				in >> tmp;
				f.push_back(tmp);

				// Texture coordinate
				if (in.peek() != '/')
				{
					continue;
				}
				else
				{
					in.ignore();
					if (in.peek() != '/')
					{
						in >> tmp;
						ft.push_back(tmp);
					}
				}

				// Normal
				if (in.peek() != '/')
				{
					continue;
				}
				else
				{
					in.ignore();
					in >> tmp;
					fn.push_back(tmp);
				}
			}

			// Now store values
			// Assume only triangles for now
			if (f.size() >= 3)
			{
				// OBJ indices start at 1 not zero
				for (int i=0; i<3; i++)
				{
					size_t findex = (f[i]-1) * 3;
					m_vertices.push_back(vertices[findex+0]); // X
					m_vertices.push_back(vertices[findex+1]); // Y
					m_vertices.push_back(vertices[findex+2]); // Z
				}

				if (ft.size() > 0)
				{
					for (int i=0; i<3; i++)
					{
						size_t findex = (ft[i]-1) * 2;
						m_tex_coords.push_back(tex_coords[findex+0]); // U
						m_tex_coords.push_back(tex_coords[findex+1]); // V
					}
				}

				if (fn.size() > 0)
				{
					for (int i=0; i<3; i++)
					{
						size_t findex = (fn[i]-1) * 3;
						m_normals.push_back(normals[findex+0]); // dX
						m_normals.push_back(normals[findex+1]); // dY
						m_normals.push_back(normals[findex+2]); // dZ
					}
				}
			}
		}
	}
}

void WavefrontObj::createBuffers()
{
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &uv_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
	glBufferData(GL_ARRAY_BUFFER, m_tex_coords.size() * sizeof(float), m_tex_coords.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &normal_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
	glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(float), m_normals.data(), GL_STATIC_DRAW);
}

void WavefrontObj::bindBuffers()
{
	// First attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No rtexparticular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);

	// Second attribute buffer: texture coords
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
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
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
	glVertexAttribPointer(
		2,
		3,
		GL_FLOAT,
		GL_TRUE,
		0,
		(void*)0
		);
}

void WavefrontObj::dump()
{
	cout << "Vertices:\n";
	for (size_t i=0; i<m_vertices.size(); i+=3)
	{
		cout << i/3 << "   X: " << m_vertices[i] << "   Y: " << m_vertices[i+1] << "   Z: " << m_vertices[i+2] << endl;
	}

	cout << "Texture Coords:\n";
	for (size_t i=0; i<m_tex_coords.size(); i+=2)
	{
		cout << i/2 << "   U: " << m_tex_coords[i] << "   V: " << m_tex_coords[i+1] << endl;
	}

	cout << "Normals:\n";
	for (size_t i=0; i<m_normals.size(); i+=3)
	{
		cout << i/3 << "   dX: " << m_normals[i] << "   dY: " << m_normals[i+1] << "   dZ: " << m_normals[i+2] << endl;
	}
}
