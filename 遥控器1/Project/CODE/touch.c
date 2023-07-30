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




//SPI写数据
//向触摸屏IC写入1byte数据    
//num:要写入的数据
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
		TOUCH_CLK_SET;		//上升沿有效	        
	}		 			    
} 		 

uint16 TP_Read_AD(uint8 CMD)	  
{ 	 
	u8 count=0; 	  
	u16 Num=0; 
	TOUCH_CLK_CLE;		//先拉低时钟 	 
	TOUCH_MO_CLE; 	//拉低数据线
	TOUCH_CS_CLE; 		//选中触摸屏IC
	TP_Write_Byte(CMD);//发送命令字
	systick_delay_us(6);//ADS7846的转换时间最长为6us
	TOUCH_CLK_CLE; 	     	    
	systick_delay_us(1);    	   
	TOUCH_CLK_SET;		//给1个时钟，清除BUSY
	systick_delay_us(1);    
	TOUCH_CLK_CLE; 	     	    
	for(count=0;count<16;count++)//读出16位数据,只有高12位有效 
	{ 				  
		Num<<=1; 	 
		TOUCH_CLK_CLE;	//下降沿有效  	    	   
		systick_delay_us(1);    
 		TOUCH_CLK_SET;
 		if(TOUCH_MI_GET)Num++; 		 
	}  	
	Num>>=4;   	//只有高12位有效.
	TOUCH_CS_SET;		//释放片选	 
	

	return(Num);   
}

//读取一个坐标值(x或者y)
//连续读取READ_TIMES次数据,对这些数据升序排列,
//然后去掉最低和最高LOST_VAL个数,取平均值 
//xy:指令（CMD_RDX/CMD_RDY）
//返回值:读到的数据
#define READ_TIMES 5 	//读取次数
#define LOST_VAL 1	  	//丢弃值
uint16 TP_Read_XOY(uint8 xy)
{
	uint16 i, j;
	uint16 buf[READ_TIMES];
	uint16 sum=0;
	uint16 temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=TP_Read_AD(xy);		 		    
	for(i=0;i<READ_TIMES-1; i++)//排序
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//升序排列
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


//读取x,y坐标
//最小值不能少于100.
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
uint8 TP_Read_XY(uint16 *x,uint16 *y)
{
	u16 xtemp,ytemp;			 	 		  
	xtemp=TP_Read_XOY(CMD_RDX);
	ytemp=TP_Read_XOY(CMD_RDY);	  												   
	//if(xtemp<100||ytemp<100)return 0;//读数失败
	*x=xtemp;
	*y=ytemp;
	return 1;//读数成功
}

//连续2次读取触摸屏IC,且这两次的偏差不能超过
//ERR_RANGE,满足条件,则认为读数正确,否则读数错误.	   
//该函数能大大提高准确度
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
#define ERR_RANGE 50 //误差范围 
uint8 TP_Read_XY2(uint16 *x,uint16 *y) 
{
	uint16 x1,y1;
 	uint16 x2,y2;
 	uint8 flag;    
    flag=TP_Read_XY(&x1,&y1);   
    if(flag==0)return(0);
    flag=TP_Read_XY(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//前后两次采样在+-50内
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

		if(TP_Read_XY2(&tp_pos.y_current,&tp_pos.x_current)&& tp_sta.tp_sta_current==0)//之前没有被按下
		{
			tp_sta.tp_press_turn=1;//标记按键按下未处理
			//记下当前坐标点
			tp_pos.x_current=0.127659*tp_pos.x_current-20;
			tp_pos.y_current=-0.090607*tp_pos.y_current+349; 
		}
		tp_sta.tp_sta_current=1;//标记为按键按下
	}else
	{
		if(tp_sta.tp_sta_current==1)//之前按下了
		{
			tp_sta.tp_release_turn=1;//标记按键松开未处理
			//记下上次坐标点
			tp_pos.x_last=tp_pos.x_current;
			tp_pos.y_last=tp_pos.y_current;
		}
		tp_sta.tp_sta_current=0;//标记为按键松开
	}
}


//触摸屏初始化
void TP_Init(void)
{	
	touch_pin_init();	
	TP_Read_XY(&tp_pos.x_current,&tp_pos.y_current);//第一次读取初始化	
	 									 
}








