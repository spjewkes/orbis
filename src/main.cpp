// Include standard headers
#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <random>

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
#include "texture.hpp"
#include "light.hpp"
#include "instance.hpp"

#include "ant_attack.hpp"

using namespace std;

void handleMovement(Window &win, glm::vec3 &move, glm::vec3 &rotate, float elapsed_time)
{
	float rotate_step = 20.0f;
	float move_step = 2.0f;

	// Deal with movement
	if (win.isKeyPressed(GLFW_KEY_LEFT_SHIFT) || win.isKeyPressed(GLFW_KEY_RIGHT_SHIFT))
	{
		rotate_step *= 4.0f;
		move_step *= 4.0f;
	}

	if (win.isKeyPressed(GLFW_KEY_UP))
	{
		rotate.x -= glm::radians(rotate_step) * elapsed_time;
	}
	if (win.isKeyPressed(GLFW_KEY_DOWN))
	{
		rotate.x += glm::radians(rotate_step) * elapsed_time;
	}
	if (win.isKeyPressed(GLFW_KEY_LEFT))
	{
		rotate.y -= glm::radians(rotate_step) * elapsed_time;
	}
	if (win.isKeyPressed(GLFW_KEY_RIGHT))
	{
		rotate.y += glm::radians(rotate_step) * elapsed_time;
	}
	if (win.isKeyPressed(GLFW_KEY_Q))
	{
		rotate.z += glm::radians(rotate_step) * elapsed_time;
	}
	if (win.isKeyPressed(GLFW_KEY_E))
	{
		rotate.z -= glm::radians(rotate_step) * elapsed_time;
	}

	if (win.isKeyPressed(GLFW_KEY_A))
	{
		move.x -= move_step * elapsed_time;
	}
	if (win.isKeyPressed(GLFW_KEY_D))
	{
		move.x += move_step * elapsed_time;
	}
	if (win.isKeyPressed(GLFW_KEY_PAGE_UP))
	{
		move.y += move_step * elapsed_time;
	}
	if (win.isKeyPressed(GLFW_KEY_PAGE_DOWN))
	{
		move.y -= move_step * elapsed_time;
	}
	if (win.isKeyPressed(GLFW_KEY_W))
	{
		move.z -= move_step * elapsed_time;
	}
	if (win.isKeyPressed(GLFW_KEY_S))
	{
		move.z += move_step * elapsed_time;
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
	Texture texture = Texture(options.imagepath(), 0);

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

	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();

	Light light = Light(glm::vec3(3, 2, 3), glm::vec3(1, 1, 1));

	Camera camera = Camera(glm::vec3(0, 0, 5),
						   glm::vec3(glm::radians(0.0f), glm::radians(0.0f), 0.0f),
						   glm::radians(45.0f),
						   static_cast<float>(width) / static_cast<float>(height));

	// Set up objects to render
	vector<Instance> objects;
	for (int z = 0; z < 128; z++)
	{
		for (int x = 0; x < 128; x++)
		{
			int idx = (z * 128) + x;

			for (int y = 0; y < 6; y++)
			{
				if ((map_data[idx] & (0x1 << y)) != 0)
				{
					Instance instance = Instance(object, texture, program_id, light, camera);
					instance.position().x = -2.0f * (x - 63);
					instance.position().y = -2.0f + (-2.0f * (5 - y));
					instance.position().z = -2.0f * (z - 63);
					instance.rotation().z = glm::radians(90.0f);

					objects.push_back(instance);
				}
			}

			// Add floor
			Instance instance = Instance(object, texture, program_id, light, camera);
			instance.position().x = -2.0f * (x - 63);
			instance.position().y = -2.0f + (-2.0f * 6);
			instance.position().z = -2.0f * (z - 63);
			instance.rotation().z = glm::radians(90.0f);

			objects.push_back(instance);
		}
	}

	cout << "Number of objects in scene: " << objects.size() << endl;

	// Render loop
	do
	{
		// Get time taken to draw the frame
		tp2 = chrono::system_clock::now();
		chrono::duration<float> elapsed_time = tp2 - tp1;
		tp1 = tp2;

		char title[256];
		snprintf(title, 256, "Orbis - %3.1f fps", 1.0f / elapsed_time.count());
		win.setTitle(title);

		// Handle movement of camera
		glm::vec3 rotate(0, 0, 0);
		glm::vec3 move(0, 0, 0);

		handleMovement(win, move, rotate, elapsed_time.count());
		camera.move(move, rotate);

		glClearColor(0.25f, 0.25f, 0.25f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (auto &object : objects)
		{
			object.setUniforms();
			object.render();
		}

		win.swapBuffers();
	}
	while (!win.isKeyPressed(GLFW_KEY_ESCAPE));

	return 0;
}
