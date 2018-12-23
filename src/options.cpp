#include <cstdlib>
#include <iostream>
#include <getopt.h>
#include "options.hpp"

using namespace std;

void Options::initialize(int argc, char *argv[])
{
	static struct option long_options[] =
	{
		{"verbose", no_argument, 0, 'v'},
		{"file", required_argument, 0, 'f'},
		{"width", required_argument, 0, 'w'},
		{"height", required_argument, 0, 'h'},
		{"image", required_argument, 0, 'i'},
		{0, 0, 0, 0}
	};

	strcpy(m_filepath, "");
	strcpy(m_imagepath, "res/texture.png");

	while (true)
	{
		int option_index = 0;
		int c = getopt_long(argc, argv, "vf:w:h:", long_options, &option_index);

		if (c == -1)
		{
			break;
		}

		switch (c)
		{
		case 0:
			display_help(argv[0]);
			break;
		case 'v':
			m_verbose = true;
			break;
		case 'w':
			m_width = atoi(optarg);
			break;
		case 'h':
			m_height = atoi(optarg);
			break;
		case 'f':
			strcpy(m_filepath, optarg);
			break;
		case 'i':
			strcpy(m_imagepath, optarg);
			break;
		}
	}

	if (strlen(m_filepath) == 0)
	{
		cerr << "ERROR: No Wavefront Obj file specified. Aborting.\n";
		display_help(argv[0]);
		abort();
	}
}

void Options::display_help(const char *app_name)
{
	cout << "Usage: " << app_name << " <options>\n";
	cout << "  --verbose - enable verbose output.\n";
	cout << "  --file <obj file> - Wavefront Obj file to load.\n";
	cout << "  --width <width> - width of display in pixels.\n";
	cout << "  --height <height> - height of display in pixels.\n";
	cout << "  --image <png file> - PNG of texture to use.\n";
}
