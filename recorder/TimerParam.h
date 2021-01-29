#pragma once
#include "toSave.h"
class TimerParam
{
public:
	TimerParam(float duration_p, int count_p, toSave* writeParamFirst_p, toSave* writeParamSecond_p);
	float duration;
	int count;
	toSave* writeParamFirst;
	toSave* writeParamSecond;
};

