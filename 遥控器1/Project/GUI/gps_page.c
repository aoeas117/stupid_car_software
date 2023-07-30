#include "gps_page.h"





GPS_PAGE Gps_page;

char *gps_map_dl[]={"latitude","longtitude","height","\0"};

char *gps_btnm_map[]={"0","1","2","back","\n",
										  "3","4","5","del", "\n",
											"6","7","8","9","\n",
											"get","save","delete","\n",
											"        run        ","\0"};

											
double Latitude=0.0,Longitude=0.0;
float HEight=0.0;
float gps_la[COORD_NUM];
float gps_lo[COORD_NUM];											
											
											
									


void gps_page_create(void)
{
	
	Gps_page.flag=0;
	
	//数据列表初始化
	style_t stylet={89,89,89,89,89,89};
	Gps_page.dl1=lcd_dl_create(225,50,gps_map_dl,stylet,bg->style,0);
	lcd_dl_set_page_object_num(Gps_page.dl1,6);
	lcd_dl_set_data_length(Gps_page.dl1,12);
	
	
	lcd_dl_set_data_index(Gps_page.dl1,&Latitude,0,double_type);
	lcd_dl_set_data_index(Gps_page.dl1,&Longitude,1,double_type);
	lcd_dl_set_data_index(Gps_page.dl1,&HEight,2,float_type);

	//坐标图初始化
	lcd_coord_create(&Gps_page.coord,20,56,180,220,&bg->style);
	lcd_coord_set_data(&Gps_page.coord,gps_la,gps_lo,COORD_NUM);
	
	
	//矩阵按钮初始化
	stylet.backcolor.r=64;
	stylet.backcolor.g=64;
	stylet.backcolor.b=64;
	Gps_page.btnm1=lcd_btnm_create(225,150,gps_btnm_map,stylet,bg->style);
	lcd_style_set_color(&stylet,128,128,128,128,128,128);
	lcd_btnm_set_btn_style(Gps_page.btnm1,stylet);
	
	
	
	
	
	//文本框初始化
	Gps_page.ta1=lcd_ta_create(270,125,stylet,"",bg->style);
	lcd_ta_set_max_length(Gps_page.ta1,3);
	
	//按钮初始化
	Gps_page.btn1=lcd_btn_create(55,293,"auto_update",stylet,bg->style);
	
	//标签初始化
	Gps_page.label1=lcd_label_create(320,125,0x0000,"",bg->style);
	lcd_label_set_max_length(Gps_page.label1,12);
	
	Gps_page.label2=lcd_label_create(20,39,0x0000,"north",bg->style);
	lcd_label_set_max_length(Gps_page.label2,15);
	
	Gps_page.label3=lcd_label_create(87,276,0x0000,"east",bg->style);
	lcd_label_set_max_length(Gps_page.label3,15);
	
	
	Gps_page.gps_point.gps_point_total=128;
	Gps_page.gps_point.gps_point_saved=0;
	Gps_page.gps_point.gps_point_index=0;
	Gps_page.gps_point.gps_data_auto_update=0;
	Gps_page.gps_point.gps_save_valid=0;
	Gps_page.gps_point.gps_delete_valid=0;
	Gps_page.gps_point.gps_receive_count=0;
	Gps_page.gps_point.gps_receive_flag=0;
	
	
	
	sprintf(Gps_page.label1->string,"saved:%u/%u",Gps_page.gps_point.gps_point_saved,Gps_page.gps_point.gps_point_total);
}


void gps_page_enable_display(void)
{
	page_flag=gps_page;
	
	lcd_dl_enable_display(Gps_page.dl1);
	
	lcd_dl_data_enable_display(Gps_page.dl1,0);
	
	lcd_btnm_enable_display(Gps_page.btnm1);
	
	lcd_ta_enable_display(Gps_page.ta1);
	
	lcd_label_set_text(Gps_page.label2,"north");
	
	lcd_label_set_text(Gps_page.label3,"east");
	
	lcd_btn_enable_display(Gps_page.btn1);
	
	lcd_coord_enable_display(&Gps_page.coord);
	
	encode_lora_cmd(Gps_get_saved,NULL,NULL);
	


}


void gps_page_disable_display(void)
{

	
	lcd_btnm_disable_display(Gps_page.btnm1);

	lcd_dl_disable_display(Gps_page.dl1);
	
	lcd_ta_disable_display(Gps_page.ta1);

	lcd_label_disable_display(Gps_page.label2);
	
	lcd_label_disable_display(Gps_page.label3);
	
	lcd_btn_disable_display(Gps_page.btn1);

	lcd_label_disable_display(Gps_page.label1);

	lcd_coord_disable_display(&Gps_page.coord);
}

void gps_page_handler(void)
{
	u8 flag_num=lcd_btnm_ack(Gps_page.btnm1);
	char *p_char=lcd_btnm_get_text(Gps_page.btnm1,flag_num);
	
	if(flag_num<Gps_page.btnm1->object_count)
	{
	
		if(p_char[0]<='9' && p_char[0]>='0')
			lcd_ta_add_char(Gps_page.ta1,p_char[0]);
		else if(p_char[0]=='b')
			lcd_ta_del_char(Gps_page.ta1);
		else if(flag_num==7)
			lcd_ta_set_text(Gps_page.ta1,"");
		else if(flag_num>11)
		{
			
			char *string=lcd_ta_get_text(Gps_page.ta1);
			
			u16 number_t=gps_page_string2int(string);
			
			if(Gps_page.gps_point.gps_data_auto_update)
			if(string[0]!='0')
			{
				lcd_mbox_set_message(mbox1,"please close au!");
				return;
			}
			if(number_t>255)
			{	
				lcd_mbox_set_message(mbox1,"distance");
				u8 cmd_f=4;
				encode_lora_cmd(task_switch,&cmd_f,unsigned_char_type);
				return;
			}
			if(flag_num==12)
			{
				encode_lora_cmd(gps_get_point,string,string_type);
			}
			else if(flag_num==13)
			{
				encode_lora_cmd(gps_save_point,string,string_type);
			}
			else if(flag_num==14)
			{
				encode_lora_cmd(gps_delete_point,string,string_type);
			}else if(flag_num==15)
			{
				encode_lora_cmd(goto_point,string,string_type);
			}
			
			
			
	}
		
	
	}
	
	if(lcd_btn_get_response(Gps_page.btn1))
		strat_gps_lalo_get_data();
	
	if(tp_is_sect(Gps_page.coord.object))
		lcd_coord_grid(&Gps_page.coord);
	
	if(tp_is_sect(Gps_page.ta1->object))
	{
			char *string=lcd_ta_get_text(Gps_page.ta1);
			u16 number_t=gps_page_string2int(string);
		if(number_t>COORD_NUM)
			lcd_mbox_set_message(mbox1,"error");
		else
		{
			lcd_coord_draw(&Gps_page.coord,1);
			lcd_coord_single_point(&Gps_page.coord,number_t,lcd_makecolor(0,0,255));
		}
	
	
	}
	
	
}



u16 gps_page_string2int(char *string)
{
	u8 count=strlen(string);
	if(count==0)
		return 1000;//表示字符串为无效值
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


void strat_gps_lalo_get_data(void)
{
	if(!Gps_page.gps_point.gps_receive_flag)
	{
		char *string;
		string=lcd_ta_get_text(Gps_page.ta1);
		u8 index=gps_page_string2int(string);
		if(index>COORD_NUM)
		{
			lcd_mbox_set_message(mbox1,"index error!");
			return;
		}
		lcd_coord_draw(&Gps_page.coord,0);//清空点
		Gps_page.gps_point.gps_receive_count=0;//是否存入当前位置的点
		Gps_page.gps_point.gps_receive_flag=1;//接收数据标志
		encode_lora_cmd(auto_update,&index,unsigned_char_type);
		Gps_page.gps_point.gps_receive_max=index;
		lcd_mbox_set_message(mbox1,"auto on");
		
	}else
	{
		Gps_page.gps_point.gps_receive_flag=0;
		lcd_mbox_set_message(mbox1,"auto off");
	}

}

void gps_get_lalo_data(void)
{

	if(Gps_page.gps_point.gps_receive_flag)
	{
		char string[20];
		sprintf(string,"%u",Gps_page.gps_point.gps_receive_count);
		lcd_ta_set_text(Gps_page.ta1,string);
		gps_la[Gps_page.gps_point.gps_receive_count]=Latitude;
		gps_lo[Gps_page.gps_point.gps_receive_count]=Longitude;
		Gps_page.gps_point.gps_receive_count++;
		
		if(Gps_page.gps_point.gps_receive_count==Gps_page.gps_point.gps_receive_max)
		{
			
			lcd_coord_pram_update(&Gps_page.coord);
			lcd_coord_draw(&Gps_page.coord,1);
			Gps_page.gps_point.gps_receive_flag=0;
			sprintf(string,"");
			lcd_ta_set_text(Gps_page.ta1,string);
			sprintf(string,"north(dy:%.2fm)",Gps_page.coord.dy);
			lcd_label_set_text(Gps_page.label2,string);
			sprintf(string,"(dx:%.2fm)east",Gps_page.coord.dx);
			lcd_label_set_text(Gps_page.label3,string);
			lcd_mbox_set_message(mbox1,"auto off");
			return;
		}

	
	}

}







