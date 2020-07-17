#ifndef __ENCODER_H
#define __ENCODER_H	 
#include "tim.h"
#include "stm32f10x.h"
#include "sys.h"
extern u8  counter;
extern u8  pwm;
extern u8 speed;

void Lencoder_init(void);
uint32_t Lread_cnt(void);
void Rencoder_init(void);
uint32_t Rread_cnt(void);
#endif
