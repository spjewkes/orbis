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

using namespace std;

float g_zoom = 3.0f;

void scroll_callback(GLFWwindow *, double, double yoffset)
{
	g_zoom += (yoffset / 10.0f);

	if (g_zoom < 0.1f)
	{
		g_zoom = 0.1f;
	}
	else if (g_zoom > 100.0f)
	{
		g_zoom = 100.0f;
	}
}

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

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Set up for OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

	// Open a window and create its OpenGL context
	GLFWwindow* window;
	window = glfwCreateWindow( width, height, "OpenGL Object Viewer", NULL, NULL);
	if( window == NULL )
	{
		cerr << "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window); // Initialize GLEW
	glewExperimental=true; // Needed in core profile
	if (glewInit() != GLEW_OK)
	{
		cerr << "Failed to initialize GLEW\n";
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Create the buffers
	GLuint vertex_array_id;
	glGenVertexArrays(1, &vertex_array_id);
	glBindVertexArray(vertex_array_id);

	cout << "Loading file: " << options.filepath() << endl;
	WavefrontObj object(options.filepath());
	if (options.verbose())
	{
		object.dump();
	}
	cout << "Object has " << object.num_vertices() << " number of vertices\n";

	GLuint vertex_buffer = object.create_vertex_buffer();
	GLuint uv_buffer = object.create_tex_coord_buffer();
	GLuint normal_buffer = object.create_normal_buffer();

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

	glfwSetScrollCallback(window, scroll_callback);

	double xpos = 0;
	double ypos = 0;
	float x_angle = 0.0;
	float y_angle = 0.0;

	char title[256];
	snprintf(title, 256, "WIP - OpenGL Object Viewer");
	glfwSetWindowTitle(window, title);
	
	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();

	auto camera_pos = glm::vec3(3, 2, 3);
	auto light_pos = glm::vec3(3, 2, 3);
	auto light_col = glm::vec3(1, 1, 1);

	// The scaler returns the diagonal length of the bounding box of the object being viewed.
	// Use this to try and create a scale value for the object to keep them reasonably scaled in the window.
	auto scaler = 1.732f / object.get_scaler();

	do
	{
		// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) width / (float)height, 0.1f, 100.0f);
  
		// Or, for an ortho camera :
		// glm::mat4 projection = glm::ortho(-2.0f,2.0f,-2.0f,2.0f,0.0f,100.0f); // In world coordinates
  
		camera_pos.z = g_zoom;

		// Camera matrix
		glm::mat4 view = glm::lookAt(
			camera_pos, // The position of the camera
			glm::vec3(0,0,0), // and looks at the origin
			glm::vec3(0,-1,0)  // Head is up (set to 0,-1,0 to look upside-down)
			);
  
		// Model matrix : an identity matrix (model will be at the origin)
		glm::mat4 model = glm::mat4(1);

		// Update model to create a rotation
		model = glm::rotate(model, x_angle, glm::vec3(0.0, 1.0, 0.0)) * glm::rotate(model, y_angle, glm::vec3(1.0, 0.0, 0.0)) * glm::scale(model, glm::vec3(scaler, scaler, scaler));
	
		// our ModelViewProjection : multiplication of our 3 matrices
		glm::mat4 mvp = projection * view * model;

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
		GLuint cam_pos_id = glGetUniformLocation(program_id, "Camera_Pos");
		glUniform3fv(cam_pos_id, 1, &camera_pos[0]);

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

		// First attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
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

		// Draw the array
		glDrawArrays(GL_TRIANGLES, 0, object.num_vertices()); // Starting from vertex 0; 3 vertices total -> 1 triangle
		glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		// Get time taken to draw the frame
		tp2 = chrono::system_clock::now();
		chrono::duration<float> elapsed_time = tp2 - tp1;
		tp1 = tp2;
		
		char title[256];
		snprintf(title, 256, "WIP - OpenGL Object Viewer - %3.f fps", 1.0 / elapsed_time.count());
		glfwSetWindowTitle(window, title);

		// Move object based on mouse position relative to center
		double old_xpos = xpos;
		double old_ypos = ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		if (fabs(old_xpos - xpos) > DBL_EPSILON ||
			fabs(old_ypos - ypos) > DBL_EPSILON)
		{
			x_angle = 0.005f * static_cast<float>(width / 2 - xpos);
			y_angle = 0.005f * static_cast<float>(height / 2 - ypos);
		}
		else
		{
			x_angle += 0.5 * elapsed_time.count();
			y_angle += 0.25 * elapsed_time.count();
		}
	}
	while (glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	return 0;
}
