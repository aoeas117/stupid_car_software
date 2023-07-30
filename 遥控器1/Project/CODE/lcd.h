#ifndef __LCD_H
#define __LCD_H		

#include "common.h"

#define BIT0 B7
#define BIT1 B6
#define BIT2 B5
#define BIT3 B4
#define BIT4 B3
#define BIT5 D6
#define BIT6 C12
#define BIT7 C11
#define BIT8 C10
#define BIT9 A15
#define BIT10 D3
#define BIT11 D2
#define BIT12 A12
#define BIT13 A11
#define BIT14 A8
#define BIT15 C9


#define LCD_CS C0//片选
#define LCD_WR C14//读
#define LCD_RD C13//写
#define LCD_RS C15//命令，数据
#define LCD_RST B8//复位
#define LCD_LED C8//背光

//LCD重要参数集
typedef struct  
{
    uint16 width;      //LCD 宽度
    uint16 height;     //LCD 高度
    uint8  dir;        //横屏还是竖屏控制：0，竖屏；1，横屏。
    uint16 wramcmd;    //开始写gram指令
    uint16 setxcmd;    //设置x坐标指令
    uint16  setycmd;   //设置y坐标指令
} _lcd_dev;



//LCD参数
extern _lcd_dev lcddev; //管理LCD重要参数

extern uint16  POINT_COLOR;//画笔颜色，默认为红色    
extern uint16  BACK_COLOR; //背景颜色.默认为白色

/***************命令************/

#define LCD_WRITE_GRAM LCD_WR_REG(0X2C)//写GRAM命令
#define LCD_WRITE_X LCD_WR_REG(0X2A)//写x坐标命令
#define LCD_WRITE_Y LCD_WR_REG(0X2B)//写y坐标命令

/*******************************/

 



/********置位********/
#define LCD_CS_SET  gpio_set(LCD_CS,1)    //片选端口      PC9
#define LCD_RS_SET  gpio_set(LCD_RS,1)    //数据/命令     PC8
#define LCD_WR_SET  gpio_set(LCD_WR,1)   //写数据        PC7
#define LCD_RD_SET  gpio_set(LCD_RD,1)    //读数据        PC6

/********清除********/
#define LCD_CS_CLR  gpio_set(LCD_CS,0)    //片选端口      PC9
#define LCD_RS_CLR  gpio_set(LCD_RS,0)     //数据/命令     PC8
#define LCD_WR_CLR  gpio_set(LCD_WR,0)     //写数据        PC7
#define LCD_RD_CLR  gpio_set(LCD_RD,0)     //读数据        PC6


//16位数据输入输出



/**********************************************************************/
//扫描方向定义
#define L2R_U2D  0 //从左到右,从上到下
#define L2R_D2U  1 //从左到右,从下到上
#define R2L_U2D  2 //从右到左,从上到下
#define R2L_D2U  3 //从右到左,从下到上

#define U2D_L2R  4 //从上到下,从左到右
#define U2D_R2L  5 //从上到下,从右到左
#define D2U_L2R  6 //从下到上,从左到右
#define D2U_R2L  7 //从下到上,从右到左

#define DFT_SCAN_DIR  L2R_U2D  //默认的扫描方向

//写数据函数
#define LCD_WR_DATA(data){\
        LCD_RS_SET;\
        LCD_CS_CLR;\
        DATAOUT(data);\
        LCD_WR_CLR;\
        LCD_WR_SET;\
        LCD_CS_SET;\
    }



/*********************************************************************/

/*********565颜色格式**************/
//画笔颜色
#define WHITE           0xFFFF
#define BLACK           0x0000
#define BLUE            0x001F
#define BRED            0XF81F
#define GRED            0XFFE0
#define GBLUE           0X07FF
#define RED             0xF800
#define MAGENTA         0xF81F
#define GREEN           0x07E0
#define CYAN            0x7FFF
#define YELLOW          0xFFE0
#define BROWN           0XBC40  //棕色
#define BRRED           0XFC07  //棕红色
#define GRAY            0X8430  //灰色
//GUI颜色

#define DARKBLUE        0X01CF  //深蓝色
#define LIGHTBLUE       0X7D7C  //浅蓝色  
#define GRAYBLUE        0X5458  //灰蓝色
//以上三色为PANEL的颜色

#define LIGHTGREEN      0X841F  //浅绿色 
#define LGRAY           0XC618  //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE       0XA651  //浅灰蓝色(中间层颜色)
#define LBBLUE          0X2B12  //浅棕蓝色(选择条目的反色)

#define ADD_COLOR 0xabff




/***********函数声明************************/
void DATAOUT(u16 data);
void LCD_WR_REG(uint16 cmd);//写寄存器编号或命令函数
void LCD_WR_DATAX(uint16 data);//写数据函数
void opt_delay(uint8 i);//延时
void LCD_WriteReg(uint16 LCD_Reg,uint16 LCD_RegValue);//写寄存器
void LCD_WriteRAM_Prepare(void);//开始写GRAM的命令（可以用宏替代）
void LCD_WriteRAM(uint16 RGB_Code);//LCD写GRAM（可以连续写）
void LCD_SetCursor(uint16 Xpos, uint16 Ypos);//设置光标位置,通常只需要在初始化中设置一次ex,ey坐标
void LCD_DisplayOn(void);//LCD开启显示
void LCD_DisplayOff(void);//LCD关闭显示
void LCD_Scan_Dir(uint8 dir);//设置LCD的自动扫描方向
void LCD_DrawPoint(uint16 x,uint16 y,uint16 color);//画点
void LCD_SSD_BackLightSet(uint8 pwm);//背光设置
void LCD_Display_Dir(uint8 dir);//设置LCD显示方向
void LCD_Set_Window(uint16 sx,uint16 sy,uint16 width,uint16 height);//设置窗户区域
void LCD_Clear(uint16 color);//以指定颜色填充屏幕
void lcd_pin_init();//理查德引脚初始化
void LCD_Fill(uint16 sx,uint16 sy,uint16 ex,uint16 ey,uint16 color);//在指定区域内填充指定颜色
void LCD_Draw_Circle(uint16 x0,uint16 y0,uint8 r);//在指定位置画一个指定大小的圆
void LCD_Init(void);//理查德初始化













void lcd_test();//lcd测试




#endif