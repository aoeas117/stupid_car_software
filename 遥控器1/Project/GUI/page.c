#include "page.h"
#include "touch.h"
#include "myimage.h"


PAGE_FLAG page_flag=send_page;
bg_t *bg;
mbox_t *mbox1;
mbox_t *mbox2;
style_t style1={0x13,0x11,0xf3,0x50,0xff,0xbf,0,0};
style_t style2={0x12,0x1c,0x29,0xc3,0xff,0xbf,10,110};
style_t style3={0x71,0x82,0xfa,0x11,0xf1,0x11,0,0};
style_t style4={0x91,0x64,0x2a,0xf2,0xb1,0xf3,0,0};
style_t style5={0x4a,0x81,0xf7,0xf1,0x11,0x00,0,0};
style_t style6={72,209,204,186,85,211,0,0};
style_t style7={0x76,0xf9,0xdc,255,105,180,0,0};
style_t style8={0xf1,0x56,0x11,0x13,0xfa,0xb1,0,0};
style_t style9={0,191,255,240,230,140,0,0};
style_t style10={0,206,209,148,0,211,0,0};



image_t *image[5];
cb_t *page_cb;


void page_style_init(void)
{
	lcd_create_random_style(&style1);
	lcd_create_random_style(&style2);
	lcd_create_random_style(&style3);
	lcd_create_random_style(&style4);
	lcd_create_random_style(&style5);
	lcd_create_random_style(&style6);
	lcd_create_random_style(&style7);
	lcd_create_random_style(&style8);
	lcd_create_random_style(&style9);
	lcd_create_random_style(&style10);


}






void page_create(void)
{
	page_style_init();
	
	style_t stylet={38,38,38,38,38,38};
	bg=lcd_bg_create(0,0,480,320,stylet);
	
	lcd_style_set_color(&stylet,11,11,11,56,56,56);
	mbox1=lcd_mbox_create(20,stylet,bg->style,8);
	lcd_mbox_enable_display(mbox1);
	
	mbox2=lcd_mbox_create(220,stylet,bg->style,8);
	lcd_mbox_enable_display(mbox2);
	
	page_cb=lcd_cb_create(370,3,"timer",bg->style);
	lcd_cb_enable_display(page_cb);

	image[0]=lcd_image_create(440,0,40,40,bg->style,gImage_SEND);
	image[1]=lcd_image_create(440,64,40,40,bg->style,gImage_DATA);
	image[2]=lcd_image_create(440,128,40,40,bg->style,gImage_GPS);
	image[3]=lcd_image_create(440,192,40,40,bg->style,gImage_ROS);
	image[4]=lcd_image_create(440,256,40,40,bg->style,gImage_SET);
	for(u8 i=0;i<5;i++)
		lcd_image_set_color(image[i],lcd_makecolor(233,233,233),lcd_makecolor(128,128,128));
	
	lcd_image_set_active(image[0],1);
	lcd_image_enable_display(image[1]);
	lcd_image_enable_display(image[2]);
	lcd_image_enable_display(image[3]);
	lcd_image_enable_display(image[4]);
	
	
	sned_page_create();
	rov_page_create();
	skim_page_create();
	gps_page_create();
	set_page_create();
	
	send_page_enable_display();

}



void page_del()
{
	switch(page_flag)
		{
			case send_page:
			{
				sned_page_disable_display();
				break;
			}
			case skim_page:
			{
				skim_page_disable_display();
				break;
			}
			case gps_page:
			{
				gps_page_disable_display();
				break;
			}
			case rov_page:
			{
			
				rov_page_disable_display();
				break;
			}
			case set_page:
			{
				set_page_disable_display();
				break;
			}
		}



}



void page_touch_handler(void)
{
	if(tp_sta.tp_press_turn)
	{
		page_touch_list();
		image_handler();
		lcd_mbox_ack(mbox1);
		lcd_mbox_ack(mbox2);
		u8 act=lcd_cb_ack(page_cb);
		
		if(act==1)
		{
			tim_switch(1);
		}else if(act==0)
		{
			tim_switch(0);
		}
		
		
		
		tp_sta.tp_press_turn=0;
	}
}




void page_touch_list(void)
{
	switch(page_flag)
		{
			case send_page:
			{
				send_page_handler();
				break;
			}
			case skim_page:
			{
				skim_page_handler();
				break;
			}
			case gps_page:
			{
				gps_page_handler();
				break;
			}
			case rov_page:
			{
				rov_page_handler();
				break;
			}
			case set_page:
			{
				set_page_handler();
				break;
			}
		}

}

u8 active_image=0;

void image_handler(void)
{
	u8 index=0;u8 flag_num=5;
	for(index=0;index<5;index++)
		if(tp_is_sect(image[index]->object))
			flag_num=index;
		if(flag_num<5)
		{
			if(flag_num==active_image)
				return;
			else
			{
				page_del();
				lcd_image_set_active(image[flag_num],1);
				lcd_image_set_active(image[active_image],0);
				active_image=flag_num;
				switch(flag_num)
				{
					case send_page:
					{
						send_page_enable_display();
						break;
					}
					
				
					case skim_page:
					{
						skim_page_enable_display();
						break;
					}
				
				
					case gps_page:
					{
						gps_page_enable_display();
						break;
					}
				
				
					case rov_page:
					{
				
						rov_page_enable_display();
						break;
					}
				
				
					case set_page:
					{
						set_page_enable_display();
						break;
					}
				}
			}
		}
	}

void TIM2_IRQHandler (void)
{
	uint32 state = TIM2->SR;														// 读取中断状态
	TIM2->SR &= ~state;																// 清空中断状态
	lcd_mbox_tim_ack(mbox1);
	lcd_mbox_tim_ack(mbox1);
	
	
}







void general_handler(void)//全局事物处理
{

	if(Gps_page.flag==1 && page_flag==gps_page)
	{
		lcd_dl_data_update(Gps_page.dl1,0);
		Gps_page.flag=0;
		gps_get_lalo_data();
	}
	
	if(Gps_page.gps_point.gps_save_valid==1)
	{
		Gps_page.gps_point.gps_save_valid=0;
		Gps_page.gps_point.gps_point_saved++;
		char str[15];
			
		sprintf(str,"saved:%u/%u",Gps_page.gps_point.gps_point_saved,Gps_page.gps_point.gps_point_total);
		
		lcd_label_set_text(Gps_page.label1,str);
	}
	
	if(Gps_page.gps_point.gps_delete_valid==1)
	{
		Gps_page.gps_point.gps_delete_valid=0;
		Gps_page.gps_point.gps_point_saved--;
		char str[15];
			
		sprintf(str,"saved:%u/%u",Gps_page.gps_point.gps_point_saved,Gps_page.gps_point.gps_point_total);
		
		lcd_label_set_text(Gps_page.label1,str);
	}
	if(page_flag==skim_page)
	
		if(Skim_page.update_flag==1)
		{	
			lcd_dl_data_update(Skim_page.dl1,0);
			Skim_page.update_flag=0;
		}
	
		
		
		//查询接收

			
				
			
		
		
		
		
		

}











