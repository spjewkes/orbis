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
		{"width", required_argument, 0, 'w'},
		{"height", required_argument, 0, 'h'},
		{0, 0, 0, 0}
	};

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
		}
	}
}

void Options::display_help(const char *app_name)
{
	cout << "Usage: " << app_name << " <options>\n";
	cout << "  --verbose - enable verbose output.\n";
	cout << "  --width <width> - width of display in pixels.\n";
	cout << "  --height <height> - height of display in pixels.\n";
}
