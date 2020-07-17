#include "linewalk.h"
#include "stm32f10x.h"
#include "delay.h"
#include "motor.h"
int LineL1 = 1, LineL2 = 1, LineR1 = 1, LineR2 = 1;

void GetLineWalk(int *p_iL1, int *p_iL2, int *p_iR1, int *p_iR2)
{
	*p_iL1 = GPIO_ReadInputDataBit(LineWalk_L1_PORT, LineWalk_L1_PIN);
  *p_iL2 = GPIO_ReadInputDataBit(LineWalk_L2_PORT, LineWalk_L2_PIN);
	*p_iR1 = GPIO_ReadInputDataBit(LineWalk_R1_PORT, LineWalk_R1_PIN);
	*p_iR2 = GPIO_ReadInputDataBit(LineWalk_R2_PORT, LineWalk_R2_PIN);		
}
void LineWalk_GPIO_Init(void)
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

#ifdef USE_LINE_L1
	/*��������ʱ��*/
	RCC_APB2PeriphClockCmd(LineWalk_L1_RCC, ENABLE); 
	/*ѡ��Ҫ���Ƶ�����*/															   
  	GPIO_InitStructure.GPIO_Pin = LineWalk_L1_PIN;	
	/*��������ģʽΪͨ���������*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   
	/*������������Ϊ50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*���ÿ⺯������ʼ��PORT*/
  	GPIO_Init(LineWalk_L1_PORT, &GPIO_InitStructure);		 
#endif

#ifdef USE_LINE_L2
	/*��������ʱ��*/
	RCC_APB2PeriphClockCmd(LineWalk_L2_RCC, ENABLE); 
	/*ѡ��Ҫ���Ƶ�����*/															   
  	GPIO_InitStructure.GPIO_Pin = LineWalk_L2_PIN;	
	/*��������ģʽΪͨ���������*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   
	/*������������Ϊ50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*���ÿ⺯������ʼ��PORT*/
  	GPIO_Init(LineWalk_L2_PORT, &GPIO_InitStructure);	
#endif

#ifdef USE_LINE_R1
	/*��������ʱ��*/
	RCC_APB2PeriphClockCmd(LineWalk_R1_RCC, ENABLE); 
	/*ѡ��Ҫ���Ƶ�����*/															   
  	GPIO_InitStructure.GPIO_Pin = LineWalk_R1_PIN;	
	/*��������ģʽΪͨ���������*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   
	/*������������Ϊ50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*���ÿ⺯������ʼ��PORT*/
  	GPIO_Init(LineWalk_R1_PORT, &GPIO_InitStructure);	
#endif
	
#ifdef USE_LINE_R2
	/*��������ʱ��*/
	RCC_APB2PeriphClockCmd(LineWalk_R2_RCC, ENABLE); 
	/*ѡ��Ҫ���Ƶ�����*/															   
  	GPIO_InitStructure.GPIO_Pin = LineWalk_R2_PIN;	
	/*��������ģʽΪͨ���������*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   
	/*������������Ϊ50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*���ÿ⺯������ʼ��PORT*/
  	GPIO_Init(LineWalk_R2_PORT, &GPIO_InitStructure);	
#endif

}

void linewalking(void){
	  GetLineWalk(&LineL1, &LineL2, &LineR1, &LineR2);	//��ȡ���߼��״̬	

	 if( (LineL1 == LOW || LineL2 == LOW) && LineR2 == LOW) //�����
    {
    turn_left(12);
    }
    else if ( LineL1 == LOW && (LineR1 == LOW || LineR2 == LOW)) //�Ҵ���
	{ 
    
	   turn_right(12);

    }  
    else if( LineL1 == LOW ) //���������
    {  
		
	turn_left(6);
	}
    else if ( LineR2 == LOW) //���������
    {  
		turn_right(6);
	
	}
    else if (LineL2 == LOW && LineR1 == HIGH) //�м�����ϵĴ�����΢������ת
    {   
		 
			turn_left(6); 
	}
	  else if (LineL2 == HIGH && LineR1 == LOW) //�м�����ϵĴ�����΢������ת
    {   
      turn_right(6); 
					
	}
    else if(LineL2 == LOW && LineR1 == LOW) // ���Ǻ�ɫ, ����ǰ��
    {  
	  go_straight();
			}
		else
		{	go_straight();	}
//			

//if (LineL2 == LOW && LineR1 == HIGH) //�м�����ϵĴ�����΢������ת
//    {   
//	turn_right(); 
//	}
//else if (LineL2 == HIGH && LineR1 == LOW) //�м�����ϵĴ�����΢������ת
//    {   
//			turn_left();  
//	}
//	
// else 
//    {  
//	  go_straight();			

//	}	








delay_ms(100);









}	
