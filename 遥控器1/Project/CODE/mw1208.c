#include "zf_gpio.h"
#include "mw1208.h" 
#include "page.h"
#include "common.h"
#include "zf_uart.h"

CHECK check;
char lora_send_data[DATA_SIZE] = {0};//���ͻ�������
uint8 lora_receive_data[DATA_SIZE] = {0};//���ջ�������
uint8 data_point=0;//����ָ��

_RECEIVE_DATA_STA receive_data_sta;//����ʹ�ñ�־

void mw1208_init()//mw1208��ʼ��
{
	gpio_init(pin_mw1208_aux,GPI,0,GPI_PULL_DOWN);//AUX�ɸߵ�ƽתΪ�͵�ƽʱ����ʾģ�鷢���������ꡣ�ɵ͵�ƽתΪ�ߵ�ƽ��ʾģ�鿪ʼ���ͻ����
	gpio_init(pin_mw1208_md0,GPO,0,GPO_PUSH_PULL);//MD0��1��ʾ���ù��ܣ���0���ʾͨ�Ź���
	uart_init(mw1208_uart,mw1208_baud,pin_mw1208_tx,pin_mw1208_rx);//���ڳ�ʼ��
	uart_rx_irq(mw1208_uart,1);//�򿪴��ڽ����ж�
}


void mw1208_set(void)//����mw1208
{
	
	char sendbuf[20];
	
//��������ģʽǰ����ͨ�Ų����ʺ�У��λ(115200 8λ���� 1λֹͣ ������У�飩
	
	while(gpio_get(pin_mw1208_aux));//�ȴ�ģ�����
	gpio_set(pin_mw1208_md0,1);
	systick_delay_ms(40);
	sprintf(sendbuf,"AT+ADDR=37,29\r\n");//�����豸��ַ
	mw1208_send_data(sendbuf);
	gpio_set(pin_mw1208_md0,0);



}













void mw1208_uart_handle()//���ڽ����ж�
{
	check.Flag=0;

	while(!(uart_index[mw1208_uart]->CSR & UART_CSR_RXAVL));
	uint8 res=(uint8)uart_index[mw1208_uart]->RDR;//����һ���ֽ�

	if(res=='#' && receive_data_sta==data_receive_finish)//�����������ͷ������ʹ������
	{
		receive_data_sta=data_receive_start;//���Ϊ��ʼ��������
	}
	if(receive_data_sta==data_receive_start)//������ڽ�����
	{
		*(lora_receive_data+data_point)=res;
		data_point++;
	} 
	if(res=='\n' && receive_data_sta==data_receive_start)//���������ֹ���Ҵ��ڽ�����
	{
		*(lora_receive_data+data_point)=0;
		receive_data_sta=data_recevie_unfinish;//���Ϊ������������&&��ʹ������
		data_point=0;//ָ������
	}
			
	
}




void mw1208_send_data(char *str)//���͡�Ĭ��ʹ��͸������
{
	char *p_send_str=str;
	while(*p_send_str)//�����ַ���
	{
		uart_index[mw1208_uart]->TDR =*p_send_str++;														
		while(!(uart_index[mw1208_uart]->CSR & UART_CSR_TXC));
	}
	check.Flag=1;
	
}








double float_num=0;
long long_num=0;
 


void encode_lora_cmd(CMD_NAME cmd,void *data,DATA_TYPE data_type)
{
	
		
	if(data_type==void_type)
		sprintf(lora_send_data,"#cmd,%c,()\n",cmd);
	else if(data_type==float_type)
		sprintf(lora_send_data,"#cmd,%c,(%f)\n",cmd,*(float*)data);
	else if(data_type==int_type)
		sprintf(lora_send_data,"#cmd,%c,(%d)\n",cmd,*(int*)data);
	else if(data_type== string_type)
		sprintf(lora_send_data,"#cmd,%c,(%s)\n",cmd,(char*)data);
	else if(data_type==unsigned_short_type)
		sprintf(lora_send_data,"#cmd,%c,(%u)\n",cmd,*(u16*)data);
	else if(data_type==unsigned_char_type)
		sprintf(lora_send_data,"#cmd,%c,(%u)\n",cmd,*(u8*)data);
	else if(data_type==unsigned_int_type)
		sprintf(lora_send_data,"#cmd,%c,(%u)\n",cmd,*(u32*)data);
	else if(data_type==double_type)
		sprintf(lora_send_data,"#cmd,%c,(%lf)\n",cmd,*(double*)data);
	else if(data_type==long_long_type)
		sprintf(lora_send_data,"#cmd,%c,(%lld)\n",cmd,*(long long*)data);
	else if(data_type==long_type)
		sprintf(lora_send_data,"#cmd,%c,(%ld)\n",cmd,*(long*)data);
	else if(data_type==short_type)
		sprintf(lora_send_data,"#cmd,%c,(%hd)\n",cmd,*(short*)data);

	memset(lora_send_data+strlen(lora_send_data),'\n',32-strlen(lora_send_data));
	lora_send_data[31]=0;
	mw1208_send_data(lora_send_data);

	//lcd_mbox_set_message(mbox1,lora_send_data);
}


void encode_lora_data(DATA_NAME data_name,void *data,DATA_TYPE data_type) 
{
	if(data_type==void_type)
		sprintf(lora_send_data,"#dat,%c,()\n",data_name);
	else if(data_type==float_type)
		sprintf(lora_send_data,"#dac,%c,(%f)\n",data_name,*(float*)data);
	else if(data_type==int_type)
		sprintf(lora_send_data,"#dac,%c,(%d)\n",data_name,*(int*)data);
	else if(data_type== string_type)
		sprintf(lora_send_data,"#dac,%c,(%s)\n",data_name,(char*)data);
	else if(data_type==unsigned_short_type)
		sprintf(lora_send_data,"#dac,%c,(%u)\n",data_name,*(u16*)data);
	else if(data_type==unsigned_char_type)
		sprintf(lora_send_data,"#dac,%c,(%u)\n",data_name,*(u8*)data);
	else if(data_type==unsigned_int_type)
		sprintf(lora_send_data,"#dac,%c,(%u)\n",data_name,*(u32*)data);
	else if(data_type==double_type)
		sprintf(lora_send_data,"#dac,%c,(%lf)\n",data_name,*(double*)data);
	else if(data_type==long_long_type)
		sprintf(lora_send_data,"#dac,%c,(%lld)\n",data_name,*(long long*)data);
	else if(data_type==long_type)
		sprintf(lora_send_data,"#dac,%c,(%ld)\n",data_name,*(long*)data);
	memset(lora_send_data+strlen(lora_send_data),'\n',32-strlen(lora_send_data));
	lora_send_data[31]=0;
	mw1208_send_data(lora_send_data);
}




void dat_handle(u8 *lora_str)//dat���� 
{
	char str[15];
	decode_data(lora_str,str);
	lcd_mbox_set_message(mbox2,str);
	
}







void decode_lora_str(u8 *lora_str)
{
	if(receive_data_sta==data_recevie_unfinish)
	{

	 if(lora_str[1]=='d'&&lora_str[2]=='a'&&lora_str[3]=='t')
		{
			u8 index;
			index=decode_name((char*)lora_str);
			if(index)
			{
				if(index>read_only)
				{
					decode_str_data((char*)lora_str);
					if(index>=latitude && index<= height)
					{	
						
						if(index==height)
							Gps_page.flag=1;
						lcd_dl_set_data(Gps_page.dl1,index-latitude,&data_buffer);
						
				
					}else
					{
						if(index==yaw)
							Skim_page.update_flag=1;
						index=index-read_only-1;
						lcd_dl_set_data(Skim_page.dl1,index,&data_buffer);
					}
				}else
				dat_handle(lora_str);
			}
		}else if(lora_str[1]=='g'&&lora_str[2]=='p'&& lora_str[3]=='s')
		{
			if(lora_str[4]=='s')//gps_save��Ч
					Gps_page.gps_point.gps_save_valid=1;
			
			else if(lora_str[4]=='d')//gps_delete��Ч
					Gps_page.gps_point.gps_delete_valid=1;
			
			else
			{
				Gps_page.gps_point.gps_point_saved=gps_page_string2int((char*)lora_str+4);
				u8 index=3;
				while(lora_str[index]!='/')
					index++;
				Gps_page.gps_point.gps_point_total=gps_page_string2int((char*)lora_str+index+1);
				
				
				char str[15];
					
				sprintf(str,"saved:%u/%u",Gps_page.gps_point.gps_point_saved,Gps_page.gps_point.gps_point_total);
				
				lcd_label_set_text(Gps_page.label1,str);
			}
			
			
		
		}
		else
		{
			cut_str((char*)lora_str);
			
			lcd_mbox_set_message(mbox2,(char*)lora_str+1);
		}
		receive_data_sta=data_receive_finish;
	}
}



void cut_str(char*string)
{
	u8 index=0;
	while(string[index])
	{
			if(string[index]=='\n')
				string[index]=0;
			index++;
	}
}






void decode_data(u8 *lora_str,char *string)
{
	u8 index_start=0,index_end=0,index=0;
	if(lora_str[6]==',' && lora_str[7]=='(')
		index_start=8;
   
    while(lora_str[index])
		{
			if(lora_str[index]==')' && lora_str[index+1]=='\n')
					index_end=index;
			index++;
		}
	for(index=0;index<index_end-index_start;index++)
		string[index]=lora_str[index+index_start];
	string[index]=0;
	
}



u8 decode_name(char* string)//????
{
    u8 index_start=0;
    u8 index=0;

    while(string[index])
    {
        if(string[index]==',')
        {
            index_start=index;
            break;
        }
        index++;
    }
    if(string[index_start+2]==',')
       return string[index_start+1];
    else
        return 0;
    
}








