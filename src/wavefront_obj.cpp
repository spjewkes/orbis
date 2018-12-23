#include <fstream>
#include <iostream>
#include <sstream>
#include <limits>
#include <cmath>
#include "wavefront_obj.hpp"

using namespace std;

/// Generate data from file
void WavefrontObj::generate_data()
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

GLuint WavefrontObj::create_vertex_buffer()
{
	GLuint id;
	glGenBuffers(1, &id);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_STATIC_DRAW);
	return id;
}

GLuint WavefrontObj::create_tex_coord_buffer()
{
	GLuint id;
	glGenBuffers(1, &id);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, m_tex_coords.size() * sizeof(float), m_tex_coords.data(), GL_STATIC_DRAW);
	return id;
}

GLuint WavefrontObj::create_normal_buffer()
{
	GLuint id;
	glGenBuffers(1, &id);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(float), m_normals.data(), GL_STATIC_DRAW);
	return id;
}

float WavefrontObj::get_scaler()
{
	// The scaler tries to give an idea of how to scale the box based on the diagonal length
	// of a cube that tightly surrounds the object. This enables the program to try and scale
	// objects as best as possible for the viewer

	float xmin = numeric_limits<float>::max();
	float ymin = numeric_limits<float>::max();
	float zmin = numeric_limits<float>::max();
	float xmax = numeric_limits<float>::min();
	float ymax = numeric_limits<float>::min();
	float zmax = numeric_limits<float>::min();

	const size_t count = m_vertices.size();
	for (size_t i = 0; i<count; i+=3)
	{
		xmin = min(xmin, m_vertices[i+0]);
		xmax = max(xmax, m_vertices[i+0]);
		ymin = min(ymin, m_vertices[i+1]);
		ymax = max(ymax, m_vertices[i+1]);
		zmin = min(zmin, m_vertices[i+2]);
		zmax = max(zmax, m_vertices[i+2]);
	}

	float x = xmax - xmin;
	float y = ymax - ymin;
	float z = zmax - zmin;

	return sqrtf((x * x) + (y * y) + (z * z));
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
