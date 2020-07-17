//把TIM3理解为一个计数器而不是一个定时器，则没有了时序信号。
//这里TIM3的时钟信号（或者说是计数信号）将由电机编码器输出的脉冲代替，也就是说电机脉冲信号成为TIM3的信号，电机每产生一个脉冲被TIM3检测到，则计数器CNT加一（类比于时序信号时每隔一个时间段计数值加一）
//这样的话，输入捕获的自动重装载值period则影响着脉冲值计数到多少之后就溢出，比如65535的话，则接收到65535个脉冲信号之后计数值置零溢出
//这样的话，输入捕获的预分频系数prescaler的作用是，当我不分频时，来一个电机脉冲信号我计数值就加一，当我二分频时，只有接收到两个脉冲信号我才认为是一个有效脉冲，计数值才加一，简单来说就是计数值总体除以二了
//这样我们就把输入捕获初始化完成了，接下来是编码器模式的初始化
//设为TIM_EncoderMode_TI12模式，即计数器在TI1和TI2上升沿处均计数，再根据设置的极性是TIM_ICPolarity_Rising，也就是在TI1和TI2的上升沿计数器累加（或累减）-->那么到时候要除以二
//这样，编码器的初始化就完成了，接下来我们只要通过函数得出它的计数值，就可以知道电机产生的脉冲数，再根据电机的参数（每转产生多少个脉冲）就可以得到电机转了几圈
#include "encoder.h"
#include "stm32f10x.h"

#define ENCODER_TIMER                TIM3  // Encoder unit connected to TIM3

#define COUNTER_RESET   (u16)30000
#define ICx_FILTER      (u8) 6 // 6<-> 670nsec

#define TIMx_PRE_EMPTION_PRIORITY 1
#define TIMx_SUB_PRIORITY 0


#define SPEED_SAMPLING_FREQ (u16)(2000/(SPEED_SAMPLING_TIME+1))


//初始化编码器，用的是定时器3
void Lencoder_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;
  
/* Encoder unit connected to TIM3, 4X mode */    
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* TIM3 clock source enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  /* Enable GPIOA, clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
  GPIO_StructInit(&GPIO_InitStructure);
  /* Configure PA.06,07 as encoder input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Enable the TIM3 Update Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIMx_PRE_EMPTION_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIMx_SUB_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Timer configuration in Encoder mode */
  TIM_DeInit(ENCODER_TIMER);
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  
  TIM_TimeBaseStructure.TIM_Prescaler = 0x0;  // No prescaling 
  TIM_TimeBaseStructure.TIM_Period = 65535;  
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
  TIM_TimeBaseInit(ENCODER_TIMER, &TIM_TimeBaseStructure);
 
  TIM_EncoderInterfaceConfig(ENCODER_TIMER, TIM_EncoderMode_TI12, 
                             TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	//后面两个rising不是指上升沿，而是指不进行反向，只是借用了上升沿的宏定义
  TIM_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TIM_ICFilter = ICx_FILTER;
  TIM_ICInit(ENCODER_TIMER, &TIM_ICInitStructure);
  
 // Clear all pending interrupts
  TIM_ClearFlag(ENCODER_TIMER, TIM_FLAG_Update);
  TIM_ITConfig(ENCODER_TIMER, TIM_IT_Update, ENABLE);
  //Reset counter
  ENCODER_TIMER->CNT = COUNTER_RESET;
  
  
  TIM_Cmd(ENCODER_TIMER, ENABLE);  
}

// 读取定时器计数值
uint32_t Lread_cnt(void)
{
	uint32_t encoder_cnt;
	encoder_cnt = TIM3->CNT;		//读取计数器CNT的值，CNT系uint32_t型的变量
	TIM_SetCounter(TIM3, 30000);		//每一次读取完计数值后将计数值清零，重新开始累加脉冲，方便下一次计数
	return encoder_cnt;				//返回的值就是本次读到的计数值
}


void TIM3_IRQHandler(void)
{  
	/* Clear the interrupt pending flag */
	TIM_ClearFlag(ENCODER_TIMER, TIM_FLAG_Update);
	TIM3->CNT = 30000;
}



//初始化编码器，用的是定时器3
void Rencoder_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;
  
/* Encoder unit connected to TIM4, 4X mode */    
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* TIM3 clock source enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  /* Enable GPIOB, clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  
  GPIO_StructInit(&GPIO_InitStructure);
  /* Configure PB.06,07 as encoder input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /* Enable the TIM4 Update Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIMx_PRE_EMPTION_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIMx_SUB_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Timer configuration in Encoder mode */
  TIM_DeInit(TIM4);
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  
  TIM_TimeBaseStructure.TIM_Prescaler = 0x0;  // No prescaling 
  TIM_TimeBaseStructure.TIM_Period = 65535;  
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
 
  TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, 
                             TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	//后面两个rising不是指上升沿，而是指不进行反向，只是借用了上升沿的宏定义
  TIM_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TIM_ICFilter = ICx_FILTER;
  TIM_ICInit(TIM4, &TIM_ICInitStructure);
  
 // Clear all pending interrupts
  TIM_ClearFlag(TIM4, TIM_FLAG_Update);
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
  //Reset counter
  TIM4->CNT = COUNTER_RESET;
  
  
  TIM_Cmd(TIM4, ENABLE);  
}

// 读取定时器计数值
uint32_t Rread_cnt(void)
{
	uint32_t encoder_cnt;
	encoder_cnt = TIM4->CNT;		//读取计数器CNT的值，CNT系uint32_t型的变量
	TIM_SetCounter(TIM4, 30000);		//每一次读取完计数值后将计数值清零，重新开始累加脉冲，方便下一次计数
	return encoder_cnt;				//返回的值就是本次读到的计数值
}


void TIM4_IRQHandler(void)
{  
	/* Clear the interrupt pending flag */
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	TIM4->CNT = 30000;
}







