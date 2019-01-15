#include <png.h>
#include <zlib.h>
#include <iostream>
#include <vector>

#include "texture.hpp"

using namespace std;

Texture::Texture(const char *filename, GLuint unit, bool linearfiltering) : unit(unit), linearfiltering(linearfiltering)
{
	id = load_png(filename);
}

Texture::~Texture()
{
	glDeleteTextures(1, &id);
}
	
void Texture::setUniform(GLuint program_id, const char *name)
{
	uniform_id = glGetUniformLocation(program_id, name);
	glUniform1i(uniform_id, unit);
}

void Texture::bind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, id);
}

GLuint Texture::load_png(const char*filename)
{
	const int header_size = 8;
	unsigned char header[header_size];

	// Open the file and check it has a PNG signature
	FILE *file = fopen(filename, "rb");
	if (!file)
	{
		cerr << "Image could not be opened: " << filename << endl;
		return 0;
	}

	if (fread(header, 1, header_size, file) != header_size)
	{
		cerr << "Failed to read PNG header bytes\n";
		return 0;
	}
	
	if (png_sig_cmp(header, 0, header_size))
	{
		cerr << "File is not a valid PNG: " << filename << endl;
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

	cout << "PNG texture to be loaded: " << filename << endl;
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
	if (linearfiltering)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	// Clean up
	delete [] data;
	png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
	fclose(file);

	return texture_id;
}
