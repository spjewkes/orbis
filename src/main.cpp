// Include standard headers
#include <iostream>
#include <string>
#include <chrono>

// Include GLEW. Always include it before gl.h and glfw.h, since it's a bit magic.
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "options.hpp"
#include "utility.hpp"
#include "wavefront_obj.hpp"
#include "window.hpp"
#include "camera.hpp"

using namespace std;

int main(int argc, char *argv[])
{
	Options options(argc, argv);
	int width = options.width();
	int height = options.height();

	// Initialise GLFW
	if( !glfwInit() )
	{
		cerr << "Failed to initialize GLFW\n";
		return -1;
	}

	Window win = Window(width, height, "Orbis");

	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK)
	{
		cerr << "Failed to initialize GLEW\n";
		return -1;
	}

	cout << "Loading file: " << options.filepath() << endl;
	WavefrontObj object(options.filepath());
	if (options.verbose())
	{
		object.dump();
	}
	cout << "Object has " << object.numVertices() << " number of vertices\n";

	// Load texture
	cout << "Using texture: " << options.imagepath() << "\n";
	GLuint cube_texture = load_png(options.imagepath());

	// Create and compile our GLSL program from the shaders
	GLuint program_id = load_shaders( "res/vertex_shader.glsl", "res/fragment_shader.glsl" );
	if (!program_id)
	{
		cerr << "Error detected when loading shaders. Aborting.\n";
		abort();
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	float x_angle = 0.0;
	float y_angle = 0.0;
	float z_angle = 0.0;

	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();

	Camera camera = Camera(glm::vec3(3, 2, 3), glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height));

	auto light_pos = glm::vec3(3, 2, 3);
	auto light_col = glm::vec3(1, 1, 1);
	auto lookAt = glm::vec3(0, 0, 0);

	do
	{
		glm::mat4 view = camera.getLookAt(lookAt);

		// Model matrix : an identity matrix (model will be at the origin)
		glm::mat4 model = glm::mat4(1);

		// Update model to create a rotation
		model = glm::translate(glm::vec3(0, 0, 0)) *
			glm::rotate(model, x_angle, glm::vec3(1.0, 0.0, 0.0)) *
			glm::rotate(model, y_angle, glm::vec3(0.0, 1.0, 0.0)) *
			glm::rotate(model, z_angle, glm::vec3(0.0, 0.0, 1.0)) *
			glm::scale(model, glm::vec3(1, 1, 1));
	
		// our ModelViewProjection : multiplication of our 3 matrices
		glm::mat4 mvp = camera.projection() * view * model;

		glClearColor(0.25f, 0.25f, 0.25f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(program_id);

		// Set up uniforms
		GLuint mvp_id = glGetUniformLocation(program_id, "MVP");
		glUniformMatrix4fv(mvp_id, 1, GL_FALSE, &mvp[0][0]);

		GLuint m_id = glGetUniformLocation(program_id, "M");
		glUniformMatrix4fv(m_id, 1, GL_FALSE, &model[0][0]);

		GLuint v_id = glGetUniformLocation(program_id, "V");
		glUniformMatrix4fv(v_id, 1, GL_FALSE, &view[0][0]);

		// Camera uniform
		camera.setUniform(glGetUniformLocation(program_id, "Camera_Pos"));

		// Light uniforms
		GLuint light_pos_id = glGetUniformLocation(program_id, "Light_Pos");
		glUniform3fv(light_pos_id, 1, &light_pos[0]);

		GLuint light_col_id = glGetUniformLocation(program_id, "Light_Col");
		glUniform3fv(light_col_id, 1, &light_col[0]);

		// Texture uniform
		GLuint tex_id = glGetUniformLocation(program_id, "Tex_Cube");
		glUniform1i(tex_id, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cube_texture);

		object.bindBuffers();

		// Draw the array
		glDrawArrays(GL_TRIANGLES, 0, object.numVertices()); // Starting from vertex 0; 3 vertices total -> 1 triangle
		glBindVertexArray(0);

		win.swapBuffers();

		// Get time taken to draw the frame
		tp2 = chrono::system_clock::now();
		chrono::duration<float> elapsed_time = tp2 - tp1;
		tp1 = tp2;
	}
	while (!win.isKeyPressed(GLFW_KEY_ESCAPE));

	return 0;
}
