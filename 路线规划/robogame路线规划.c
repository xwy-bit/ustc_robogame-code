#include<stdio.h>
#include<math.h>
//路线简化图 
//5->ICU = 2700  ICU->5 =700
//5->4=...= 1->0=600
//5->O5=..=0->O0=700
//fc=方舱医院  O是出发点 
//fc->3=fc->2=fc->1=700
//在左边的医院归为B类，右边的病人点归为A类 
/*
ICU-----5-------O5
        |
        4-------O4
        |
fc------3-------O3
        |
fc------2-------O2
        |
fc------1-------O1
        |
        0-------O0
        |
        O
        */
//测量两点之间的距离 
int distance(char c1,char n1,char c2,char n2)
{
	if( c1 == 'A' && c2=='A') return 700*2+abs(n1-n2)*600;
	if( c1 == 'A' && c2 =='B')
	{
		if (n2 == 5) return 700+600*abs(n1-n2)+2700;//包括戴口罩
		else return 700*2+600*abs(n1-n2); 
	}
	if(c1 =='B' && c2 == 'A')
	{
		if(n1 == 5) return 700+700+abs(n1-n2)*600;
		else return 700*2+abs(n1-n2)*600;
	}
	if (c1 =='B' && c2 =='B')
	{
		if(n1 == 5) return 700*2+abs(n1-n2)*600;
		if(n2 == 5) return 700+2700+abs(n1-n2)*600;
		return 700*2+abs(n1-n2)*600;
	}
	if(c1 == 'O') return 600+700+600*n2;
	return -1000000;
}
//指数
int power(int a,int b)
{
	if(b==0) return 1;
	return a*power(a,b-1); 
 } 

//交换
void swap(int *a,int *b)
{
	
	int c;
	c=*a;
	*a=*b;
	*b=c;
	
 } 
 //阶乘 
 int multi(unsigned char n )
 {
 	if(n==1) return 1;
 	else return n*multi(n-1);
 }

//全排列 permutation(p,n,m)表示起始指针为p的n个数全排列的第m个排列顺序，比如permutation(p,3,0)返回 p，操作后 p对应的排列顺序为 0 1 2 
int* permutation(int* p,char n,int m)
{
    unsigned char i;
	int k=0,j=0; 
	if(n==1) return p;
	else 
	{
        k=m/multi(n-1);
	    j=m%multi(n-1);
	    swap(p,p+k);
	    return permutation(p+1,n-1,j)-1;
	} 
} 
 //检测类型 
char text(int n)
{
	if(n<3) return 'A';
	else return 'B';
 } 
int count=0;
int T[6]={0,1,2,3,4,5};
int p[6];
int g[6];
int tem1[6],tem2[6];
int s=0,i,j,q=0;
int coutol=0;
int con[720]={0};
int min=10000000,dis=0;
int q1,q2,z,k1,k2;//q1=轻症病人1，q2=轻症病人2，z=重症病人，k1=空床位1，k2=空床位2，他们的值表示所对应简化图同一水平线上的节点标号 
char name[][13]={"mild1","mild2","critical","fangcang1","fangcang2","ICU"};
int tem3[6]={0};
int tem4[6]={0};
int total[6][6][6][6][6][20]={0};
int key=1;
int tem5[4]={0};
int tem6[6]={0};
int bed,ren;
main()
{
	//位置初始化 
	for(q1=1;q1<6;q1++)
	    for(q2=0;q2<q1;q2++)//q1,q2位置不同 
	        for(z=0;z<6;z++)
	        {
	        	if( z==q1 || z==q2 ) continue;//z,q1,q2位置不同 
	        	for( k1=2;k1<4;k1++)
	        	    for (k2=1;k2<k1;k2++)
	        	    {
	        	    	min=100000000;//初始化最小值 
	        	    	q=0;
	        	    	for(i=0;i<720;i++) con[i]=0;
	        	    	//将病人和两张床位简化成0-5的数字 
	        	    	p[0]=q1; p[1]=q2;
	        	    	p[2]=z;
	        	    	p[3]=k1; p[4]=k2;
	        	    	p[5]=5;
	        	    	//确定情况下，机器人总共要经过六个点------轻症病人1，轻症病人2，重症病人，空床位1.空床位2，ICU
						//首先全排列六个地点经过的前后顺序 
						for(s=0;s<multi(6);s++)
	        	    	{
	        	    		for(i=0;i<6;i++) T[i]=i;
	        	    		permutation(T,6,s);//T数列T[i]=j表示i(经过地点标号)经过顺序为j 
	        	    		//针对两个轻症病人，两个空床位 
	        	    		if(T[3]<T[0] && T[3]<T[1]) continue;//空床位1不能在四者的最前面 
	        	    		if(T[4]<T[0] && T[4]<T[1]) continue;//同上 
	        	    		if(T[3]<T[0] && T[4]<T[0]) continue;//轻症病人不能在四者的最后面 
	        	    		if(T[3]<T[1] && T[4]<T[1]) continue;//同上 
	        	    		if(T[5]<T[2] ) continue;//ICU不能在重症病人前面 
	        	    		if((abs(T[0]-T[1])+abs(T[0]-T[2])+abs(T[1]-T[2])==4)) continue;//三个病人不能相邻 
	        	    		for(i=0;i<6;i++) g[T[i]]=i;//g数列g[i]=j表示第i个经过的是j(经过地点标号) 
	        	    		//每两个经过地点之间计算距离并且相加 
	        	    		dis=
							distance('O',1,text(g[0]),p[g[0]])+
							distance(text(g[0]),p[g[0]],text(g[1]),p[g[1]])+
	        	    		distance(text(g[1]),p[g[1]],text(g[2]),p[g[2]])+
	        	    		distance(text(g[2]),p[g[2]],text(g[3]),p[g[3]])+
	        	    		distance(text(g[3]),p[g[3]],text(g[4]),p[g[4]])+
	        	    		distance(text(g[4]),p[g[4]],text(g[5]),p[g[5]]);
	        	    		//求最小值并且记录 
	        	    		con[s]=dis;
	        	    		if(dis<min)
	        	    		{
	        	    			min=dis;
	        	    			q=s;
							}
							
						}
						//输出表示
						for(i=0;i<6;i++) {tem3[i]=0; tem4[i]=0;}
						tem3[q1]=1; tem3[q2]=1; tem3[z]=2;
						 tem4[k1]=1; tem4[k2]=1;
					    
						//printf("|mild1 %d |mild2 %d |critical %d |fangcang1 %d |fangcang2 %d |ICU 5\n",q1,q2,z,k1,k2);
						printf("NO.%d:%d%d%d%d%d5\n",count,q1,q2,z,k1,k2); count++;
						printf("         %d       %d       %d               U\n",tem4[1],tem4[2],tem4[3]);
						printf("         |       |       |               |\n");
						printf("/0------/1------/2------/3------/4------/5\n");
						printf(" |       |       |       |       |       |\n");
						printf(" %d       %d       %d       %d       %d       %d\n",tem3[0],tem3[1],tem3[2],tem3[3],tem3[4],tem3[5]);
						coutol=0;
						for(j=0;j<720;j++)
						{
						    if(con[j]==min)
							{
							    total[q1][q2][z][k1][k2][coutol]=j;
							    coutol++;
								for(i=0;i<6;i++) tem1[i]=i;
						        permutation(tem1,6,j);
						        for(i=0;i<6;i++) tem2[tem1[i]]=i;
						        printf("%s(%d)->%s(%d)->%s(%d)->%s(%d)->%s(%d)->%s(%d)  min=%d\n",name[tem2[0]],p[tem2[0]],name[tem2[1]],p[tem2[1]],name[tem2[2]],p[tem2[2]],name[tem2[3]],p[tem2[3]],name[tem2[4]],p[tem2[4]],name[tem2[5]],p[tem2[5]],min);
					        }
						}
						printf("\n");
					}
			}
			
	printf("\nif you want to search condition of fangcangs' positions are 1(the other one is known)\n");	
	printf("and the mild petients' positions is 0 and 5 but critical petient's position is known \n");
	printf("so you can enter:bed: 177 next line petient: 177771");
	printf("  then it'll print all the condition you want\n");
	printf("if it's the occupied fangcang and healthy preson you can enter 0. if it's  critical petient, you can enter 2\n");	
	while(1)
	{
		printf("\n************************************\nsearch or not(search=1/over=0)： "); 
		scanf("%d",&key); if(key==0) break;
	    printf("condition(unknow=7)：\n");
	    printf("bed=： "); 
	    scanf("%d",&bed);
	    printf("patients： ");
	    scanf("%d",&ren);
	    printf("\n");
	    for(q1=1;q1<6;q1++)
	    for(q2=0;q2<q1;q2++)//q1,q2位置不同 
	        for(z=0;z<6;z++)
	        {
	        	if( z==q1 || z==q2 ) continue;//z,q1,q2位置不同 
	        	for( k1=2;k1<4;k1++)
	        	    for (k2=1;k2<k1;k2++)
	        	    {
	        	    	p[0]=q1; p[1]=q2;
	        	    	p[2]=z;
	        	    	p[3]=k1; p[4]=k2;
	        	    	p[5]=5;
						for(i=0;i<4;i++) tem5[i]=0;
	        	    	for(i=0;i<6;i++) tem6[i]=0;
	        	    	tem5[k1]=1; tem5[k2]=1;
						tem6[q1]=1; tem6[q2]=1; tem6[z]=2;
						if(((tem5[1]==bed/100) || bed/100==7) && ((tem5[2]==(bed%100/10)) || (bed%100/10==7) ) && ((tem5[3]==bed%10) || (bed%10==7))) 
						{
							if( ((tem6[0]==ren/power(10,5)) || (ren/power(10,5)==7)) && ((tem6[1]==(ren%power(10,5)/power(10,4))) || ((ren%power(10,5)/power(10,4))==7)) )
							{
								if(((tem6[2]==(ren%power(10,4)/power(10,3))) || ((ren%power(10,4)/power(10,3))==7)) && ((tem6[3]==(ren%power(10,3)/power(10,2))) || ((ren%power(10,3)/power(10,2))==7)))
								{
									if(((tem6[4]==(ren%power(10,2)/power(10,1))) || ((ren%power(10,2)/power(10,1))==7)) && ((tem6[5]==(ren%power(10,1)/power(10,0))) || ((ren%power(10,1)/power(10,0))==7)))
									{
										printf("         %d       %d       %d               U\n",tem5[1],tem5[2],tem5[3]);
						                printf("         |       |       |               |\n");
						                printf("/0------/1------/2------/3------/4------/5\n");
						                printf(" |       |       |       |       |       |\n");
						                printf(" %d       %d       %d       %d       %d       %d\n",tem6[0],tem6[1],tem6[2],tem6[3],tem6[4],tem6[5]);
						                coutol=0;
										while( total[q1][q2][z][k1][k2][coutol])
						                {
						                	for(i=0;i<6;i++) tem1[i]=i;
						                	j=total[q1][q2][z][k1][k2][coutol];
						                    permutation(tem1,6,j);
						                    for(i=0;i<6;i++) tem2[tem1[i]]=i;
						                    printf("%s(%d)->%s(%d)->%s(%d)->%s(%d)->%s(%d)->%s(%d)  min=%d\n",name[tem2[0]],p[tem2[0]],name[tem2[1]],p[tem2[1]],name[tem2[2]],p[tem2[2]],name[tem2[3]],p[tem2[3]],name[tem2[4]],p[tem2[4]],name[tem2[5]],p[tem2[5]],min);
											coutol++;
										}
									}
								}
							}
						}
	        	    }
	        }
	    
	}	
}
