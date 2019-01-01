#ifndef __WORLD_HPP__
#define __WORLD_HPP__

#include "camera.hpp"
#include "light.hpp"

class World
{
public:
	World(Camera &camera, Light &light);
	virtual ~World();

	Camera &camera() const { return view; }
	Light &light() const { return lights; }

private:
	Camera &view;
	Light &lights;
};

#endif
