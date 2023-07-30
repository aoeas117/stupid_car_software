

#include "isr_config.h"
#include "isr.h"
#include "IfxGtm_reg.h"




// **************************** PIT�жϺ��� ****************************
IFX_INTERRUPT(cc60_pit_ch0_isr, 0, CCU6_0_CH0_ISR_PRIORITY)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    pit_clear_flag(CCU60_CH0);
    tsl1401_collect_pit_handler();                  // ����CCD�ɼ�

    if(gps_tau1201_flag)
    {
           gps_tau1201_flag = 0;
           gps_data_parse();
//           if(control.imu_solution_flag)
//               gps_get_data();
    }
    icm_get_data();
    icm_filter();


}


IFX_INTERRUPT(cc60_pit_ch1_isr, 0, CCU6_0_CH1_ISR_PRIORITY)//���ݷ���
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    pit_clear_flag(CCU60_CH1);

    if(gps_asistant_yaw_flag==0)
    {
        if(gps_asistant_time_count==25)
        {
            gps_asistant_time_count=0;
            gps_asistant_yaw_flag=1;
        }else
        {
            gps_asistant_time_count++;
        }

    }


    lora_auto_send(&lora);//�Զ���������

}

uint8 bridge_count=0;

IFX_INTERRUPT(cc61_pit_ch0_isr, 0, CCU6_1_CH0_ISR_PRIORITY)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    pit_clear_flag(CCU61_CH0);


    gps_get_data();



//    if(control.search_point_flag|| control.seach_point.flag)
//    {
//        bridge_count++;
//
//        if(bridge_count==10)
//        {
//            bridge_count=0;
//            lora_send_double(bridge);
//        }
//    }


}

IFX_INTERRUPT(cc61_pit_ch1_isr, 0, CCU6_1_CH1_ISR_PRIORITY)//״̬���
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    pit_clear_flag(CCU61_CH1);

    phase_change_time_check(motor);//��ת���(��ʱɲ��)
    adc_information_read();//��ȡ������ѹ��Ϣ
    adc_information_check();//���������ż��
    //motor_set_pwm_ud(motor,motor_pwm_up);
    //lora_send_double(bridge);
    if(control.imu_solution_flag && gps_asistant_flag)
        servo_control_angle_output(&servo);





    if(motor->motor_speed.motor_brake==0 )
    {
        if(motor->control_mode==0)
        motor_control_speed_output(motor);



    }








}



//*****************************SPE0�жϺ���****************************

IFX_INTERRUPT (spe_isr, 0, SRC_SPE_INT_PRIO)
{







    if (GTM_SPE0_IRQ_NOTIFY.B.SPE_RCMP)         // ��ת����ƥ���ж�
    {
        GTM_SPE0_IRQ_NOTIFY.B.SPE_RCMP = 1;     // д��1����Ĵ�������

    }

    if (GTM_SPE0_IRQ_NOTIFY.B.SPE_DCHG)         // �����ת����仯
    {
        GTM_SPE0_IRQ_NOTIFY.B.SPE_DCHG = 1;

    }

    if (GTM_SPE0_IRQ_NOTIFY.B.SPE_PERR)         // ����������Ч
    {
        GTM_SPE0_IRQ_NOTIFY.B.SPE_PERR = 1;
        spe0_pattern_cfg();

    }

    if (GTM_SPE0_IRQ_NOTIFY.B.SPE_BIS)          // ��⵽�����źŷ���  ��Ծ�ź� һ���жϳ��ֶ�λ���ֵ�ı�
    {
        GTM_SPE0_IRQ_NOTIFY.B.SPE_BIS = 1;

    }

    if (GTM_SPE0_IRQ_NOTIFY.B.SPE_NIPD)         // �µ�����ȷ������ģʽ�����ࣩ
    {
        GTM_SPE0_IRQ_NOTIFY.B.SPE_NIPD = 1;
        motor_get_speed(motor);//��⵱ǰ�ٶ�
        if(location_licheng.add_flag<12)
        location_licheng.add_flag++;


    }
}
// **************************** PIT�жϺ��� ****************************


// **************************** �ⲿ�жϺ��� ****************************
IFX_INTERRUPT(exti_ch0_ch4_isr, 0, EXTI_CH0_CH4_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    if(exti_flag_get(ERU_CH0_REQ0_P15_4))           // ͨ��0�ж�
    {
        exti_flag_clear(ERU_CH0_REQ0_P15_4);




    }

    if(exti_flag_get(ERU_CH4_REQ13_P15_5))          // ͨ��4�ж�
    {
        exti_flag_clear(ERU_CH4_REQ13_P15_5);




    }
}

IFX_INTERRUPT(exti_ch1_ch5_isr, 0, EXTI_CH1_CH5_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    if(exti_flag_get(ERU_CH1_REQ10_P14_3))          // ͨ��1�ж�
    {
        exti_flag_clear(ERU_CH1_REQ10_P14_3);



    }

    if(exti_flag_get(ERU_CH5_REQ1_P15_8))           // ͨ��5�ж�
    {
        exti_flag_clear(ERU_CH5_REQ1_P15_8);




    }
}

// ��������ͷpclk����Ĭ��ռ���� 2ͨ�������ڴ���DMA��������ﲻ�ٶ����жϺ���
// IFX_INTERRUPT(exti_ch2_ch6_isr, 0, EXTI_CH2_CH6_INT_PRIO)
// {
//  interrupt_global_enable(0);                     // �����ж�Ƕ��
//  if(exti_flag_get(ERU_CH2_REQ7_P00_4))           // ͨ��2�ж�
//  {
//      exti_flag_clear(ERU_CH2_REQ7_P00_4);
//  }
//  if(exti_flag_get(ERU_CH6_REQ9_P20_0))           // ͨ��6�ж�
//  {
//      exti_flag_clear(ERU_CH6_REQ9_P20_0);
//  }
// }

IFX_INTERRUPT(exti_ch3_ch7_isr, 0, EXTI_CH3_CH7_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    if(exti_flag_get(ERU_CH3_REQ6_P02_0))           // ͨ��3�ж�
    {
        exti_flag_clear(ERU_CH3_REQ6_P02_0);
        camera_vsync_handler();                     // ����ͷ�����ɼ�ͳһ�ص�����
    }
    if(exti_flag_get(ERU_CH7_REQ16_P15_1))          // ͨ��7�ж�
    {
        exti_flag_clear(ERU_CH7_REQ16_P15_1);




    }
}
// **************************** �ⲿ�жϺ��� ****************************


// **************************** DMA�жϺ��� ****************************
IFX_INTERRUPT(dma_ch5_isr, 0, DMA_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    camera_dma_handler();                           // ����ͷ�ɼ����ͳһ�ص�����
}
// **************************** DMA�жϺ��� ****************************


// **************************** �����жϺ��� ****************************
IFX_INTERRUPT(uart0_tx_isr, 0, UART0_TX_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrTransmit(&uart0_handle);


}
IFX_INTERRUPT(uart0_rx_isr, 0, UART0_RX_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrReceive(&uart0_handle);

#if DEBUG_UART_USE_INTERRUPT                        // ������� debug �����ж�
        debug_interrupr_handler();                  // ���� debug ���ڽ��մ����� ���ݻᱻ debug ���λ�������ȡ
#endif                                              // ����޸��� DEBUG_UART_INDEX ����δ�����Ҫ�ŵ���Ӧ�Ĵ����ж�ȥ

}
IFX_INTERRUPT(uart0_er_isr, 0, UART0_ER_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrError(&uart0_handle);



}

// ����1Ĭ�����ӵ�����ͷ���ô���
IFX_INTERRUPT(uart1_tx_isr, 0, UART1_TX_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrTransmit(&uart1_handle);




}
IFX_INTERRUPT(uart1_rx_isr, 0, UART1_RX_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrReceive(&uart1_handle);

    gps_uart_callback();
}
IFX_INTERRUPT(uart1_er_isr, 0, UART1_ER_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrError(&uart1_handle);




}


// ����2Ĭ�����ӵ�����ת����ģ��
IFX_INTERRUPT(uart2_tx_isr, 0, UART2_TX_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrTransmit(&uart2_handle);





}

IFX_INTERRUPT(uart2_rx_isr, 0, UART2_RX_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrReceive(&uart2_handle);
    lora_receive_data(&lora);

}
IFX_INTERRUPT(uart2_er_isr, 0, UART2_ER_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrError(&uart2_handle);

}



IFX_INTERRUPT(uart3_tx_isr, 0, UART3_TX_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrTransmit(&uart3_handle);




}
IFX_INTERRUPT(uart3_rx_isr, 0, UART3_RX_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrReceive(&uart3_handle);





}
IFX_INTERRUPT(uart3_er_isr, 0, UART3_ER_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrError(&uart3_handle);





}
// **************************** �����жϺ��� ****************************
