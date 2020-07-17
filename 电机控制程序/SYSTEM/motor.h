#ifndef __MOTOR_H
#define __MOTOR_H	 


void  turn_left(int x);
void  turn_right(int x);
void  go_straight(void);
void  start(void);
void  stop(void);
extern float Sev_L,Sev_R;
extern int   k,t,t1,change;

#endif
