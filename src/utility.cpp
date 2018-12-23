// Include standard headers
#include <iostream>
#include <fstream>
#include <vector>

// Include GLEW. Always include it before gl.h and glfw.h, since it's a bit magic.
#include <GL/glew.h>

// Includes for PNG
#include <png.h>
#include <zlib.h>

using namespace std;

GLuint load_png(const char *imagepath)
{
	const int header_size = 8;
	unsigned char header[header_size];

	// Open the file and check it has a PNG signature
	FILE *file = fopen(imagepath, "rb");
	if (!file)
	{
		cerr << "Image could not be opened: " << imagepath << endl;
		return 0;
	}

	if (fread(header, 1, header_size, file) != header_size)
	{
		cerr << "Failed to read PNG header bytes\n";
		return 0;
	}
	
	if (png_sig_cmp(header, 0, header_size))
	{
		cerr << "File is not a valid PNG: " << imagepath << endl;
		return 0;
	}

	// Create data structures for reading
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr)
	{
		cerr << "Failed to create libPNG header struct\n";
		return 0;
	}

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
		png_destroy_read_struct(&png_ptr, nullptr, nullptr);
		cerr << "Failed to create libPNG info struct\n";
		return 0;
    }

	png_init_io(png_ptr, file);
	png_set_sig_bytes(png_ptr, header_size);

	// Read PNG info
	png_read_info(png_ptr, info_ptr);

	int width      = png_get_image_width(png_ptr, info_ptr);
	int height     = png_get_image_height(png_ptr, info_ptr);
	auto color_type = png_get_color_type(png_ptr, info_ptr);
	auto bit_depth  = png_get_bit_depth(png_ptr, info_ptr);	

	cout << "PNG texture to be loaded: " << imagepath << endl;
	cout << "Width: " << width << endl;
	cout << "Height: " << height << endl;
	cout << "Color type: " << static_cast<int>(color_type) << endl;
	cout << "Bit depth: " << static_cast<int>(bit_depth) << endl;

	// Convert any color type to 8-bit RGBA
	if (bit_depth == 16)
	{
		png_set_strip_16(png_ptr);
	}

	if (color_type == PNG_COLOR_TYPE_PALETTE)
	{
		png_set_palette_to_rgb(png_ptr);
	}

	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
	{
		png_set_expand_gray_1_2_4_to_8(png_ptr);
	}

	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
	{
		png_set_tRNS_to_alpha(png_ptr);
	}

	if (color_type == PNG_COLOR_TYPE_RGB ||
		color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_PALETTE)
	{
		png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
	}

	if (color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	{
		png_set_gray_to_rgb(png_ptr);
	}

	png_read_update_info(png_ptr, info_ptr);

	// Now read data
	size_t row_size = png_get_rowbytes(png_ptr, info_ptr);
	png_byte *data = new png_byte[row_size * height];
	vector<png_bytep> row_pointers(height);
	for (int i=0; i<height; i++)
	{
		// Need to flip date over vertically as glTexImage2D expected data origin
		// to be from the bottom left
		row_pointers[height - i - 1] = &data[i * row_size];
	}

	png_read_image(png_ptr, row_pointers.data());

	// Now create GLES texture
	GLuint texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	// Set-up filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Clean up
	delete [] data;
	png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
	fclose(file);

	return texture_id;
}

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

