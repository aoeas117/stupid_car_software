

#include "headfile.h"
#include "touch.h"
#include "gui.h"
#include "page.h"
#include "spi_tfcard.h"


int main(void)
{
	board_init(true);																// 初始化 debug 输出串口
  mw1208_init();//lora初始化
	ps2_init();//遥杆初始化
  key_init();//按键初始化
	//gpio_init(B11, GPO, 0, GPO_PUSH_PULL);
  LCD_Init();
	//gui_test();
	page_create();
	TP_Init();
	lcd_tim_init();
	//lcd_test();
  //systick_delay_ms(1);
  //gpio_set(B11,1);
	
	systick_start();//系统滴答定时器启动
	
	

	while(1)
	{
		tp_scan();//触摸扫描
		key_scan();//按键扫描
		decode_lora_str(lora_receive_data);//Lora处理
		//gui_handler();
		page_touch_handler();//触摸处理
		general_handler();//全局事物处理
		/*static uint32 a = 0;
		a= systick_getval()-a;
		printf("%lu\r\n",a);*/
	}
}


// **************************** 代码区域 ****************************
