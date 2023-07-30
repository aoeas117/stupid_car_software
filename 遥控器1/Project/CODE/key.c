#include "key.h"
#include "zf_gpio.h"
#include "zf_systick.h"
#include "mw1208.h"

void key_init()
{
	gpio_init(KEY3, GPI, GPIO_HIGH, GPI_PULL_UP);
	gpio_init(KEY6, GPI, GPIO_HIGH, GPI_PULL_UP);
}





u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	uint8 key3,key6;
	if(mode)key_up=1;  //֧������,���ų��ض�����
	key3=gpio_get(KEY3);
	key6=gpio_get(KEY6);
	
	if(key_up&&(key3==0||key6==0))
	{
		systick_delay_ms(20);//ȥ���� 
		key_up=0;
    if(gpio_get(KEY3)==0&&gpio_get(KEY6)==0)return 12;//˫�����
		else if(gpio_get(KEY3)==0)return 3;
		else if(gpio_get(KEY6)==0)return 6;		
	}
		else if(key3==1&&key6==1)key_up=1;
	
	return 0;// �ް�������
}


void key_function(uint8 function_number)
{
	switch(function_number)
	{
		case 1:{
			mw1208_send_data("cmd1");
			break;}
		case 2:{
			mw1208_send_data("cmd2");
			break;}
		case 3:{
			mw1208_send_data("cmd3");
			break;}
		case 6:{
			mw1208_send_data("cmd6");
			break;}
                case 7:{
			mw1208_send_data("cmd7");
			break;}
		case 9:{
			gpio_toggle(pin_mw1208_md0);//����ģʽ
			break;}
                case 12:{
                         mw1208_send_data("cmd7");
                         break;
                         }
	}
}
