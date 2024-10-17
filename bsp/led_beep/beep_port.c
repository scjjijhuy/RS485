/*
	蜂鸣器底层代码源文件
	作者：lee
	时间：2019/5/17
*/

#include "main.h"

/*
	蜂鸣器控制
*/
void beep(int control)
{
	if(1 == control)
	{
		global_flag.beep_flag = 1;
		global_timer.beep_timer=0;
//		BEEP_ON();
	}
	else
	{
//		BEEP_OFF();
		global_timer.beep_timer=0;
		global_flag.beep_flag = 0;
	}
}


