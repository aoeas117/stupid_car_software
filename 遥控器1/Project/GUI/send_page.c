#include "send_page.h"
#include "gui.h"




 char*btnm1_map[]=
{
    "1","2","3","back","\n",
    "4","5","6","send","\n",
    "7","8","9","ac","\n",
    "-","0",".",""
};

char *btnm2_map[]=
{
	"    get data     ","\n",
	"speed_gear","motor_p","\n",
	"motor_i","motor_d","\n",
	"target_speed","max_duty","\n",
	"servo_p","servo_d",""
};





SEND_PAGE Send_page;

void sned_page_create(void)
{
	style_t stylet={12,12,12,64,64,64};

	Send_page.ta1=lcd_ta_create(20,70,stylet,"",bg->style);
	
	lcd_style_set_color(&stylet,64,64,64,89,89,89);
	Send_page.btnm1=lcd_btnm_create(0,190,btnm1_map,stylet,bg->style);
	
	Send_page.btnm2=lcd_btnm_create(210,120,btnm2_map,stylet,bg->style);
	lcd_btnm_set_btn_style(Send_page.btnm2,style10);

	
	lcd_btnm_set_btn_style(Send_page.btnm1,stylet);
	lcd_btnm_set_btn_style(Send_page.btnm2,stylet);
	
	lcd_style_set_color(&stylet,69,69,69,128,128,128);
	lcd_btnm_set_btn_style(Send_page.btnm1,stylet);
	lcd_btnm_set_btn_style(Send_page.btnm2,stylet);
	

	
//	lcd_btn_enable_display(Send_page.btn1);
}


void send_page_enable_display(void)
{
	page_flag=send_page;
	lcd_ta_enable_display_animate(Send_page.ta1);
	lcd_btnm_enable_display(Send_page.btnm1);
	lcd_btnm_enable_display(Send_page.btnm2);
	
	
};


void sned_page_disable_display(void)
{
	lcd_ta_disable_display(Send_page.ta1);
	lcd_btnm_disable_display(Send_page.btnm1);
	lcd_btnm_disable_display(Send_page.btnm2);

};





u8 send_page_str_judge(char *str)//判断数据格式是否正确
{
	u8 char_count=strlen(str);
	u8 index;
	u8 point_count=0;
	if(char_count==0) return 0;
	if(str[0]=='.') return 0;
	else if(str[0]=='0')
	{
		if(char_count==1) 
			return 1;
		
		if(str[1]!='.')
			return 0;
		else 
		{
			if(char_count==2) return 0;
			for(index=2;index<char_count;index++)
				if(str[index]=='.' || str[index]=='-')
					return 0;
			return 1;
		}
	}else if(str[0]=='-')
	{
		if(str[1]==0) return 0;
		if(str[1]=='0')
		{
			if(str[2]!='.')
				return 0;
			else
			{
				for(index=3;index<char_count;index++)
					if(str[index]=='.' || str[index]=='-' )
						return 0;
					return 1;
			}
		}else if(str[1]=='.' || str[1]=='-')
			return 0;
		else 
		{
			point_count=0;
			for(index=2;index<char_count;index++)
			{
				if(str[index]=='-') return 0;
				if(str[index]=='.') point_count++;
				if(point_count==2) return 0;
			}
			if(str[char_count-1]=='.') return 0;
			return 1;
		}
	}else
	{
		point_count=0;
		for(index=1;index<char_count;index++)
		{
			if(str[index]=='-') return 0;
			if(str[index]=='.') point_count++;
			if(point_count==2) return 0;
		}
		if(str[char_count-1]=='.') return 0;
		return 1;
	}
	
	return 0;
}












void send_page_handler(void)
{
	
		send_page_btnm1_handler();
		Send_page_btnm2_handler();

}

u8 data_list_index;

void send_page_btnm1_handler(void)
{
		u8 flag_num=lcd_btnm_ack(Send_page.btnm1);
		char *string=lcd_btnm_get_text(Send_page.btnm1,flag_num);
	if(flag_num<Send_page.btnm1->object_count)
	{
		if((string[0]-'0'>=0 && string[0]-'0'<=9) || string[0]=='-' || string[0]=='.')
			lcd_ta_add_char(Send_page.ta1,string[0]);
		if(flag_num==3)
			lcd_ta_del_char(Send_page.ta1);
		if(flag_num==7)
		{
			string=lcd_ta_get_text(Send_page.ta1);
			if(send_page_str_judge(string))
			{
				
				if(data_list_index==0)
				{
					lcd_mbox_display_message(mbox1,"index error!",1);
					return;
				}else if(data_list_index==motor_pwm)
				{
					max_gear=send_page_string2int(string);
					if(max_gear>3500)
						max_gear=3500;
				}		
				encode_lora_data(data_list_index,string,string_type);
				//lcd_mbox_display_message(mbox1,"send succeed!",1);
			}
			else
				lcd_mbox_display_message(mbox1,"format error!",1);
			lcd_ta_set_text(Send_page.ta1,"");
		}
		if(flag_num==11)
		{
			lcd_ta_set_text(Send_page.ta1,"");
			lcd_mbox_display_message(mbox1,"del ok",1);
		}
	}
}



void Send_page_btnm2_handler(void)
{
	u8 flag_num=lcd_btnm_ack(Send_page.btnm2);
	char *string=lcd_btnm_get_text(Send_page.btnm2,flag_num);
	if(flag_num<Send_page.btnm2->object_count)
	{
		if(flag_num>0 && flag_num<9)
		{	
			data_list_index=flag_num+32;
			lcd_mbox_set_message(mbox1,string);
		}else if(flag_num==0)
		{
				if(data_list_index==0)
				{
					lcd_mbox_display_message(mbox1,"index error!",1);
					return;
				}else
				{
					encode_lora_data(data_list_index,NULL,NULL);
					
				}
		}
	}
}





u16 send_page_string2int(char *string)
{
	u8 count=strlen(string);
	if(count==0)
		return 0;//表示字符串为无效值
	else
	{
		u8 index;u16 return_num=0;
		for(index=0;index<count&&(string[index]>='0' && string[index]<='9');index++)
		{
			return_num*=10;
			return_num+=string[index]-'0';
		}
	
		return return_num;
	}

}




