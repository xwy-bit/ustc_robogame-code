//��TIM3���Ϊһ��������������һ����ʱ������û����ʱ���źš�
//����TIM3��ʱ���źţ�����˵�Ǽ����źţ����ɵ�������������������棬Ҳ����˵��������źų�ΪTIM3���źţ����ÿ����һ�����屻TIM3��⵽���������CNT��һ�������ʱ���ź�ʱÿ��һ��ʱ��μ���ֵ��һ��
//�����Ļ������벶����Զ���װ��ֵperiod��Ӱ��������ֵ����������֮������������65535�Ļ�������յ�65535�������ź�֮�����ֵ�������
//�����Ļ������벶���Ԥ��Ƶϵ��prescaler�������ǣ����Ҳ���Ƶʱ����һ����������ź��Ҽ���ֵ�ͼ�һ�����Ҷ���Ƶʱ��ֻ�н��յ����������ź��Ҳ���Ϊ��һ����Ч���壬����ֵ�ż�һ������˵���Ǽ���ֵ������Զ���
//�������ǾͰ����벶���ʼ������ˣ��������Ǳ�����ģʽ�ĳ�ʼ��
//��ΪTIM_EncoderMode_TI12ģʽ������������TI1��TI2�����ش����������ٸ������õļ�����TIM_ICPolarity_Rising��Ҳ������TI1��TI2�������ؼ������ۼӣ����ۼ���-->��ô��ʱ��Ҫ���Զ�
//�������������ĳ�ʼ��������ˣ�����������ֻҪͨ�������ó����ļ���ֵ���Ϳ���֪��������������������ٸ��ݵ���Ĳ�����ÿת�������ٸ����壩�Ϳ��Եõ����ת�˼�Ȧ
#include "encoder.h"
#include "stm32f10x.h"

#define ENCODER_TIMER                TIM3  // Encoder unit connected to TIM3

#define COUNTER_RESET   (u16)30000
#define ICx_FILTER      (u8) 6 // 6<-> 670nsec

#define TIMx_PRE_EMPTION_PRIORITY 1
#define TIMx_SUB_PRIORITY 0


#define SPEED_SAMPLING_FREQ (u16)(2000/(SPEED_SAMPLING_TIME+1))


//��ʼ�����������õ��Ƕ�ʱ��3
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
	//��������rising����ָ�����أ�����ָ�����з���ֻ�ǽ����������صĺ궨��
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

// ��ȡ��ʱ������ֵ
uint32_t Lread_cnt(void)
{
	uint32_t encoder_cnt;
	encoder_cnt = TIM3->CNT;		//��ȡ������CNT��ֵ��CNTϵuint32_t�͵ı���
	TIM_SetCounter(TIM3, 30000);		//ÿһ�ζ�ȡ�����ֵ�󽫼���ֵ���㣬���¿�ʼ�ۼ����壬������һ�μ���
	return encoder_cnt;				//���ص�ֵ���Ǳ��ζ����ļ���ֵ
}


void TIM3_IRQHandler(void)
{  
	/* Clear the interrupt pending flag */
	TIM_ClearFlag(ENCODER_TIMER, TIM_FLAG_Update);
	TIM3->CNT = 30000;
}



//��ʼ�����������õ��Ƕ�ʱ��3
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
	//��������rising����ָ�����أ�����ָ�����з���ֻ�ǽ����������صĺ궨��
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

// ��ȡ��ʱ������ֵ
uint32_t Rread_cnt(void)
{
	uint32_t encoder_cnt;
	encoder_cnt = TIM4->CNT;		//��ȡ������CNT��ֵ��CNTϵuint32_t�͵ı���
	TIM_SetCounter(TIM4, 30000);		//ÿһ�ζ�ȡ�����ֵ�󽫼���ֵ���㣬���¿�ʼ�ۼ����壬������һ�μ���
	return encoder_cnt;				//���ص�ֵ���Ǳ��ζ����ļ���ֵ
}


void TIM4_IRQHandler(void)
{  
	/* Clear the interrupt pending flag */
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	TIM4->CNT = 30000;
}







