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


#define LCD_CS C0//Ƭѡ
#define LCD_WR C14//��
#define LCD_RD C13//д
#define LCD_RS C15//�������
#define LCD_RST B8//��λ
#define LCD_LED C8//����

//LCD��Ҫ������
typedef struct  
{
    uint16 width;      //LCD ���
    uint16 height;     //LCD �߶�
    uint8  dir;        //���������������ƣ�0��������1��������
    uint16 wramcmd;    //��ʼдgramָ��
    uint16 setxcmd;    //����x����ָ��
    uint16  setycmd;   //����y����ָ��
} _lcd_dev;



//LCD����
extern _lcd_dev lcddev; //����LCD��Ҫ����

extern uint16  POINT_COLOR;//������ɫ��Ĭ��Ϊ��ɫ    
extern uint16  BACK_COLOR; //������ɫ.Ĭ��Ϊ��ɫ

/***************����************/

#define LCD_WRITE_GRAM LCD_WR_REG(0X2C)//дGRAM����
#define LCD_WRITE_X LCD_WR_REG(0X2A)//дx��������
#define LCD_WRITE_Y LCD_WR_REG(0X2B)//дy��������

/*******************************/

 



/********��λ********/
#define LCD_CS_SET  gpio_set(LCD_CS,1)    //Ƭѡ�˿�      PC9
#define LCD_RS_SET  gpio_set(LCD_RS,1)    //����/����     PC8
#define LCD_WR_SET  gpio_set(LCD_WR,1)   //д����        PC7
#define LCD_RD_SET  gpio_set(LCD_RD,1)    //������        PC6

/********���********/
#define LCD_CS_CLR  gpio_set(LCD_CS,0)    //Ƭѡ�˿�      PC9
#define LCD_RS_CLR  gpio_set(LCD_RS,0)     //����/����     PC8
#define LCD_WR_CLR  gpio_set(LCD_WR,0)     //д����        PC7
#define LCD_RD_CLR  gpio_set(LCD_RD,0)     //������        PC6


//16λ�����������



/**********************************************************************/
//ɨ�跽����
#define L2R_U2D  0 //������,���ϵ���
#define L2R_D2U  1 //������,���µ���
#define R2L_U2D  2 //���ҵ���,���ϵ���
#define R2L_D2U  3 //���ҵ���,���µ���

#define U2D_L2R  4 //���ϵ���,������
#define U2D_R2L  5 //���ϵ���,���ҵ���
#define D2U_L2R  6 //���µ���,������
#define D2U_R2L  7 //���µ���,���ҵ���

#define DFT_SCAN_DIR  L2R_U2D  //Ĭ�ϵ�ɨ�跽��

//д���ݺ���
#define LCD_WR_DATA(data){\
        LCD_RS_SET;\
        LCD_CS_CLR;\
        DATAOUT(data);\
        LCD_WR_CLR;\
        LCD_WR_SET;\
        LCD_CS_SET;\
    }



/*********************************************************************/

/*********565��ɫ��ʽ**************/
//������ɫ
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
#define BROWN           0XBC40  //��ɫ
#define BRRED           0XFC07  //�غ�ɫ
#define GRAY            0X8430  //��ɫ
//GUI��ɫ

#define DARKBLUE        0X01CF  //����ɫ
#define LIGHTBLUE       0X7D7C  //ǳ��ɫ  
#define GRAYBLUE        0X5458  //����ɫ
//������ɫΪPANEL����ɫ

#define LIGHTGREEN      0X841F  //ǳ��ɫ 
#define LGRAY           0XC618  //ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE       0XA651  //ǳ����ɫ(�м����ɫ)
#define LBBLUE          0X2B12  //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)

#define ADD_COLOR 0xabff




/***********��������************************/
void DATAOUT(u16 data);
void LCD_WR_REG(uint16 cmd);//д�Ĵ�����Ż������
void LCD_WR_DATAX(uint16 data);//д���ݺ���
void opt_delay(uint8 i);//��ʱ
void LCD_WriteReg(uint16 LCD_Reg,uint16 LCD_RegValue);//д�Ĵ���
void LCD_WriteRAM_Prepare(void);//��ʼдGRAM����������ú������
void LCD_WriteRAM(uint16 RGB_Code);//LCDдGRAM����������д��
void LCD_SetCursor(uint16 Xpos, uint16 Ypos);//���ù��λ��,ͨ��ֻ��Ҫ�ڳ�ʼ��������һ��ex,ey����
void LCD_DisplayOn(void);//LCD������ʾ
void LCD_DisplayOff(void);//LCD�ر���ʾ
void LCD_Scan_Dir(uint8 dir);//����LCD���Զ�ɨ�跽��
void LCD_DrawPoint(uint16 x,uint16 y,uint16 color);//����
void LCD_SSD_BackLightSet(uint8 pwm);//��������
void LCD_Display_Dir(uint8 dir);//����LCD��ʾ����
void LCD_Set_Window(uint16 sx,uint16 sy,uint16 width,uint16 height);//���ô�������
void LCD_Clear(uint16 color);//��ָ����ɫ�����Ļ
void lcd_pin_init();//�������ų�ʼ��
void LCD_Fill(uint16 sx,uint16 sy,uint16 ex,uint16 ey,uint16 color);//��ָ�����������ָ����ɫ
void LCD_Draw_Circle(uint16 x0,uint16 y0,uint8 r);//��ָ��λ�û�һ��ָ����С��Բ
void LCD_Init(void);//���³�ʼ��













void lcd_test();//lcd����




#endif