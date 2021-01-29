#include "stdafx.h"
#include "TimerParam.h"
#include "toSave.h"

TimerParam::TimerParam(float duration_p, int count_p, toSave* writeParamFirst_p, toSave* writeParamSecond_p)
{
	duration = duration_p;
	count = count_p;
	writeParamFirst = writeParamFirst_p;
	writeParamSecond = writeParamSecond_p;
}
