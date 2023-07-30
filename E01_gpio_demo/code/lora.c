/*
 * lora.c
 *
 *  Created on: 2023��1��13��
 *      Author: ���¹�
 */

#include "lora.h"
#include "stdio.h"
#include "string.h"
#include "servo.h"
#include "imu.h"




LORA_Administrators lora;
uint8 lora_send_buffer[LORA_SEND_BUFFER_SIZE];
uint8 lora_receive_buffer[LORA_RECEIVE_BUFFER_SIZE];


int16 motor_pwm_up=200;


void lora_init(void)//lora��ʼ��
{
    gpio_init(PIN_LORA_AUX,GPI,0,GPI_PULL_DOWN);//AUX�ɸߵ�ƽתΪ�͵�ƽʱ����ʾģ�鷢����ɻ������ɡ��ɵ͵�ƽתΪ�ߵ�ƽ��ʾģ�鿪ʼ���ͻ���ա�
    gpio_init(PIN_LORA_MD0,GPO,0,GPO_PUSH_PULL);//MD0��1��ʾ����ģʽ����0���ʾͨ��ģʽ��
    uart_init(LORA_UART,LORA_BAUD,PIN_LORA_TX,PIN_LORA_RX);//���ڳ�ʼ��
    lora_administrators_init(&lora);//lora����ṹ���ʼ��
    uart_rx_interrupt(LORA_UART,1);//�򿪴��ڽ����ж�
}









void lora_administrators_init(LORA_Administrators *LA)
{
    LA->lora_send_buffer=lora_send_buffer;
    LA->lora_receive_buffer=lora_receive_buffer;
    LA->decode.receive_name=0;
    LA->decode.receive_data_float=0.0;
    LA->decode.receive_data_int=0;
    LA->sta.lora_work_sta=1;
    LA->sta.lora_receive_sta=data_receive_used;

    for(uint8 index=0;index<TOTAL_INTERNAL_DATA_SIZE;index++)//����������ʼ��
        LA->data_index[index].data_pointer=NULL;

    /*��������ָ��*/

    //1:�ɱ����
    lora_set_data_index(&lora,motor_pwm,&motor_pwm_up,short_type);
    lora_set_data_index(&lora,motor_p,&motor->pid.p,float_type);
    lora_set_data_index(&lora,motor_i,&motor->pid.i,float_type);
    lora_set_data_index(&lora,motor_d,&motor->pid.d,float_type);
    lora_set_data_index(&lora,target_speed,&motor->motor_speed.target_speed,unsigned_short_type);
    lora_set_data_index(&lora,max_duty,&motor->motor_speed.max_duty,unsigned_short_type);
    lora_set_data_index(&lora,servo_p,&servo.pid.p,float_type);
    lora_set_data_index(&lora,servo_d,&servo.pid.d,float_type);
    lora_set_data_index(&lora,ramp_speed,&speed_pram.ramp_speed,unsigned_short_type);
    lora_set_data_index(&lora,straight_speed,&speed_pram.straight_speed,unsigned_short_type);
    lora_set_data_index(&lora,loop_speed,&speed_pram.loop_speed,unsigned_short_type);
    lora_set_data_index(&lora,cone_num,&speed_pram.cone_num,unsigned_char_type);
    //2:ֻ��
    /*******imu_data*****/
    lora_set_data_index(&lora,gyro_X,&imu_data.gyro_x,double_type);
    lora_set_data_index(&lora,gyro_Y,&imu_data.gyro_y,double_type);
    lora_set_data_index(&lora,gyro_Z,&imu_data.gyro_z,double_type);
    lora_set_data_index(&lora,acc_X,&imu_data.acc_x,double_type);
    lora_set_data_index(&lora,acc_Y,&imu_data.acc_y,double_type);
    lora_set_data_index(&lora,acc_Z,&imu_data.acc_z,double_type);
    lora_set_data_index(&lora,latitude,&gps.gps_data.latitude,double_type);
    lora_set_data_index(&lora,longitude,&gps.gps_data.longitude,double_type);
    lora_set_data_index(&lora,height,&gps.gps_data.height,float_type);
    /***********/

    LA->auto_send.auto_enable_flag=0;
    LA->auto_send.index=read_only+1;



}



void lora_set_data_index(LORA_Administrators *LA,DATA_NAME data_n,void *p,DATA_TYPE data_t)//��������ָ������
{
    if(data_n-1-void_data>TOTAL_INTERNAL_DATA_SIZE) return;
    if(data_n>read_only)
    {
        LA->data_index[data_n-2-void_data].data_pointer=p;
        LA->data_index[data_n-2-void_data].data_type=data_t;

    }else if(data_n<read_only)
    {
        LA->data_index[data_n-1-void_data].data_pointer=p;
        LA->data_index[data_n-1-void_data].data_type=data_t;
    }

}


void lora_receive_data(LORA_Administrators *LA)//��������
{

    uint8 res=uart_read_byte(LORA_UART);//����һ���ֽ�

    if(res=='#' && LA->sta.lora_receive_sta==data_receive_used)//�����������ͷ������ʹ����ϻ��ڽ���״̬�С�Ϊ��ֹ״̬����ʧ�ܡ�
    {
        LA->sta.lora_receive_sta=data_receiving;//���״̬Ϊ������
        LA->lora_receive_buffer_index=0;//������������
    }


    if(LA->sta.lora_receive_sta==data_receiving)//������鴦�ڽ���״̬��
    {
        *(LA->lora_receive_buffer+LA->lora_receive_buffer_index)=res;
        LA->lora_receive_buffer_index++;
    }

    if(res=='\n' &&  LA->sta.lora_receive_sta==data_receiving)//�ڽ��չ�����������β
    {
        LA->sta.lora_receive_sta=data_recevie_unuse;//�������Ϊδʹ��
        *(LA->lora_receive_buffer+LA->lora_receive_buffer_index)=0;

    }

    if(LA->lora_receive_buffer_index>LORA_RECEIVE_BUFFER_SIZE)//�������������С:������һ�ε�ͷ��������ͷβ����ʧ�����»�����������������š�
    {
        LA->sta.lora_receive_sta=data_receive_used;
    }

}


void lora_send_data(LORA_Administrators *LA)//��������
{
    size_t count=strlen((char*)LA->lora_send_buffer);
    uart_write_buffer(LORA_UART,LA->lora_send_buffer,count);

}



void lora_send_str(LORA_Administrators *LA,char* str)
{
    sprintf((char*)lora_send_buffer,"#%s\n",str);
    lora_send_data(LA);
}

void lora_send_ta(LORA_Administrators *LA,char* str,uint8 index)
{
    if(index>3 || index==0)return;
    sprintf((char*)lora_send_buffer,"#ta%u%s\n",index,str);
    lora_send_data(LA);

}





uint8 decode_name(LORA_Administrators *LA)//���ƽ���
{
    uint8 index_start=0;
    uint8 index=0;

    while(LA->lora_receive_buffer[index])
    {
        if(LA->lora_receive_buffer[index]==',')
        {
            index_start=index;
            break;
        }
        index++;
    }
    if(LA->lora_receive_buffer[index_start+2]==',')
    {
        LA->decode.receive_name=LA->lora_receive_buffer[index_start+1];
        return 1;
    }
    else
    {
        LA->decode.receive_name=0;
        return 0;
    }
}



uint8 decode_data(LORA_Administrators *LA)//���ݽ���
{
    uint8 index_start=0,index_end=0,pn=0;
    uint8 index=0;
    uint8 index_point=0;
    LA->decode.receive_data_int=0;
    LA->decode.receive_data_float=0.0;
    int int_part=0;float float_part=0;

    if(LA->lora_receive_buffer[6]==',' && LA->lora_receive_buffer[7]=='(')
        index_start=7;
    else
        return 0;


    while(LA->lora_receive_buffer[index])
    {
       if(LA->lora_receive_buffer[index]==')' && LA->lora_receive_buffer[index+1]=='\n')
           index_end=index;
        index++;
    }
    if(index_end==0 || index_end==index_start+1)
        return 0;



    for(index=index_start+1;index<index_end;index++)
        if(LA->lora_receive_buffer[index]=='.')
            index_point=index;
    if(LA->lora_receive_buffer[index_start+1]=='-')
    {
        pn=1;
        index_start+=1;
    }
    if(index_point==0)
    {
        for(index=index_start+1;index<index_end;index++)
        {
            LA->decode.receive_data_int*=10;
            LA->decode.receive_data_int+=LA->lora_receive_buffer[index]-'0';
        }

    }
    else
    {
        for(index=index_start+1;index<index_point;index++)
        {
            int_part*=10;
            int_part+=LA->lora_receive_buffer[index]-'0';
        }
        for(index=index_point+1;index<index_end;index++)
        {
            float_part*=10;
            float_part+=LA->lora_receive_buffer[index]-'0';
        }
        LA->decode.receive_data_float=int_part+mpow(0.1,(index_end-index_point-1))*float_part;
    }
    if(pn)
    {
        LA->decode.receive_data_int=-LA->decode.receive_data_int;
        LA->decode.receive_data_float=-LA->decode.receive_data_float;
    }
    return 1;
}



void cmd_handle(LORA_Administrators *LA)//�����
{
    if( decode_name(LA))
    {
        switch(LA->decode.receive_name)
        {
            case setspeed:
            {
                if(motor->motor_speed.motor_brake==0)//��ɲ��
                    if(motor->control_mode==1)//����
                    {
                        if(decode_data(LA))
                            motor_set_pwm_from_out(motor,(short)LA->decode.receive_data_int);
                    }else
                    {
                        if(decode_data(LA))
                            motor->motor_speed.target_speed=(short)LA->decode.receive_data_int;

                    }
                break;
            }
            case setangle:

            {
                if(decode_data(LA))
                    if(control.imu_solution_flag==0)
                        servo_set_pwm((unsigned short)LA->decode.receive_data_int);
                break;
            }

            case enable_auto_send:
            {
                if(decode_data(LA))
                    lora_enable_auto_send(LA,(uint8)LA->decode.receive_data_int);
                break;
            }
            case motor_brake:
            {
                if(decode_data(LA))
                {
                    motor_set_stop(motor,(uint8)LA->decode.receive_data_int);

                    if(motor->motor_speed.motor_brake==1)
                        lora_send_str(&lora,"brake(user)");//�û�ɲ��
                    else
                        lora_send_str(&lora,"brake_clear");
                }
                break;
            }

            case switch_dir:
            {
                motor_open_loop_switch_dir(motor);
                break;
            }
            case switch_control_mode:
            {
                motor_switch_control_mode(motor);
                break;
            }

            case Gps_read_point:
            {
                if(decode_data(LA))
                    gps_read_point(&gps,(uint8)LA->decode.receive_data_int);
                break;
            }

            case Gps_save_point:
            {
                if(decode_data(LA))
                    gps_save_point(&gps,(uint8)LA->decode.receive_data_int);

                break;
            }
            case Gps_delete_point:
            {
                if(decode_data(LA))
                    gps_delete_point(&gps,(uint8)LA->decode.receive_data_int);
                break;
            }
            case Gps_get_saved :
            {
                gps_send_saved_point(&gps);
                break;
            }
            case task_switch:
            {
               if(decode_data(LA))
               {
                   uint8 index=(uint8)LA->decode.receive_data_int;
                   task_cmd_list(index);


               }
                break;
            }

            case goto_point:
            {
                if(decode_data(LA))
                {
                    start_search_point(&control,(uint8)LA->decode.receive_data_int);
                }
                break;
            }

            case auto_update:
            {
                if(decode_data(LA))
                {
                    uint8 index=(uint8)LA->decode.receive_data_int;
                    for(uint8 i=0;i<index;i++)
                        gps_read_point(&gps,i);

                }

                break;
            }


        }
    }
}



void task_cmd_list(uint8 index)
{
    switch(index)
    {
        case 0:
        {

            control.turn_round_task.turn_round_flag=1;
            lora_send_str(&lora,"task0");
            break;
        }
        case 1:
        {

            control.loop_task.loop_flag=1;
            lora_send_str(&lora,"task1");
            break;
        }
        case 2:
        {
            control.sconcer_task.sconcer_flag=1;
            lora_send_str(&lora,"task2");
            break;
        }
        case 3:
        {
            control.search_point_flag=1;
            control.target_point=0;
            control.whole_test=1;
            lora_send_str(&lora,"task3");
            break;
        }
        case 4:
        {
            gps_asistant_flag=0;
            lora_send_str(&lora,"flag_clear");
            break;
        }
        case 5:
        {
            char string[12];
            sprintf(string,"%f",gps_data.speed);
            lora_send_str(&lora,string);
            break;
        }

        default:lora_send_str(&lora,"error");



    }




}





void dat_handle(LORA_Administrators *LA)//���ݷ��ʹ���
{
    if(decode_name(LA))
    {
        encode_lora_str(LA);
    }

}

void dac_handle(LORA_Administrators *LA)//���ݸı䴦��
{
   if(motor->motor_speed.motor_brake==0)
   {
     lora_send_str(&lora,"please brake");
     return;
   };
   if(decode_name(LA))
   {
       if(decode_data(LA))
       {
           if(LA->data_index[LA->decode.receive_name-1-void_data].data_pointer!=NULL)//�����ɳ�ʼ��
            {
               uint8 index=LA->decode.receive_name-1-void_data;//ö��ת��������


               void *data_p=LA->data_index[index].data_pointer;
               if(LA->data_index[index].data_type==int_type)
                   *(int *)data_p=(int)(LA->decode.receive_data_float+LA->decode.receive_data_int);
               else if(LA->data_index[index].data_type==float_type)
                   *(float *)data_p=(float)(LA->decode.receive_data_float+LA->decode.receive_data_int);
               else if(LA->data_index[index].data_type==unsigned_char_type)
                   *(unsigned char*)data_p=(unsigned char)LA->decode.receive_data_int;
               else if(LA->data_index[index].data_type==unsigned_short_type)
                   *(unsigned short*)data_p=(unsigned short)LA->decode.receive_data_int;
               else if(LA->data_index[index].data_type==unsigned_int_type)
                   *(unsigned int*)data_p=(unsigned int)LA->decode.receive_data_int;
               else if(LA->data_index[index].data_type==int_type)
                   *(int*)data_p=(int)(LA->decode.receive_data_float+LA->decode.receive_data_int);
               else if(LA->data_index[index].data_type==double_type)
                   *(float*)data_p=(float)(LA->decode.receive_data_float+LA->decode.receive_data_int);
               else if(LA->data_index[index].data_type==short_type)
                   *(short*)data_p=(short)(LA->decode.receive_data_float+LA->decode.receive_data_int);

               write_pram_data_flash(LA);

               lora_send_str(LA,"ok!");//�������ݳɹ���ң�ط�����Ӧ

             }else
                 lora_send_str(LA,"uninit!");//ʧ��ָ��������δ��ʼ��


       }
   }
}









void encode_lora_str(LORA_Administrators *LA)//���ݴ��ͱ���   (Ϊ�˱��ڹ���������ö��������һö�����ָread_only,����������������������)
{
    if(LA->data_index[LA->decode.receive_name-1-void_data].data_pointer!=NULL)
    {
        lora_encode_send_data(LA,LA->decode.receive_name);

    }else
    {
        sprintf((char*)lora_send_buffer,"#uninit!\n");
        lora_send_data(LA);
    }



}



void lora_auto_send(LORA_Administrators *LA)//ֻ���read_only(�ŵ���ʱ���ж���)
{

    if(LA->auto_send.auto_enable_flag)//ʹ�ܸù���
    {

        if(LA->auto_send.index<=read_only ||LA->auto_send.index>=end_data)
        {
            lora_send_str(LA,"initerror!");
            LA->auto_send.auto_enable_flag=0;//�رոù���
        }else
        {
            if(LA->data_index[LA->auto_send.index-2-void_data].data_pointer==NULL)
            {
                //lora_send_str(LA,"uninit!");
                //LA->auto_send.auto_enable_flag=0;//�رոù���
                LA->auto_send.index++;
            }else
            {
                lora_encode_send_data(LA,LA->auto_send.index);//��������

                LA->auto_send.index++;
            }
            if(LA->auto_send.index==end_data)
            {
                if(LA->auto_send.auto_enable_flag==1)
                    LA->auto_send.index=read_only+1;
                else if(LA->auto_send.auto_enable_flag==2)
                {
                    LA->auto_send.index=read_only+1;
                    LA->auto_send.auto_enable_flag=0;

                }
            }
        }
    }
}



void lora_enable_auto_send(LORA_Administrators *LA,uint8 enable)//0:�ر��Զ����͹��� 1:������������ 2:��������һ��
{
    if(enable>2) return;
    LA->auto_send.auto_enable_flag=enable;




}



void lora_encode_send_data(LORA_Administrators *LA,DATA_NAME data_name)
{

    if(data_name>=end_data ||data_name==read_only || data_name==void_data)
     {
            lora_send_str(&lora,"name_error!");
            return;
     }
    uint8 index=0;
    if(data_name>read_only)//���Ϊֻ������
        index=data_name-2-void_data;//ö��ת��������
    else                                 //���Ϊ��д����
        index=data_name-1-void_data;//ö��ת��������

    void *data_p=LA->data_index[index].data_pointer;

    if(LA->data_index[index].data_type==int_type)
        sprintf((char*)lora_send_buffer,"#dat,%c,(%d)\n",data_name,*(int *)data_p);
    else if(LA->data_index[index].data_type==double_type)
    {
        P_T g;
        g.int_part=(int)(*(float*)data_p);
        g.float_part=*(float*)data_p-g.int_part;
        sprintf((char*)lora_send_buffer,"#dat,%c,(",data_name);
        mysprintf((char*)lora_send_buffer+8,g);

     }
    else if(LA->data_index[index].data_type==float_type)
    {
        P_T g;
        g.int_part=(int)(*(float*)data_p);
        g.float_part=*(float*)data_p-g.int_part;
        sprintf((char*)lora_send_buffer,"#dat,%c,(",data_name);
        mysprintf((char*)lora_send_buffer+8,g);
    }
    else if(LA->data_index[index].data_type==unsigned_int_type)
        sprintf((char*)lora_send_buffer,"#dat,%c,(%u)\n",data_name,*(unsigned int *)data_p);
    else if(LA->data_index[index].data_type==unsigned_short_type)
        sprintf((char*)lora_send_buffer,"#dat,%c,(%u)\n",data_name,*(unsigned short *)data_p);
    else if(LA->data_index[index].data_type==unsigned_char_type)
        sprintf((char*)lora_send_buffer,"#dat,%c,(%u)\n",data_name,*(unsigned char *)data_p);
    else if(LA->data_index[index].data_type==string_type)
        sprintf((char*)lora_send_buffer,"#dat,%c,(%s)\n",data_name,(char*)data_p);
    else if(LA->data_index[index].data_type==short_type)
        sprintf((char*)lora_send_buffer,"#dat,%c,(%hd)\n",data_name,*(short*)data_p);
    lora_send_data(LA);

}






void decode_lora_str(LORA_Administrators *LA)//����
{
    if(LA->sta.lora_receive_sta==data_recevie_unuse)
    {
        if(LA->lora_receive_buffer[1]=='c'&&LA->lora_receive_buffer[2]=='m'&&LA->lora_receive_buffer[3]=='d')
                cmd_handle(LA);
        else if(LA->lora_receive_buffer[1]=='d'&&LA->lora_receive_buffer[2]=='a'&&LA->lora_receive_buffer[3]=='t')
                dat_handle(LA);
        else if(LA->lora_receive_buffer[1]=='d'&&LA->lora_receive_buffer[2]=='a'&&LA->lora_receive_buffer[3]=='c')
                dac_handle(LA);
        LA->sta.lora_receive_sta=data_receive_used;
    }
}





void mysprintf(char *string,P_T p)
{
    if(p.float_part>=1 || p.float_part<=-1) p.float_part=0.000001;

    unsigned char index=0;
    if(p.int_part<0 || p.float_part<0)
    {
        p.int_part=-p.int_part;
        p.float_part=-p.float_part;
        string[0]='-';
        index=1;
    }
    unsigned char count=0;
    int temp=p.int_part;
    while(temp)
    {
        temp/=10;
        count++;
    }
    temp=p.int_part;
    index+=count;
    while(temp)
    {
        string[--index]='0'+temp%10;
        temp/=10;
    }
    if(p.int_part==0)
        string[index++]='0';


    index+=count;

    if(p.float_part!=0)
    {
        string[index++]='.';
        count=6;
        while(count--)
        {
            p.float_part*=10;
            string[index++]=(unsigned char)p.float_part+'0';
            p.float_part=p.float_part-(int)p.float_part;
        }
    }

    string[index++]=')';
    string[index++]='\n';
    string[index]='\0';

}





void lora_send_double(float x)
{
   char string[15];
   P_T g;
   g.int_part=(int)x;
   g.float_part=x- g.int_part;
   mysprintf(string,g);
   lora_send_str(&lora,string);
}


void write_pram_data_flash(LORA_Administrators *LA)
{
    flash_buffer_clear();//�建����
    if(flash_check(0,11))//��������ݣ���������
    {
       flash_read_page_to_buffer(0,11);// �򻺳���д��ָ�� FLASH ��������ָ��ҳ�������
    }

    uint8 index=0;void *data_p=NULL;
    uint16 index_flash=GPS_POINT_TOTAL*3;
    for(index=0;index<read_only-void_data-1;index++)
    {
        data_p=LA->data_index[index].data_pointer;
        if(LA->data_index[index].data_type==float_type)
            flash_union_buffer[index_flash].float_type=*(float*)data_p;
        else if(LA->data_index[index].data_type==unsigned_char_type)
            flash_union_buffer[index_flash].uint8_type=*(uint8*)data_p;
        else if(LA->data_index[index].data_type==short_type)
            flash_union_buffer[index_flash].int16_type=*(int16*)data_p;
        else if(LA->data_index[index].data_type==long_type)
            flash_union_buffer[index_flash].int32_type=*(int32*)data_p;
        else if(LA->data_index[index].data_type==char_type)
            flash_union_buffer[index_flash].int8_type=*(int8*)data_p;
        else if(LA->data_index[index].data_type==unsigned_short_type)
            flash_union_buffer[index_flash].uint16_type=*(uint16*)data_p;
        else if(LA->data_index[index].data_type==unsigned_long_type)
            flash_union_buffer[index_flash].uint32_type=*(uint32*)data_p;

        index_flash++;
    }

    if(flash_check(0,11))
    {
        flash_erase_page(0,11);//�Ȳ���Ƭ��
        flash_write_page_from_buffer(0,11);// ��ָ�� FLASH ��������ָ��ҳ��д�뻺����������
    }else
    {
        flash_write_page_from_buffer(0,11);
    }



}

void read_pram_data_flash(LORA_Administrators *LA)
{
    uint8 index=0;void *data_p=NULL;
    uint16 index_flash=GPS_POINT_TOTAL*3;
    for(index=0;index<read_only-void_data-1;index++)
    {
       data_p=LA->data_index[index].data_pointer;
       if(LA->data_index[index].data_type==float_type)
           *(float*)data_p=flash_union_buffer[index_flash].float_type;
       else if(LA->data_index[index].data_type==unsigned_char_type)
           *(uint8*)data_p=flash_union_buffer[index_flash].uint8_type;
       else if(LA->data_index[index].data_type==short_type)
           *(int16*)data_p=flash_union_buffer[index_flash].int16_type;
       else if(LA->data_index[index].data_type==long_type)
           *(int32*)data_p=flash_union_buffer[index_flash].int32_type;
       else if(LA->data_index[index].data_type==char_type)
           *(int8*)data_p=flash_union_buffer[index_flash].int8_type;
       else if(LA->data_index[index].data_type==unsigned_short_type)
           *(uint16*)data_p=flash_union_buffer[index_flash].uint16_type;
       else if(LA->data_index[index].data_type==unsigned_long_type)
           *(uint32*)data_p=flash_union_buffer[index_flash].uint32_type;

       index_flash++;
    }




}




