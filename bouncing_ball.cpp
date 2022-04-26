#include <iostream>
#include "minigl2d.hpp"

using namespace minigl;
using namespace glm;


std::vector<shape> func(std::vector<shape> world)
{
	return world;
}

int main()
{
	window2d my_window(400_px, 400_px, color(colors::forest_green), "bouncing ball demo");
	std::vector<shape> shapes = { triangle(5) };
	/*
	render2d::animate(my_window, 60, shapes);
	*/
}

