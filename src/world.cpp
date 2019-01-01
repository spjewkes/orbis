#include "world.hpp"

World::World(Camera &camera, Light &light) : view(camera), lights(light)
{
}

World::~World()
{
}

