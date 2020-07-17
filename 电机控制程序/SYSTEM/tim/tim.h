#ifndef __TIM_H
#define __TIM_H 

#include "stm32f10x.h"
void TIM1_PWM_Init(void);
void set_pwm_dutycycle(uint8_t Channel, float dutycycle);

void TIM2_IRQ_init(void);
void TIM2_init(void);
void TIM4_IRQ_init(void);
void TIM4_init(void);

#endif
