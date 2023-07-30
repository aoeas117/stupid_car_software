#include "touch.h"
#include "zf_gpio.h"
#include "zf_systick.h"
#include "lcd.h"
#include "gui.h"
#include "page.h"




TP_POS tp_pos={0};
TP_STA tp_sta={0};







void touch_pin_init()
{
	gpio_init(TOUCH_CLK,GPO,1,GPO_PUSH_PULL);
	gpio_init(TOUCH_CS,GPO,1,GPO_PUSH_PULL);
	gpio_init(TOUCH_MO,GPO,1,GPO_PUSH_PULL);
	gpio_init(TOUCH_PEN,GPI,0,GPI_PULL_UP);
	gpio_init(TOUCH_MI,GPI,0,GPI_PULL_UP);
}




//SPIд����
//������ICд��1byte����    
//num:Ҫд�������
void TP_Write_Byte(uint8 num)    
{  
	uint8 count=0;   
	for(count=0;count<8;count++)  
	{ 	  
		if(num&0x80)TOUCH_MO_SET;  
		else TOUCH_MO_CLE;   
		num<<=1;    
		TOUCH_CLK_CLE; 
		systick_delay_us(1);
		TOUCH_CLK_SET;		//��������Ч	        
	}		 			    
} 		 

uint16 TP_Read_AD(uint8 CMD)	  
{ 	 
	u8 count=0; 	  
	u16 Num=0; 
	TOUCH_CLK_CLE;		//������ʱ�� 	 
	TOUCH_MO_CLE; 	//����������
	TOUCH_CS_CLE; 		//ѡ�д�����IC
	TP_Write_Byte(CMD);//����������
	systick_delay_us(6);//ADS7846��ת��ʱ���Ϊ6us
	TOUCH_CLK_CLE; 	     	    
	systick_delay_us(1);    	   
	TOUCH_CLK_SET;		//��1��ʱ�ӣ����BUSY
	systick_delay_us(1);    
	TOUCH_CLK_CLE; 	     	    
	for(count=0;count<16;count++)//����16λ����,ֻ�и�12λ��Ч 
	{ 				  
		Num<<=1; 	 
		TOUCH_CLK_CLE;	//�½�����Ч  	    	   
		systick_delay_us(1);    
 		TOUCH_CLK_SET;
 		if(TOUCH_MI_GET)Num++; 		 
	}  	
	Num>>=4;   	//ֻ�и�12λ��Ч.
	TOUCH_CS_SET;		//�ͷ�Ƭѡ	 
	

	return(Num);   
}

//��ȡһ������ֵ(x����y)
//������ȡREAD_TIMES������,����Щ������������,
//Ȼ��ȥ����ͺ����LOST_VAL����,ȡƽ��ֵ 
//xy:ָ�CMD_RDX/CMD_RDY��
//����ֵ:����������
#define READ_TIMES 5 	//��ȡ����
#define LOST_VAL 1	  	//����ֵ
uint16 TP_Read_XOY(uint8 xy)
{
	uint16 i, j;
	uint16 buf[READ_TIMES];
	uint16 sum=0;
	uint16 temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=TP_Read_AD(xy);		 		    
	for(i=0;i<READ_TIMES-1; i++)//����
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//��������
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 


//��ȡx,y����
//��Сֵ��������100.
//x,y:��ȡ��������ֵ
//����ֵ:0,ʧ��;1,�ɹ���
uint8 TP_Read_XY(uint16 *x,uint16 *y)
{
	u16 xtemp,ytemp;			 	 		  
	xtemp=TP_Read_XOY(CMD_RDX);
	ytemp=TP_Read_XOY(CMD_RDY);	  												   
	//if(xtemp<100||ytemp<100)return 0;//����ʧ��
	*x=xtemp;
	*y=ytemp;
	return 1;//�����ɹ�
}

//����2�ζ�ȡ������IC,�������ε�ƫ��ܳ���
//ERR_RANGE,��������,����Ϊ������ȷ,�����������.	   
//�ú����ܴ�����׼ȷ��
//x,y:��ȡ��������ֵ
//����ֵ:0,ʧ��;1,�ɹ���
#define ERR_RANGE 50 //��Χ 
uint8 TP_Read_XY2(uint16 *x,uint16 *y) 
{
	uint16 x1,y1;
 	uint16 x2,y2;
 	uint8 flag;    
    flag=TP_Read_XY(&x1,&y1);   
    if(flag==0)return(0);
    flag=TP_Read_XY(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//ǰ�����β�����+-50��
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
}  






	  


void tp_scan(void)
{
	if(TOUCH_PEN_GET==0)
	{

		if(TP_Read_XY2(&tp_pos.y_current,&tp_pos.x_current)&& tp_sta.tp_sta_current==0)//֮ǰû�б�����
		{
			tp_sta.tp_press_turn=1;//��ǰ�������δ����
			//���µ�ǰ�����
			tp_pos.x_current=0.127659*tp_pos.x_current-20;
			tp_pos.y_current=-0.090607*tp_pos.y_current+349; 
		}
		tp_sta.tp_sta_current=1;//���Ϊ��������
	}else
	{
		if(tp_sta.tp_sta_current==1)//֮ǰ������
		{
			tp_sta.tp_release_turn=1;//��ǰ����ɿ�δ����
			//�����ϴ������
			tp_pos.x_last=tp_pos.x_current;
			tp_pos.y_last=tp_pos.y_current;
		}
		tp_sta.tp_sta_current=0;//���Ϊ�����ɿ�
	}
}


//��������ʼ��
void TP_Init(void)
{	
	touch_pin_init();	
	TP_Read_XY(&tp_pos.x_current,&tp_pos.y_current);//��һ�ζ�ȡ��ʼ��	
	 									 
}








