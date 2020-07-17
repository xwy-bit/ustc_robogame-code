#include<stdio.h>
#include<math.h>
//·�߼�ͼ 
//5->ICU = 2700  ICU->5 =700
//5->4=...= 1->0=600
//5->O5=..=0->O0=700
//fc=����ҽԺ  O�ǳ����� 
//fc->3=fc->2=fc->1=700
//����ߵ�ҽԺ��ΪB�࣬�ұߵĲ��˵��ΪA�� 
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
//��������֮��ľ��� 
int distance(char c1,char n1,char c2,char n2)
{
	if( c1 == 'A' && c2=='A') return 700*2+abs(n1-n2)*600;
	if( c1 == 'A' && c2 =='B')
	{
		if (n2 == 5) return 700+600*abs(n1-n2)+2700;//����������
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
//ָ��
int power(int a,int b)
{
	if(b==0) return 1;
	return a*power(a,b-1); 
 } 

//����
void swap(int *a,int *b)
{
	
	int c;
	c=*a;
	*a=*b;
	*b=c;
	
 } 
 //�׳� 
 int multi(unsigned char n )
 {
 	if(n==1) return 1;
 	else return n*multi(n-1);
 }

//ȫ���� permutation(p,n,m)��ʾ��ʼָ��Ϊp��n����ȫ���еĵ�m������˳�򣬱���permutation(p,3,0)���� p�������� p��Ӧ������˳��Ϊ 0 1 2 
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
 //������� 
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
int q1,q2,z,k1,k2;//q1=��֢����1��q2=��֢����2��z=��֢���ˣ�k1=�մ�λ1��k2=�մ�λ2�����ǵ�ֵ��ʾ����Ӧ��ͼͬһˮƽ���ϵĽڵ��� 
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
	//λ�ó�ʼ�� 
	for(q1=1;q1<6;q1++)
	    for(q2=0;q2<q1;q2++)//q1,q2λ�ò�ͬ 
	        for(z=0;z<6;z++)
	        {
	        	if( z==q1 || z==q2 ) continue;//z,q1,q2λ�ò�ͬ 
	        	for( k1=2;k1<4;k1++)
	        	    for (k2=1;k2<k1;k2++)
	        	    {
	        	    	min=100000000;//��ʼ����Сֵ 
	        	    	q=0;
	        	    	for(i=0;i<720;i++) con[i]=0;
	        	    	//�����˺����Ŵ�λ�򻯳�0-5������ 
	        	    	p[0]=q1; p[1]=q2;
	        	    	p[2]=z;
	        	    	p[3]=k1; p[4]=k2;
	        	    	p[5]=5;
	        	    	//ȷ������£��������ܹ�Ҫ����������------��֢����1����֢����2����֢���ˣ��մ�λ1.�մ�λ2��ICU
						//����ȫ���������ص㾭����ǰ��˳�� 
						for(s=0;s<multi(6);s++)
	        	    	{
	        	    		for(i=0;i<6;i++) T[i]=i;
	        	    		permutation(T,6,s);//T����T[i]=j��ʾi(�����ص���)����˳��Ϊj 
	        	    		//���������֢���ˣ������մ�λ 
	        	    		if(T[3]<T[0] && T[3]<T[1]) continue;//�մ�λ1���������ߵ���ǰ�� 
	        	    		if(T[4]<T[0] && T[4]<T[1]) continue;//ͬ�� 
	        	    		if(T[3]<T[0] && T[4]<T[0]) continue;//��֢���˲��������ߵ������ 
	        	    		if(T[3]<T[1] && T[4]<T[1]) continue;//ͬ�� 
	        	    		if(T[5]<T[2] ) continue;//ICU��������֢����ǰ�� 
	        	    		if((abs(T[0]-T[1])+abs(T[0]-T[2])+abs(T[1]-T[2])==4)) continue;//�������˲������� 
	        	    		for(i=0;i<6;i++) g[T[i]]=i;//g����g[i]=j��ʾ��i����������j(�����ص���) 
	        	    		//ÿ���������ص�֮�������벢����� 
	        	    		dis=
							distance('O',1,text(g[0]),p[g[0]])+
							distance(text(g[0]),p[g[0]],text(g[1]),p[g[1]])+
	        	    		distance(text(g[1]),p[g[1]],text(g[2]),p[g[2]])+
	        	    		distance(text(g[2]),p[g[2]],text(g[3]),p[g[3]])+
	        	    		distance(text(g[3]),p[g[3]],text(g[4]),p[g[4]])+
	        	    		distance(text(g[4]),p[g[4]],text(g[5]),p[g[5]]);
	        	    		//����Сֵ���Ҽ�¼ 
	        	    		con[s]=dis;
	        	    		if(dis<min)
	        	    		{
	        	    			min=dis;
	        	    			q=s;
							}
							
						}
						//�����ʾ
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
		printf("\n************************************\nsearch or not(search=1/over=0)�� "); 
		scanf("%d",&key); if(key==0) break;
	    printf("condition(unknow=7)��\n");
	    printf("bed=�� "); 
	    scanf("%d",&bed);
	    printf("patients�� ");
	    scanf("%d",&ren);
	    printf("\n");
	    for(q1=1;q1<6;q1++)
	    for(q2=0;q2<q1;q2++)//q1,q2λ�ò�ͬ 
	        for(z=0;z<6;z++)
	        {
	        	if( z==q1 || z==q2 ) continue;//z,q1,q2λ�ò�ͬ 
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
