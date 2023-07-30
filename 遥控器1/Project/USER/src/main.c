

#include "headfile.h"
#include "touch.h"
#include "gui.h"
#include "page.h"
#include "spi_tfcard.h"


int main(void)
{
	board_init(true);																// ��ʼ�� debug �������
  mw1208_init();//lora��ʼ��
	ps2_init();//ң�˳�ʼ��
  key_init();//������ʼ��
	//gpio_init(B11, GPO, 0, GPO_PUSH_PULL);
  LCD_Init();
	//gui_test();
	page_create();
	TP_Init();
	lcd_tim_init();
	//lcd_test();
  //systick_delay_ms(1);
  //gpio_set(B11,1);
	
	systick_start();//ϵͳ�δ�ʱ������
	
	

	while(1)
	{
		tp_scan();//����ɨ��
		key_scan();//����ɨ��
		decode_lora_str(lora_receive_data);//Lora����
		//gui_handler();
		page_touch_handler();//��������
		general_handler();//ȫ�����ﴦ��
		/*static uint32 a = 0;
		a= systick_getval()-a;
		printf("%lu\r\n",a);*/
	}
}


// **************************** �������� ****************************
