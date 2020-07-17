#include "tim.h"
#define TIM_PWM_PERIOD 50000

/* ����TIM1�������PWMʱ�õ���I/O  */
/*	TIM1_CH1		PA8
	TIM1_CH1N		PB13
	TIM1_CH2		PA9
	TIM1_CH2N		PB14
	TIM1_CH3		PA10
	TIM1_CH3N		PB15
*/

static void TIM1_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

/* TIM1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

  /* GPIOA and GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

   GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*������*/	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��PC�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//IO�˿ڵĹ���ģʽ���������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//IO�˿���ߵ�������ʣ�50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//���ṹ��(GPIO_InitStructure)���趨д��GPIOC���üĴ���
}

static void TIM1_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* PWM�źŵ�ƽ����ֵ */
	u16 CCR1_Val = 50000;        
	u16 CCR2_Val = 25000;//25000/50000=50%
	u16 CCR3_Val = 25000;//25000/50000=50%
	//u16 CCR4_Val = 125;

/* -----------------------------------------------------------------------
    TIM1 Configuration: generate 2 PWM signals with 2 different duty cycles:
    TIM1CLK = 72 MHz, Prescaler = 0x0, TIM1 counter clock = 36 MHz
    TIM3 ARR Register = 50000 => TIM1 Frequency = TIM1 counter clock/(ARR + 1)
    TIM3 Frequency = 1440Hz.
    TIM1 Channel1 duty cycle = (TIM1_CCR1/ TIM1_ARR)* 100 
    TIM1 Channel2 duty cycle = (TIM1_CCR2/ TIM1_ARR)* 100 
    
  ----------------------------------------------------------------------- */

 /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = TIM_PWM_PERIOD;//50000
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;//������ͨ��
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OC1Init(TIM1, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

  /* PWM1 Mode configuration: Channel2 */
 
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;//������ͨ��
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;	  

  TIM_OC2Init(TIM1, &TIM_OCInitStructure);

  TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);	
	

  /* PWM1 Mode configuration: Channel3 */
  
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;//������ͨ��
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;	  

  TIM_OC3Init(TIM1, &TIM_OCInitStructure);

  TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);	

  

  TIM_ARRPreloadConfig(TIM1, ENABLE);

  /* TIM1 enable counter */
  TIM_Cmd(TIM1, ENABLE);
  TIM_CtrlPWMOutputs(TIM1, ENABLE);	
}

/*���úͳ�ʼ��TIM1 */
void TIM1_PWM_Init(void)
{
	TIM1_GPIO_Config();
	TIM1_Mode_Config();	
}

/*����ĳһͨ��PWM��ռ�ձ�*/
void set_pwm_dutycycle(uint8_t Channel, float dutycycle){
	uint16_t ccr;
	if(dutycycle>100.0)
		dutycycle=100.0;
	if(dutycycle<0.0)
		dutycycle=0.0;
	dutycycle=100-dutycycle;
	ccr=TIM_PWM_PERIOD*dutycycle/100;//duty cycle = (TIM1_CCR/ TIM1_ARR)* 100
	switch(Channel){
		case 1: TIM1->CCR1 = ccr;	break;
		case 2: TIM1->CCR2 = ccr;	break;
		case 3: TIM1->CCR3 = ccr;	break;
		default: break;
	}
}

//TIM2�ж�ͨ����ʼ������
void TIM2_IRQ_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;					//����ṹ�壬�����ж�ͨ�����ȼ�
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;			//ѡ���ж�ͨ������ʱ��2�жϣ�TIM2_IRQn��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;	//���ж�ͨ����ռ���ȼ���1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;		//���ж�ͨ����Ӧ���ȼ���1
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;			//ʹ�ܸ��ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);							//���ṹ��(NVIC_InitStructture)���趨д��NVIC���ƼĴ���
}

//TIM2��ʼ������
void TIM2_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;		//����ṹ�壬��ʼ����ʱ��

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//ʹ�ܶ�ʱ��2��TIM2��ʱ��
	TIM_DeInit(TIM2);										//������TIM2�Ĵ�������ΪĬ��ֵ
	
	TIM_TimeBaseInitStructure.TIM_Prescaler=719;				//��ʱ��ʱ��Ԥ��Ƶֵ ȡֵ��Χ��0~65535
	TIM_TimeBaseInitStructure.TIM_Period=999;				//��ʱ��������ֵ ȡֵ��Χ��0~65535
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);		//���ṹ��(TIM_TimeBaseInitStructture)���趨д��TIM2���üĴ�����
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);					// �������жϱ�־
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE);	//�ȹرյȴ�ʹ��
}



