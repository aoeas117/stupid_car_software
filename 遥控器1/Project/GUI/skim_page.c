#include "skim_page.h"



SKIM_PAGE Skim_page;

DATA_T data_buffer;

char *map_dl[]={"gyro_x","gyro_y","gyro_z","acc_x","acc_y","acc_z","pitch","roll","yaw","\0"};
char *map_btnm1[]={"clear","\n","display","\n","update","\0"};
char *map_cbl5[]={"imu_solution","ekf","\0"};

float Gyro_x=0.0,Gyro_y=0.0,Gyro_z=0.0;
float Acc_x=0.0,Acc_y=0.0,Acc_z=0.0;
float Pitch=0.0,Roll=0.0,Yaw=0.0;








void skim_page_create(void)
{
	style_t stylet={64,64,64,64,64,64};
	Skim_page.dl1=lcd_dl_create(40,80,map_dl,stylet,bg->style,0);
	lcd_dl_set_page_object_num(Skim_page.dl1,8);//设置单页数据数
	lcd_dl_set_data_length(Skim_page.dl1,12);//设置数据长度
	
	lcd_style_set_color(&stylet,89,89,89,89,89,89);
	Skim_page.btnm1=lcd_btnm_create(260,80,map_btnm1,stylet,bg->style);
	
	lcd_style_set_color(&stylet,128,128,128,128,128,128);
	lcd_btnm_set_btn_style(Skim_page.btnm1,stylet);
	
	
	Skim_page.cbl1=lcd_cbl_create(260,200,map_cbl5,stylet,bg->style);
	
	
	
	/**设置数据索引**/
	lcd_dl_set_data_index(Skim_page.dl1,&Gyro_x,0,float_type);
	lcd_dl_set_data_index(Skim_page.dl1,&Gyro_y,1,float_type);
	lcd_dl_set_data_index(Skim_page.dl1,&Gyro_z,2,float_type);
	lcd_dl_set_data_index(Skim_page.dl1,&Acc_x,3,float_type);
	lcd_dl_set_data_index(Skim_page.dl1,&Acc_y,4,float_type);
	lcd_dl_set_data_index(Skim_page.dl1,&Acc_z,5,float_type);
	lcd_dl_set_data_index(Skim_page.dl1,&Pitch,6,float_type);
	lcd_dl_set_data_index(Skim_page.dl1,&Roll,7,float_type);
	lcd_dl_set_data_index(Skim_page.dl1,&Yaw,8,float_type);
	
	/***********/

}




void skim_page_enable_display(void)
{
	
	page_flag=skim_page;
	lcd_dl_enable_display(Skim_page.dl1);
	lcd_dl_data_enable_display(Skim_page.dl1,0);
	lcd_btnm_enable_display(Skim_page.btnm1);
	lcd_cbl_enable_display(Skim_page.cbl1);
	lcd_cbl_sta_save(Skim_page.cbl1);
	
		
}



void skim_page_disable_display(void)
{

	lcd_dl_disable_display(Skim_page.dl1);
	lcd_btnm_disable_display(Skim_page.btnm1);
	lcd_cbl_disable_display(Skim_page.cbl1);

}


void skim_page_handler(void)
{
	lcd_dl_lbtn_action(Skim_page.dl1);
	
	
	u8 flag_num=lcd_btnm_ack(Skim_page.btnm1);
	if(flag_num==0)
		lcd_dl_data_disable_display(Skim_page.dl1,0);
	else if(flag_num==1)
		lcd_dl_data_enable_display(Skim_page.dl1,0);
	else if(flag_num==2)
		skim_page_get_data();
	
	
	flag_num=lcd_cbl_ack(Skim_page.cbl1);
	if(flag_num!=Skim_page.cbl1->object_count)
	{
		uint8 flag=(flag_num&0x80)>>7;
		flag_num&=0x7f;
		if(flag_num==0)
			encode_lora_cmd(imu_solve,&flag,unsigned_char_type);
		else
			encode_lora_cmd(ekf,&flag,unsigned_char_type);
		
	
	
	}
	
}




void skim_page_get_data()
{
	u8 enable=2;
	encode_lora_cmd(enable_auto_send,&enable,unsigned_char_type);
}













uint8 decode_str_data(char *string)//????
{
    uint8 index_start=0,index_end=0,pn=0;
    uint8 index=0;
    uint8 index_point=0;
		data_buffer.data_float=0;
    data_buffer.data_int=0.0;
    int int_part=0;float float_part=0;

    if(string[6]==',' && string[7]=='(')
        index_start=7;
    else
        return 0;


    while(string[index])
    {
       if(string[index]==')' && string[index+1]=='\n')
           index_end=index;
        index++;
    }
    if(index_end==0)
        return 0;
    for(index=index_start+1;index<index_end;index++)
        if(string[index]=='.')
            index_point=index;
    if(string[index_start+1]=='-')
    {
        pn=1;
        index_start+=1;
    }

    if(index_point==0)
    {
        for(index=index_start+1;index<index_end;index++)
        {
            data_buffer.data_int*=10;
            data_buffer.data_int+=string[index]-'0';
        }

    }
    else
    {
        for(index=index_start+1;index<index_point;index++)
        {
            int_part*=10;
            int_part+=string[index]-'0';
        }
        for(index=index_point+1;index<index_end;index++)
        {
            float_part*=10;
            float_part+=string[index]-'0';
        }
        data_buffer.data_float=int_part+pow(0.1,(index_end-index_point-1))*float_part;
    }
    if(pn)
    {
        data_buffer.data_int=-data_buffer.data_int;
        data_buffer.data_float=-data_buffer.data_float;
    }
    return 1;
}





