#include "stm32f10x.h"
#include "TEA5767.h"
#include "delay.h"
#include "iic.h"
/**********************************************************
** 函数名: main
** 功能描述: 设置电台频率，通过按键手动进行电台搜索
** 输入参数: 无
** 输出参数: 无
***********************************************************/
int main(void)
{ 
	IIC_Init();	//IIC接口初始化
	Set_Frequency(88700); //设置电台频率为101.8MHz
	while(1);
}
