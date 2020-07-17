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
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

#ifdef USE_LINE_L1
	/*开启外设时钟*/
	RCC_APB2PeriphClockCmd(LineWalk_L1_RCC, ENABLE); 
	/*选择要控制的引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = LineWalk_L1_PIN;	
	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   
	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*调用库函数，初始化PORT*/
  	GPIO_Init(LineWalk_L1_PORT, &GPIO_InitStructure);		 
#endif

#ifdef USE_LINE_L2
	/*开启外设时钟*/
	RCC_APB2PeriphClockCmd(LineWalk_L2_RCC, ENABLE); 
	/*选择要控制的引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = LineWalk_L2_PIN;	
	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   
	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*调用库函数，初始化PORT*/
  	GPIO_Init(LineWalk_L2_PORT, &GPIO_InitStructure);	
#endif

#ifdef USE_LINE_R1
	/*开启外设时钟*/
	RCC_APB2PeriphClockCmd(LineWalk_R1_RCC, ENABLE); 
	/*选择要控制的引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = LineWalk_R1_PIN;	
	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   
	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*调用库函数，初始化PORT*/
  	GPIO_Init(LineWalk_R1_PORT, &GPIO_InitStructure);	
#endif
	
#ifdef USE_LINE_R2
	/*开启外设时钟*/
	RCC_APB2PeriphClockCmd(LineWalk_R2_RCC, ENABLE); 
	/*选择要控制的引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = LineWalk_R2_PIN;	
	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   
	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*调用库函数，初始化PORT*/
  	GPIO_Init(LineWalk_R2_PORT, &GPIO_InitStructure);	
#endif

}

void linewalking(void){
	  GetLineWalk(&LineL1, &LineL2, &LineR1, &LineR2);	//获取黑线检测状态	

	 if( (LineL1 == LOW || LineL2 == LOW) && LineR2 == LOW) //左大弯
    {
    turn_left(12);
    }
    else if ( LineL1 == LOW && (LineR1 == LOW || LineR2 == LOW)) //右大弯
	{ 
    
	   turn_right(12);

    }  
    else if( LineL1 == LOW ) //左最外侧检测
    {  
		
	turn_left(6);
	}
    else if ( LineR2 == LOW) //右最外侧检测
    {  
		turn_right(6);
	
	}
    else if (LineL2 == LOW && LineR1 == HIGH) //中间黑线上的传感器微调车左转
    {   
		 
			turn_left(6); 
	}
	  else if (LineL2 == HIGH && LineR1 == LOW) //中间黑线上的传感器微调车右转
    {   
      turn_right(6); 
					
	}
    else if(LineL2 == LOW && LineR1 == LOW) // 都是黑色, 加速前进
    {  
	  go_straight();
			}
		else
		{	go_straight();	}
//			

//if (LineL2 == LOW && LineR1 == HIGH) //中间黑线上的传感器微调车左转
//    {   
//	turn_right(); 
//	}
//else if (LineL2 == HIGH && LineR1 == LOW) //中间黑线上的传感器微调车右转
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
