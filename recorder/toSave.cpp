#include "stdafx.h"
#include "toSave.h"
#include <string>

toSave::toSave(char** Mem, int start_p, int length_p, int width_p, int height_p, float fps_p, std::string final_namestr_p)
{
	ppcImageMem = Mem;
	start = start_p;
	length = length_p;
	final_namestr = final_namestr_p;
}
