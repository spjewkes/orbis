// Include standard headers
#include <iostream>
#include <fstream>
#include <vector>

// Include GLEW. Always include it before gl.h and glfw.h, since it's a bit magic.
#include <GL/glew.h>

using namespace std;

GLuint load_shaders(const char * vertex_file_path,const char * fragment_file_path)
{

	// Create the shaders
	GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	string vertex_shader_code;
	ifstream vertex_shader_stream(vertex_file_path, ios::in);
	if(vertex_shader_stream.is_open())
	{
		string Line = "";
		while(getline(vertex_shader_stream, Line))
		{
			vertex_shader_code += "\n" + Line;
		}
		vertex_shader_stream.close();
	}
	else
	{
		cerr << "Impossible to open " << vertex_file_path << ". Are you in the right directory? Don't forget to read the FAQ!\n";
		return 0;
	}

	// Read the Fragment Shader code from the file
	string fragment_shader_code;
	ifstream fragment_shader_stream(fragment_file_path, ios::in);
	if(fragment_shader_stream.is_open())
	{
		string Line = "";
		while(getline(fragment_shader_stream, Line))
		{
			fragment_shader_code += "\n" + Line;
		}
		fragment_shader_stream.close();
	}

	GLint result = GL_FALSE;
	int info_log_length;

	// Compile Vertex Shader
	cout << "Compiling shader: " << vertex_file_path << endl;
	char const * vertex_source_pointer = vertex_shader_code.c_str();
	glShaderSource(vertex_shader_id, 1, &vertex_source_pointer , NULL);
	glCompileShader(vertex_shader_id);

	// Check Vertex Shader
	glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
	if ( info_log_length > 0 )
	{
		vector<char> vertex_shader_error_message(info_log_length+1);
		glGetShaderInfoLog(vertex_shader_id, info_log_length, NULL, &vertex_shader_error_message[0]);
		cerr << &vertex_shader_error_message[0] << endl;
	}
	if (!result)
	{
		return 0;
	}

	// Compile Fragment Shader
	cout << "Compiling shader: " << fragment_file_path << endl;
	char const * fragment_source_pointer = fragment_shader_code.c_str();
	glShaderSource(fragment_shader_id, 1, &fragment_source_pointer , NULL);
	glCompileShader(fragment_shader_id);

	// Check Fragment Shader
	glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
	if ( info_log_length > 0 )
	{
		vector<char> fragment_shader_error_message(info_log_length+1);
		glGetShaderInfoLog(fragment_shader_id, info_log_length, NULL, &fragment_shader_error_message[0]);
		cerr << &fragment_shader_error_message[0] << endl;
	}
	if (!result)
	{
		return 0;
	}

	// Link the program
	cout << "Linking program\n";
	GLuint program_id = glCreateProgram();
	glAttachShader(program_id, vertex_shader_id);
	glAttachShader(program_id, fragment_shader_id);
	glLinkProgram(program_id);

	// Check the program
	glGetProgramiv(program_id, GL_LINK_STATUS, &result);
	glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
	if ( info_log_length > 0 )
	{
		vector<char> ProgramErrorMessage(info_log_length+1);
		glGetProgramInfoLog(program_id, info_log_length, NULL, &ProgramErrorMessage[0]);
		cerr << &ProgramErrorMessage[0] << endl;
	}
	
	glDetachShader(program_id, vertex_shader_id);
	glDetachShader(program_id, fragment_shader_id);
	
	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);

	return program_id;
}

