#ifndef __TEA5767__H__INCLUDED__
#define __TEA5767__H__INCLUDED__
#include "stm32f10x.h"
#include "iic.h"

//TEA5767��������
void TEA5767_Write(void);
void Get_PLL(void);
void Get_Frequency(void);
void TEA5767_Read(void);
void Search(char mode);
void Auto_Search(char mode);
void Set_Frequency(u32 fre);
#endif
