/*
管脚对应关系
    左电机：编码器PA6，PA7    PWM 输出PB14
		右电机：编码器PB6，PB7    PWM 输出PB15
		红外对管：---------------------------------（俯视图）
		          |    L2     L1    R1      R2    |
		          |    |      |     |       |     |
              ---------------------------------
		          |    |      |     |       |     | 
              GND  PC13   PC14  PC15    PB1   VCC
*/
#include "sys.h"
#include "tim.h"
#include "math.h"
#include "motor.h"
#include "stdio.h"
#include "usart.h"
#include "delay.h"
#include "stdlib.h"
#include "string.h"
#include "encoder.h"
#include "linewalk.h"
#include "stm32f10x.h"

#define Pwm_channel_L		2
#define Pwm_channel_R		3

uint8_t pwmtick;
uint8_t dutycycle=50;


int cou=0;
//下面这里说明参数作用
	float Kp=20,Ki=2,Kd=0.5;//pid系数

	int   data_usart_receive,data_usart_receive_last;//从串口处接收的数据，用于PID调试
//左电机
	float Acv_L=0.0;//测试量，在PID.c中测试其中断函数是否有效
	float err_now_L=0.0;//现在转速与设定转速的差值
	float err_last_L=0.0,err_last2_L=0.0;//前两次转速与设定转速的差值
	float integral_L=0.0;//积分项
	float acclerate_L=0.0;//pid算出来的“加速度”
  float pwm1_L=0;//pwm占空比的两倍
  float round1_L=0;//每秒转速，*60后得到每分钟
  int   cnt_temp_L;
//右电机
	float Acv_R=0.0;//测试量，在PID.c中测试其中断函数是否有效
	float err_now_R=0.0;//现在转速与设定转速的差值
	float err_last_R=0.0,err_last2_R=0.0;//前两次转速与设定转速的差值
	float integral_R=0.0;//积分项
	float acclerate_R=0.0;//pid算出来的“加速度”
  float pwm1_R=0;//pwm占空比的两倍
  float round1_R=0;//每秒转速，*60后得到每分钟
  int   cnt_temp_R;
//PID参数调整
void pid_adjust(void)
{
  data_usart_receive=USART_ReceiveData(USART1);
	if(data_usart_receive_last!=data_usart_receive)
	{
		switch(data_usart_receive)
		{
			case 48:Kp=Kp+3;break;
			case 49:Ki=Ki+0.5;break;
			case 50:Kd=Kd+0.1;break;
			case 51:Kp=Kp-1;break;
			case 52:Ki=Ki-0.1;break;
			case 53:Kd=Kd-0.1;break;
			case 54:Sev_L=Sev_L+100;break;
			case 55:Sev_L=Sev_L-40;break;
			case 56:Sev_L=Sev_L+50;break;
		}
	}
	data_usart_receive_last=data_usart_receive;
}
void pid_add_R(void)//增量式PID
{
		err_now_R=Sev_R-round1_L*6000;
	  acclerate_R=Kp*(err_now_R-err_last_R)+Ki*err_now_R+Kd*(err_now_R-2*err_last_R+err_last2_R);
	  err_last2_R=err_last_R;
	  err_last_R=err_now_R;	
   	pwm1_R=acclerate_R*0.012+pwm1_R;
		if(pwm1_R>100)
			pwm1_R=100;
		if(pwm1_R<10)
			pwm1_R=10;	 
		set_pwm_dutycycle(Pwm_channel_R,pwm1_R);
}
void pid_add_L(void)//增量式PID
{
		err_now_L=Sev_L-round1_L*6000;
	  acclerate_L=Kp*(err_now_L-err_last_L)+Ki*err_now_L+Kd*(err_now_L-2*err_last_L+err_last2_L);
	  err_last2_L=err_last_L;
	  err_last_L=err_now_L;	
	
   	pwm1_L=acclerate_L*0.012+pwm1_L;

		if(pwm1_L>100)
			pwm1_L=100;
		if(pwm1_L<10)
			pwm1_L=10;	 
		set_pwm_dutycycle(Pwm_channel_L,pwm1_L);
}
void bsp_init(void)
{

	delay_init();	    		//延时函数初始化	  
	NVIC_Configuration(); 		//设置NVIC中断分组2:2位抢占优先级，2位响应优先级 
  //uart_init(9600);	
	//uart_init(115200);			//串口初始化，波特率9600
	TIM1_PWM_Init();			//TIM1初始化PWM输出
	TIM2_IRQ_init();
	TIM2_init();
	Lencoder_init();
  Rencoder_init();
	LineWalk_GPIO_Init();
}
/**************************************************************************************/
int main(void)
{ 
//Sev_L=200;

bsp_init();

set_pwm_dutycycle(Pwm_channel_L,20);	
set_pwm_dutycycle(Pwm_channel_R,20);
	
go_straight();
  while(1){
		pid_add_L();
		pid_add_R();
		linewalking();
		if(Sev_L>100|Sev_R>100)
			{Sev_L=95;Sev_R=95;}
		delay_ms(1);

  }
}


void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{


		cnt_temp_L = Lread_cnt() - 30000	;	 //得到脉冲计数值		
		if(cnt_temp_L<0)//取绝对值
		{cnt_temp_L=0-cnt_temp_L;}
		round1_L = cnt_temp_L/4.0f/341.2f;			//电机每转平均产生314.2个脉冲，则通过该公式可求出电机转了几圈		
    Acv_L=round1_L*6000;
  
		cnt_temp_R = Rread_cnt() - 30000	;	 //得到脉冲计数值		
		if(cnt_temp_R<0)//取绝对值
		{cnt_temp_R=0-cnt_temp_R;}
		round1_R = cnt_temp_R/4.0f/341.2f;			//电机每转平均产生314.2个脉冲，则通过该公式可求出电机转了几圈		
		Acv_R=round1_R*6000;
		
		
//		if(k==30)
//		{	printf("d: %f,%.2f,%.2f,%.2f,%.2f,%.2f\r\n", Acv_L,Kp,Ki,acclerate_L,Sev_L,pwm1_L);		 //向串口打印电机每分钟转了几圈
//		    k=0;}
//		k++;
pid_adjust();
//		if(t==0&change==1)
//		{ 
//				Sev_L=10*k;
//				if(Sev_L>300)
//				{t=1;change=0;}//k=300;}	
//				k++;		  
//		}
//  
//		if(change==0)
//		{

//			if(t1==250)
//			{change=1;t1=0;}
//			t1++;
//		}
//		if(t==1&change==1)
//		{
//		
//				Sev_L=10*k;
//				if(Sev_L<60)
//				{t=0;change=0;}//k=300;}	
//				k--;	
//		}

	  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
	

