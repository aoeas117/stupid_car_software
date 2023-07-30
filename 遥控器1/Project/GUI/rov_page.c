#include "rov_page.h"
#include "page.h"
#include "yao_gan.h"

ROV_PAGE Rov_page;

char *btnm3_map[]=
{
	"600",
	"900",
	"1200",
	"gear",""
};


char *cbl1_map[]=
{
	"speed_scan","angle_scan","speed_indicator","angle_indicator",""
};


void rov_page_create(void)
{
	style_t stylet={55,55,55,89,89,89};
	Rov_page.bar1=lcd_bar_create(390,170,bg->style,style5);
	lcd_bar_set_length(Rov_page.bar1,100);
	
	Rov_page.lmeter1=lcd_lmeter_create(265,270,100,style10,bg->style);
	
	Rov_page.cbl1=lcd_cbl_create(10,190,cbl1_map,stylet,bg->style);
	lcd_style_set_color (&stylet,100,100,100,100,100,100);
	Rov_page.btn1=lcd_btn_create(40,90,"",stylet,bg->style);
	lcd_label_set_max_length(Rov_page.btn1->label,10);
	sprintf(Rov_page.btn1->label->string,"switch_dir");
	lcd_btn_set_w(Rov_page.btn1,20);
	lcd_btn_set_ack_style(Rov_page.btn1,style7);
	
	Rov_page.btn2=lcd_btn_create(36,130,"",stylet,bg->style);
	lcd_label_set_max_length(Rov_page.btn2->label,11);
	sprintf(Rov_page.btn2->label->string,"switch_mode");
	lcd_btn_set_w(Rov_page.btn2,20);
	lcd_btn_set_ack_style(Rov_page.btn2,style7);
	
	
	
	Rov_page.btnm1=lcd_btnm_create(178,50,btnm3_map,stylet,bg->style);
	lcd_btnm_set_trigger(Rov_page.btnm1,1);
	lcd_btnm_set_one_btn(Rov_page.btnm1,1);
	Rov_page.btnm1->mode.active_object=0;

	

}



void rov_page_enable_display(void)
{
	page_flag=rov_page;
	
	lcd_bar_enable_display(Rov_page.bar1);

	lcd_btn_enable_display(Rov_page.btn1);
	lcd_btn_enable_display(Rov_page.btn2);
	
	lcd_lmeter_enable_display(Rov_page.lmeter1);
	
	lcd_cbl_enable_display(Rov_page.cbl1);
	lcd_cbl_sta_save(Rov_page.cbl1);
	
	lcd_btnm_enable_display(Rov_page.btnm1);
	lcd_btnm_sta_save(Rov_page.btnm1);
	
	tim_switch(1);
	lcd_cb_set_sta(page_cb,1);
	
}

void rov_page_disable_display(void)
{
	tim_switch(0);
	lcd_cb_set_sta(page_cb,0);
	lcd_bar_disable_display(Rov_page.bar1);
	lcd_lmeter_disable_display(Rov_page.lmeter1);
	lcd_btnm_disable_display(Rov_page.btnm1);
	lcd_cbl_disable_display(Rov_page.cbl1);
	lcd_btn_disable_display(Rov_page.btn1);
	lcd_btn_disable_display(Rov_page.btn2);
	

}






uint16 max_gear=1500;

void rov_page_handler(void)
{
	
	u8 flag_num=lcd_btnm_ack(Rov_page.btnm1);
	if(flag_num<Rov_page.btnm1->object_count)
	{
		
		if(lcd_btnm_get_btn_trigger_sta(Rov_page.btnm1,flag_num))
		{
			if(flag_num<=2)
				speed_gear=(flag_num+1)*300+300;
			else if(flag_num==3)
				speed_gear=max_gear;
		}
		else
		speed_gear=0;
		
		char str[20];
		sprintf(str,"speed_gear:%u",speed_gear);
		lcd_mbox_set_message(mbox1,str);
	}
	
	
	flag_num=lcd_cbl_ack(Rov_page.cbl1);
	if((flag_num&0x7f)<Rov_page.cbl1->object_count)
	{
		if((flag_num&0x7f)==0)
			c_rov.speed_scan=(flag_num&0x80)>>7;
		else if((flag_num&0x7f)==1)
			c_rov.angle_scan=(flag_num&0x80)>>7;
		else if((flag_num&0x7f)==2)
			c_rov.speed_indicator=(flag_num&0x80)>>7;
		else if((flag_num&0x7f)==3)
			c_rov.angle_indicator=(flag_num&0x80)>>7;
	}
	
	
	if(lcd_btn_get_response(Rov_page.btn1))
			encode_lora_cmd(switch_dir,NULL,NULL);
	
if(lcd_btn_get_response(Rov_page.btn2))
			encode_lora_cmd(switch_control_mode,NULL,NULL);
	

}








