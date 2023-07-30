#include "lcd.h"
#include "hal_gpio.h"
#include "zf_gpio.h"
#include "zf_systick.h"
#include "hal_rcc.h"

//LCD�Ļ�����ɫ�ͱ���ɫ
uint16 POINT_COLOR=RED;     //������ɫ
uint16 BACK_COLOR=0xFFFF;      //����ɫ 


//����LCD��Ҫ����
_lcd_dev lcddev;



void DATAOUT(u16 data)
{
	u8 i=0;
	gpio_set(BIT0,(data>>(i++))&(0x0001));
	gpio_set(BIT1,(data>>(i++))&(0x0001));
	gpio_set(BIT2,(data>>(i++))&(0x0001));
	gpio_set(BIT3,(data>>(i++))&(0x0001));
	gpio_set(BIT4,(data>>(i++))&(0x0001));
	gpio_set(BIT5,(data>>(i++))&(0x0001));
	gpio_set(BIT6,(data>>(i++))&(0x0001));
	gpio_set(BIT7,(data>>(i++))&(0x0001));
	gpio_set(BIT8,(data>>(i++))&(0x0001));
	gpio_set(BIT9,(data>>(i++))&(0x0001));
	gpio_set(BIT10,(data>>(i++))&(0x0001));
	gpio_set(BIT11,(data>>(i++))&(0x0001));
	gpio_set(BIT12,(data>>(i++))&(0x0001));
	gpio_set(BIT13,(data>>(i++))&(0x0001));
	gpio_set(BIT14,(data>>(i++))&(0x0001));
	gpio_set(BIT15,(data>>(i++))&(0x0001));
}


//д�Ĵ�����Ż������
//cmd:�Ĵ�����Ż�����
void LCD_WR_REG(uint16 cmd)
{ 
    LCD_RS_CLR;//����
    LCD_CS_CLR;//����Ƭѡ
    DATAOUT(cmd);//������͵�GPIO��
    LCD_WR_CLR;//ʹ��д�ź�
    LCD_WR_SET;//�ر�д�ź�
    LCD_CS_SET;//�ر�Ƭѡ
}

//д���ݺ���
//data:�Ĵ���ֵ
void LCD_WR_DATAX(uint16 data)
{
    LCD_RS_SET;//����
    LCD_CS_CLR;//����Ƭѡ
    DATAOUT(data);//�����ݷ��͵�GPIO��
    LCD_WR_CLR;//ʹ��д�ź�
    LCD_WR_SET;//�ر�д�ź�
    LCD_CS_SET;//�ر�Ƭѡ
}

void opt_delay(uint8 i)//��ʱ
{
    while (i--);
}


//д�Ĵ���
//LCD_Reg:�Ĵ������
//LCD_RegValue:Ҫд���ֵ 
//��ĳ�����¼Ĵ���д��ĳ��ֵ
void LCD_WriteReg(uint16 LCD_Reg,uint16 LCD_RegValue)
{
    LCD_WR_REG(LCD_Reg);  //д��Ҫд�ļĴ���
    LCD_WR_DATA(LCD_RegValue);//д��ֵ
}





//��ʼдGRAM����������ú������
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
}



//LCDдGRAM����������д��
//RGB_Code:��ɫֵ
void LCD_WriteRAM(uint16 RGB_Code)
{
	LCD_WR_DATA(RGB_Code);      //дʮ��λGRAM
}





//���ù��λ��,ͨ��ֻ��Ҫ�ڳ�ʼ��������һ��ex,ey����
//Xpos:������
//Ypos:������
void LCD_SetCursor(uint16 Xpos, uint16 Ypos)
{
	LCD_WR_REG(lcddev.setxcmd);//д��x��������
  LCD_WR_DATA(Xpos >> 8);//������д��16λ��sx����ֵ
  LCD_WR_DATA(Xpos & 0XFF);
  LCD_WR_REG(lcddev.setycmd);//д��y��������
  LCD_WR_DATA(Ypos >> 8);//������д��16λ��sy����ֵ
  LCD_WR_DATA(Ypos & 0XFF);
}






//LCD������ʾ
void LCD_DisplayOn(void)
{
	LCD_WR_REG(0X29);       //������ʾ 
}

//LCD�ر���ʾ
void LCD_DisplayOff(void)
{ 
	LCD_WR_REG(0X28);       //�ر���ʾ  
}


//����LCD���Զ�ɨ�跽��
//dir:0~7,����8������(���嶨���lcd.h)
//ע��:�����������ܻ��ܵ��˺������õ�Ӱ��,
//һ������ΪL2R_U2D����,�������Ϊ����ɨ�跽ʽ,���ܵ�����ʾ������.
void LCD_Scan_Dir(uint8 dir)
{
  uint16 regval = 0;
	uint16 dirreg = 0;
  uint16 temp;

    //����ʱ����1963���ı�ɨ�跽��, ����IC�ı�ɨ�跽������ʱ1963�ı䷽��, ����IC���ı�ɨ�跽��
    if (lcddev.dir == 1)
    {
        switch (dir)   //����ת��
        {
            case 0:
                dir = 6;
                break;

            case 1:
                dir = 7;
                break;

            case 2:
                dir = 4;
                break;

            case 3:
                dir = 5;
                break;

            case 4:
                dir = 1;
                break;

            case 5:
                dir = 0;
                break;

            case 6:
                dir = 3;
                break;

            case 7:
                dir = 2;
                break;
        }
    }

    switch (dir)
    {
        case L2R_U2D://������,���ϵ���
            regval |= (0 << 7) | (0 << 6) | (0 << 5);
            break;

        case L2R_D2U://������,���µ���
            regval |= (1 << 7) | (0 << 6) | (0 << 5);
            break;

        case R2L_U2D://���ҵ���,���ϵ���
            regval |= (0 << 7) | (1 << 6) | (0 << 5);
            break;

        case R2L_D2U://���ҵ���,���µ���
            regval |= (1 << 7) | (1 << 6) | (0 << 5);
            break;

        case U2D_L2R://���ϵ���,������
            regval |= (0 << 7) | (0 << 6) | (1 << 5);
            break;

        case U2D_R2L://���ϵ���,���ҵ���
            regval |= (0 << 7) | (1 << 6) | (1 << 5);
            break;

        case D2U_L2R://���µ���,������
            regval |= (1 << 7) | (0 << 6) | (1 << 5);
            break;

        case D2U_R2L://���µ���,���ҵ���
            regval |= (1 << 7) | (1 << 6) | (1 << 5);
            break;
    }

	dirreg = 0X36;
  LCD_WriteReg(dirreg, regval);
  if (regval & 0X20)
  {
		if (lcddev.width < lcddev.height)   //����X,Y
    {
			temp = lcddev.width;
      lcddev.width = lcddev.height;
      lcddev.height = temp;
    }
  }
  else
	{
		if (lcddev.width > lcddev.height)   //����X,Y
    {
			temp = lcddev.width;
      lcddev.width = lcddev.height;
      lcddev.height = temp;
    }
  }
    

    //������ʾ����(����)��С(��СΪ����ҳ��)
 
	LCD_WR_REG(lcddev.setxcmd);
  LCD_WR_DATA(0);
  LCD_WR_DATA(0);
	LCD_WR_DATA((lcddev.width - 1) >> 8);
  LCD_WR_DATA((lcddev.width - 1) & 0XFF);
	LCD_WR_REG(lcddev.setycmd);
	LCD_WR_DATA(0);
	LCD_WR_DATA(0);
	LCD_WR_DATA((lcddev.height - 1) >> 8);
	LCD_WR_DATA((lcddev.height - 1) & 0XFF);
    
}

//����
//x,y:����
//color:�˵����ɫ
void LCD_DrawPoint(uint16 x,uint16 y,uint16 color)
{
	LCD_SetCursor(x, y);        //���ù��λ��
  LCD_WRITE_GRAM;    //��ʼд��GRAM
  LCD_WR_DATA(color);
}

//��������
//pwm:����ȼ�,0~100.Խ��Խ��.
void LCD_SSD_BackLightSet(uint8 pwm)
{
	LCD_WR_REG(0xBE);           //����PWM���
  LCD_WR_DATA(0x05);          //1����PWMƵ��
	LCD_WR_DATA(pwm * 2.55);    //2����PWMռ�ձ�
	LCD_WR_DATA(0x01);          //3����C
	LCD_WR_DATA(0xFF);          //4����D
	LCD_WR_DATA(0x00);          //5����E
	LCD_WR_DATA(0x00);          //6����F
}

//����LCD��ʾ����
//dir:0,������1,����
void LCD_Display_Dir(uint8 dir)
{
	lcddev.wramcmd = 0X2C;//дgram����
  lcddev.setxcmd = 0X2A;//дx��������
  lcddev.setycmd = 0X2B;//дy��������
	if(dir==0)//����
	{
		lcddev.width = 320;
    lcddev.height = 480;
	}else//����
	{
		lcddev.width = 480;
    lcddev.height = 320;
	}
	LCD_Scan_Dir(D2U_L2R);//Ĭ�������ң��ϵ���
}




//���ô���,���Զ����û������굽�������Ͻ�(sx,sy).
//sx,sy:������ʼ����(���Ͻ�)
//width,height:���ڿ�Ⱥ͸߶�,�������0!!
//�����С:width*height. 
void LCD_Set_Window(uint16 sx,uint16 sy,uint16 width,uint16 height)
{    
	u16 twidth, theight;
	twidth = sx + width - 1;
	theight = sy + height - 1;
	LCD_WR_REG(lcddev.setxcmd);
	LCD_WR_DATA(sx >> 8);
	LCD_WR_DATA(sx & 0XFF);
	LCD_WR_DATA(twidth >> 8);
	LCD_WR_DATA(twidth & 0XFF);
	LCD_WR_REG(lcddev.setycmd);
	LCD_WR_DATA(sy >> 8);
	LCD_WR_DATA(sy & 0XFF);
	LCD_WR_DATA(theight >> 8);
	LCD_WR_DATA(theight & 0XFF);    
}

//��������
//color:Ҫ���������ɫ
void LCD_Clear(uint16 color)
{
	u32 index = 0;
	u32 totalpoint = lcddev.width*lcddev.height; //�õ��ܵ���
	LCD_SetCursor(0,0);    //���ù��λ��
	LCD_WRITE_GRAM;//��ʼд��GRAM
	for (index = 0; index < totalpoint; index++)
	{
			LCD_WR_DATA(color);
	}
}





void lcd_pin_init()//lcd�ܽų�ʼ��
{
	GPIO_InitTypeDef gpio_init_struct;
	RCC_APB2PeriphClockCmd(RCC_AHBENR_GPIOA|RCC_AHBENR_GPIOB|RCC_AHBENR_GPIOC|RCC_AHBENR_GPIOD, ENABLE);
	/****16λ�������ų�ʼ��*****/
	gpio_init_struct.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_15;
  gpio_init_struct.GPIO_Mode=GPIO_Mode_Out_PP;  	//�������         
  gpio_init_struct.GPIO_Speed=GPIO_Speed_50MHz;    //��
  GPIO_Init(GPIOA,&gpio_init_struct);
	GPIOA->BSRR|=GPIO_Pin_8|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_15;
	
	gpio_init_struct.GPIO_Pin=GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;
	GPIO_Init(GPIOB,&gpio_init_struct);
	GPIOB->BSRR|=GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;
	
	gpio_init_struct.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_Init(GPIOC,&gpio_init_struct);
	GPIOC->BSRR|=GPIO_Pin_0|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	
	gpio_init_struct.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_6;
	GPIO_Init(GPIOD,&gpio_init_struct);
	GPIOD->BSRR|=GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_6;
	/***************************/
	

}



//��ָ�����������ָ����ɫ
//�����С:(xend-xsta+1)*(yend-ysta+1)
//xsta
//color:Ҫ������ɫ
void LCD_Fill(uint16 sx,uint16 sy,uint16 ex,uint16 ey,uint16 color)
{
    uint16 i, j;
    uint16 xlen = 0;

    xlen = ex - sx + 1;

    for (i = sy; i <= ey; i++)
    {
        LCD_SetCursor(sx, i);       //���ù��λ��
        LCD_WriteRAM_Prepare();     //��ʼд��GRAM

        for (j = 0; j < xlen; j++)
        {
            LCD_WR_DATA(color);     //���ù��λ��
        }
    }
}





void LCD_Draw_Circle(uint16 x0,uint16 y0,uint8 r)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);       //�ж��¸���λ�õı�־

    while (a <= b)
    {
        LCD_DrawPoint(x0 + a, y0 - b, POINT_COLOR);        //5
        LCD_DrawPoint(x0 + b, y0 - a, POINT_COLOR);        //0
        LCD_DrawPoint(x0 + b, y0 + a, POINT_COLOR);        //4
        LCD_DrawPoint(x0 + a, y0 + b, POINT_COLOR);        //6
        LCD_DrawPoint(x0 - a, y0 + b, POINT_COLOR);        //1
        LCD_DrawPoint(x0 - b, y0 + a, POINT_COLOR);
        LCD_DrawPoint(x0 - a, y0 - b, POINT_COLOR);        //2
        LCD_DrawPoint(x0 - b, y0 - a, POINT_COLOR);        //7
        a++;

        //ʹ��Bresenham�㷨��Բ
        if (di < 0)di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}
















void LCD_Init(void)
{
	lcd_pin_init();//���¹ܽų�ʼ��
	
	LCD_WR_REG(0xED);
  LCD_WR_DATAX(0x01);
	LCD_WR_DATAX(0xFE);

	LCD_WR_REG(0xEE);
	LCD_WR_DATAX(0xDE);
	LCD_WR_DATAX(0x21);

	LCD_WR_REG(0xF1);
	LCD_WR_DATAX(0x01);
	
	LCD_WR_REG(0xDF);
	LCD_WR_DATAX(0x10);

	//VCOMvoltage//
	LCD_WR_REG(0xC4);
	LCD_WR_DATAX(0x8F);	  //5f

	LCD_WR_REG(0xC6);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xE2);
	LCD_WR_DATAX(0xE2);
	LCD_WR_DATAX(0xE2);
	
	LCD_WR_REG(0xBF);
	LCD_WR_DATAX(0xAA);

	LCD_WR_REG(0xB0);
	LCD_WR_DATAX(0x0D);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x0D);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x11);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x19);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x21);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x2D);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x3D);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x5D);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x5D);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xB1);
	LCD_WR_DATAX(0x80);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x8B);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x96);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xB2);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x02);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x03);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xB3);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xB4);
	LCD_WR_DATAX(0x8B);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x96);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xA1);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xB5);
	LCD_WR_DATAX(0x02);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x03);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x04);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xB6);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xB7);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x3F);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x5E);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x64);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x8C);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xAC);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xDC);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x70);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x90);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xEB);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xDC);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xB8);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xBA);
	LCD_WR_DATAX(0x24);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xC1);
	LCD_WR_DATAX(0x20);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x54);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xFF);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xC2);
	LCD_WR_DATAX(0x0A);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x04);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xC3);
	LCD_WR_DATAX(0x3C);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x3A);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x39);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x37);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x3C);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x36);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x32);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x2F);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x2C);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x29);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x26);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x24);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x24);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x23);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x3C);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x36);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x32);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x2F);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x2C);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x29);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x26);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x24);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x24);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x23);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xC4);
	LCD_WR_DATAX(0x62);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x05);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x84);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xF0);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x18);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xA4);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x18);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x50);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x0C);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x17);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x95);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xF3);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xE6);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xC5);
	LCD_WR_DATAX(0x32);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x44);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x65);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x76);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x88);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xC6);
	LCD_WR_DATAX(0x20);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x17);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x01);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xC7);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xC8);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xC9);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xE0);
	LCD_WR_DATAX(0x16);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x1C);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x21);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x36);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x46);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x52);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x64);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x7A);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x8B);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x99);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xA8);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xB9);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xC4);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xCA);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xD2);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xD9);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xE0);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xF3);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xE1);
	LCD_WR_DATAX(0x16);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x1C);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x22);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x36);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x45);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x52);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x64);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x7A);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x8B);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x99);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xA8);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xB9);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xC4);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xCA);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xD2);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xD8);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xE0);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xF3);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xE2);
	LCD_WR_DATAX(0x05);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x0B);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x1B);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x34);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x44);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x4F);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x61);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x79);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x88);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x97);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xA6);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xB7);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xC2);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xC7);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xD1);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xD6);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xDD);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xF3);
	LCD_WR_DATAX(0x00);
	LCD_WR_REG(0xE3);
	LCD_WR_DATAX(0x05);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xA);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x1C);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x33);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x44);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x50);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x62);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x78);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x88);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x97);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xA6);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xB7);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xC2);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xC7);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xD1);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xD5);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xDD);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xF3);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xE4);
	LCD_WR_DATAX(0x01);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x01);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x02);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x2A);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x3C);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x4B);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x5D);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x74);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x84);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x93);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xA2);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xB3);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xBE);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xC4);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xCD);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xD3);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xDD);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xF3);
	LCD_WR_DATAX(0x00);
	
	LCD_WR_REG(0xE5);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x02);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x29);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x3C);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x4B);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x5D);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x74);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x84);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x93);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xA2);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xB3);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xBE);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xC4);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xCD);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xD3);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xDC);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xF3);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xE6);
	LCD_WR_DATAX(0x11);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x34);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x56);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x76);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x77);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x66);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x88);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x99);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xBB);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x99);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x66);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x55);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x55);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x45);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x43);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x44);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xE7);
	LCD_WR_DATAX(0x32);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x55);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x76);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x66);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x67);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x67);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x87);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x99);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xBB);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x99);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x77);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x44);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x56);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x23);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x33);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x45);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xE8);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x99);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x87);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x88);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x77);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x66);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x88);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xAA);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0xBB);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x99);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x66);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x55);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x55);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x44);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x44);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x55);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xE9);
	LCD_WR_DATAX(0xAA);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0x00);
	LCD_WR_DATAX(0xAA);

	LCD_WR_REG(0xCF);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xF0);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x50);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xF3);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0xF9);
	LCD_WR_DATAX(0x06);
	LCD_WR_DATAX(0x10);
	LCD_WR_DATAX(0x29);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0x3A);
	LCD_WR_DATAX(0x55);	//66

	LCD_WR_REG(0x11);
	systick_delay_ms(100);
	LCD_WR_REG(0x29);
	LCD_WR_REG(0x35);
	LCD_WR_DATAX(0x00);

	LCD_WR_REG(0x51);
	LCD_WR_DATAX(0xFF);
	LCD_WR_REG(0x53);
	LCD_WR_DATAX(0x2C);
	LCD_WR_REG(0x55);
	LCD_WR_DATAX(0x82);
	LCD_WR_REG(0x2c);
	
 LCD_Display_Dir(1);         //Ĭ��Ϊ����
 gpio_set(LCD_LED,1);        //��������
}


void lcd_test()
{
	//LCD_Showimage(120,180,15,15,gImage_game,2,1);


}



