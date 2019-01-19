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
#include "world.hpp"
#include "blockinstance.hpp"

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

	// Set up world environment
	Light light = Light(glm::vec3(3, 2, 3), glm::vec3(1, 1, 1));

	Camera camera = Camera(glm::vec3(0, 0, 5),
						   glm::vec3(glm::radians(0.0f), glm::radians(0.0f), 0.0f),
						   glm::radians(45.0f),
						   static_cast<float>(width) / static_cast<float>(height));

	World world = World(camera, light);

	// Set up objects to render
	Texture block_texture = Texture("res/blockinstance.png", 1, false);
	vector<BlockInstance> objects;
	for (int bigz=0; bigz<128; bigz+=BLOCK_DEPTH)
	{
		for (int bigx=0; bigx<128; bigx+=BLOCK_WIDTH)
		{
			BlockInstance block = BlockInstance(block_texture, program_id, world);
			block.position().x = static_cast<float>(bigx) - 64.0f;
			block.position().y = -10.f;
			block.position().z = static_cast<float>(bigz) - 64.0f;

			for (int z=0; z<BLOCK_DEPTH; z++)
			{
				for (int x=0; x<BLOCK_WIDTH; x++)
				{
					int idx = ((bigz + z) * 128) + (bigx + x);
					for (int y = 0; y < 6; y++)
					{
						if ((map_data[idx] & (0x1 << y)) != 0)
						{
							block.setBit(x, y + 1, z);
						}
					}

					// Add floor
					block.setBit(x, 0, z);
				}
			}

			block.generateBlock();
			objects.push_back(block);
		}
	}

	cout << "Number of object blocks in scene: " << objects.size() << endl;

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

		glClearColor(0.3f, 0.6f, 0.9f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (auto &object : objects)
		{
			object.setUniforms();
			object.render();
		}

		// block.setUniforms();
		// block.render();

		win.swapBuffers();
	}
	while (!win.isKeyPressed(GLFW_KEY_ESCAPE));

	return 0;
}
