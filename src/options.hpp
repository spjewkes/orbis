#ifndef __OPTIONS_HPP__
#define __OPTIONS_HPP__

class Options
{
public:
	Options(int argc, char *argv[]) { initialize(argc, argv); }
	~Options() {}

	bool verbose() const { return m_verbose; }
	int width() const { return m_width; }
	int height() const { return m_height; }
	char *filepath() const { return const_cast<char*>(&m_filepath[0]); }
	char *imagepath() const { return const_cast<char*>(&m_imagepath[0]); }

private:
	void initialize(int argc, char *argv[]);
	void display_help(const char *app_name);

	bool m_verbose = false;
	int m_width = 1024;
	int m_height = 768;
	char m_filepath[255];
	char m_imagepath[255];
};

#endif // __OPTIONS_HPP__
