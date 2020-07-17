#include "motor.h"
float Sev_L=0,Sev_R=0;
int   k=0,t=0,t1=0,change=1;
void start(void)
{
		if(t==0)
		{ 
				Sev_L=10*k;
				if(Sev_L>200)
				{t=1;}	
				k++;		  
		}
}
void turn_left(int x)
{   Sev_R=Sev_R+x;
		if(Sev_R>100)
	{Sev_L=95;Sev_R=95;}

}
void turn_right(int x)
{
   Sev_L=Sev_L+x;
	if(Sev_L>100)
	{Sev_L=95;Sev_R=95;}
}
void go_straight(void)	
{
	Sev_L=60;
	Sev_R=60;
}
void stop(void)//目前不要用
{
		if(t==1)
		{ 
				Sev_L=10*k;
				if(Sev_L<10)
				{t=0;}	
				k--;		  
		}

}
