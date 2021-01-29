#pragma once
#include <string>
class toSave
{
public:
	toSave(char** Mem, int start_p, int length_p, int width_p, int height_p, float fps_p, std::string final_namestr_p);
	char** ppcImageMem;
	int length;
	int start;
	int width;
	int height;
	float fps;
	std::string final_namestr;
};

