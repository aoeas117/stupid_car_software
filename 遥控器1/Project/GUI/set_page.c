#include "set_page.h"

SET_PAGE Set_page;

char *set_btnml_map[]=
{
	"return","round","\n",
	"scorner","total","\0"
};



void set_page_create(void)
{
	
	style_t stylet={64,64,64,112,112,112};
	//Set_page.cbl1=lcd_cbl_create(80,80,set_cbl_map,style4,bg->style);
	Set_page.btnm1=lcd_btnm_create(180,110,set_btnml_map,stylet,bg->style);
	lcd_style_set_color(&stylet,11,11,11,34,34,34);
	lcd_btnm_set_btn_style(Set_page.btnm1,stylet);
	
	
	

}



void set_page_enable_display(void)
{
	page_flag=set_page;
	
	lcd_btnm_enable_display(Set_page.btnm1);
	




}


void set_page_disable_display(void)
{
	
	lcd_btnm_disable_display(Set_page.btnm1);

}


void set_page_handler(void)
{
	u8 index=lcd_btnm_ack(Set_page.btnm1);
	
	if(index<Set_page.btnm1->object_count)
	{
		char *string=lcd_btnm_get_text(Set_page.btnm1,index);
		lcd_mbox_set_message(mbox1,string);
		encode_lora_cmd(task_switch,&index,unsigned_char_type);
	}
	
}