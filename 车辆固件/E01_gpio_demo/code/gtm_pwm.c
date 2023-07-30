/*
 * gtn_pwm.c
 *
 *  Created on: 2023��3��3��
 *      Author: ���¹�
 */

#include "gtm_pwm.h"



#include "IFXGTM_TIM_IN.h"
#include "ifxGtm_Tim.h"


MOTOR_Administer motor_administer;
MOTOR_Administer*motor;





/*GTMʱ������*/

static void clock_cfg(void)
{
    IfxGtm_enable(&MODULE_GTM);//ʹ��GTMģ��

    GTM_CMU_CLK_EN.U                    = CMU_CLK_ALL_DISABLE;      //�ر�����CMUʱ��

    GTM_CMU_GCLK_NUM.B.GCLK_NUM         = 1;                        // ��Ƶϵ���ķ���
    GTM_CMU_GCLK_DEN.B.GCLK_DEN         = 1;                        // ��Ƶϵ���ķ�ĸ

    GTM_CMU_CLK_0_CTRL.B.CLK_CNT        = 0;                        // ��Ƶϵ��
    GTM_CMU_CLK_1_CTRL.B.CLK_CNT        = 0;
    GTM_CMU_FXCLK_CTRL.B.FXCLK_SEL      = 0;                        // ѡ��FXCLK��ʱ����Դ

    GTM_CMU_CLK_EN.B.EN_CLK0            = CMU_CLK_ENABLE;
    GTM_CMU_CLK_EN.B.EN_CLK1            = CMU_CLK_ENABLE;
    GTM_CMU_CLK_EN.B.EN_FXCLK           = CMU_CLK_ENABLE;
}





/*GTMģ���ж�ˢ��*/

static void ccm_cfg(void)
{
    GTM_CTRL.B.RF_PROT                  = TRUE;                     // �ر�ȫ���ж� 1���ر�
    GTM_CTRL.B.RF_PROT                  = FALSE;                    // ����ȫ���ж� 0������
}







/*    ��ʼ�����������Լ�ɲ������*/

static void hall_tim_input_cfg(void)
{
    IfxGtm_PinMap_setTimTin(&HALL_A, IfxPort_InputMode_pullUp);     // ���û���ͨ��Ϊ��������ģʽ
    IfxGtm_PinMap_setTimTin(&HALL_B, IfxPort_InputMode_pullUp);
    IfxGtm_PinMap_setTimTin(&HALL_C, IfxPort_InputMode_pullUp);

    IfxGtm_PinMap_setTimTin(&FSOI_PIN, IfxPort_InputMode_pullDown);

    GTM_TIM0_CH0_CTRL.B.CICTRL = IfxGtm_Tim_Input_currentChannel;   // ʹ�õ�ǰͨ��
    GTM_TIM0_IN_SRC.B.MODE_0   = 0;
    GTM_TIM0_IN_SRC.B.VAL_0    = 0;

    GTM_TIM0_CH1_CTRL.B.CICTRL = IfxGtm_Tim_Input_currentChannel;   // ʹ�õ�ǰͨ��
    GTM_TIM0_IN_SRC.B.MODE_1   = 0;
    GTM_TIM0_IN_SRC.B.VAL_1    = 0;

    GTM_TIM0_CH2_CTRL.B.CICTRL = IfxGtm_Tim_Input_currentChannel;   // ʹ�õ�ǰͨ��
    GTM_TIM0_IN_SRC.B.MODE_2   = 0;
    GTM_TIM0_IN_SRC.B.VAL_2    = 0;

    //���·ֱ��ͨ��0��ͨ��1��ͨ��2��������

    GTM_TIM0_CH0_CTRL.B.CLK_SEL      = IfxGtm_Cmu_Clk_0;                                //ѡ��cmuʱ��0
    GTM_TIM0_CH0_CTRL.B.FLT_CNT_FRQ  = IfxGtm_Cmu_Tim_Filter_Clk_0;                     // �˲�����Ƶ��ѡ��
    GTM_TIM0_CH0_CTRL.B.FLT_EN       = TRUE;                                            // �˲�ʹ��
    GTM_TIM0_CH0_CTRL.B.FLT_MODE_FE  = IfxGtm_Tim_FilterMode_individualDeglitchTime;    // �½����˲�ģʽ ����
    GTM_TIM0_CH0_CTRL.B.FLT_MODE_RE  = IfxGtm_Tim_FilterMode_individualDeglitchTime;    // �Ͻ����˲�ģʽ ����
    GTM_TIM0_CH0_CTRL.B.FLT_CTR_FE   = IfxGtm_Tim_FilterCounter_upDown;                 // ��ҪFLT_MODE_RE����Ϊ1����Ч
    GTM_TIM0_CH0_CTRL.B.FLT_CTR_RE   = IfxGtm_Tim_FilterCounter_upDown;                 // ��ҪFLT_MODE_RE����Ϊ1����Ч
    GTM_TIM0_CH0_FLT_FE.B.FLT_FE     = FILTER_CON_NUM;                                  // �����˲�����ֵ
    GTM_TIM0_CH0_FLT_RE.B.FLT_RE     = FILTER_CON_NUM;

    GTM_TIM0_CH1_CTRL.B.CLK_SEL      = IfxGtm_Cmu_Clk_0;
    GTM_TIM0_CH1_CTRL.B.FLT_CNT_FRQ  = IfxGtm_Cmu_Tim_Filter_Clk_0;                     // �˲�����Ƶ��ѡ��
    GTM_TIM0_CH1_CTRL.B.FLT_EN       = TRUE;                                            // �˲�ʹ��
    GTM_TIM0_CH1_CTRL.B.FLT_MODE_FE  = IfxGtm_Tim_FilterMode_individualDeglitchTime;    // �½����˲�ģʽ ����
    GTM_TIM0_CH1_CTRL.B.FLT_MODE_RE  = IfxGtm_Tim_FilterMode_individualDeglitchTime;    // �Ͻ����˲�ģʽ ����
    GTM_TIM0_CH1_CTRL.B.FLT_CTR_FE   = IfxGtm_Tim_FilterCounter_upDown;                 // ��ҪFLT_MODE_RE����Ϊ1����Ч
    GTM_TIM0_CH1_CTRL.B.FLT_CTR_RE   = IfxGtm_Tim_FilterCounter_upDown;                 // ��ҪFLT_MODE_RE����Ϊ1����Ч
    GTM_TIM0_CH1_FLT_FE.B.FLT_FE     = FILTER_CON_NUM;                                  // �����˲�����ֵ
    GTM_TIM0_CH1_FLT_RE.B.FLT_RE     = FILTER_CON_NUM;

    GTM_TIM0_CH2_CTRL.B.CLK_SEL      = IfxGtm_Cmu_Clk_0;
    GTM_TIM0_CH2_CTRL.B.FLT_CNT_FRQ  = IfxGtm_Cmu_Tim_Filter_Clk_0;                     // �˲�����Ƶ��ѡ��
    GTM_TIM0_CH2_CTRL.B.FLT_EN       = TRUE;                                            // �˲�ʹ��
    GTM_TIM0_CH2_CTRL.B.FLT_MODE_FE  = IfxGtm_Tim_FilterMode_individualDeglitchTime;    // �½����˲�ģʽ ����
    GTM_TIM0_CH2_CTRL.B.FLT_MODE_RE  = IfxGtm_Tim_FilterMode_individualDeglitchTime;    // �Ͻ����˲�ģʽ ����
    GTM_TIM0_CH2_CTRL.B.FLT_CTR_FE   = IfxGtm_Tim_FilterCounter_upDown;                 // ��ҪFLT_MODE_RE����Ϊ1����Ч
    GTM_TIM0_CH2_CTRL.B.FLT_CTR_RE   = IfxGtm_Tim_FilterCounter_upDown;                 // ��ҪFLT_MODE_RE����Ϊ1����Ч
    GTM_TIM0_CH2_FLT_FE.B.FLT_FE     = FILTER_CON_NUM;                                  // �����˲�����ֵ
    GTM_TIM0_CH2_FLT_RE.B.FLT_RE     = FILTER_CON_NUM;

    GTM_TIM0_CH0_CTRL.B.TIM_MODE     = IfxGtm_Tim_Mode_inputEvent;                      // �����¼�
    GTM_TIM0_CH0_CTRL.B.ISL          = TRUE;                                            // ˫���ش���
    GTM_TIM0_CH0_CTRL.B.TIM_EN       = TRUE;                                            // ʹ��
    GTM_TIM0_CH1_CTRL.B.TIM_MODE     = IfxGtm_Tim_Mode_inputEvent;
    GTM_TIM0_CH1_CTRL.B.ISL          = TRUE;
    GTM_TIM0_CH1_CTRL.B.TIM_EN       = TRUE;
    GTM_TIM0_CH2_CTRL.B.TIM_MODE     = IfxGtm_Tim_Mode_inputEvent;
    GTM_TIM0_CH2_CTRL.B.ISL          = TRUE;
    GTM_TIM0_CH2_CTRL.B.TIM_EN       = TRUE;

    //FSOI���ţ���Ҫ���ڿ��ٹر����
    GTM_TIM1_CH6_CTRL.B.TIM_MODE     = IfxGtm_Tim_Mode_inputEvent;
    GTM_TIM1_CH6_CTRL.B.ISL          = TRUE;
    GTM_TIM1_CH6_CTRL.B.TIM_EN       = TRUE;
}


//����ͨ����������źź�����SPE��������







/*    SPE0ģ���ж�����*/

static void spe0_irq_init(void)
{
    IfxSrc_init(&SRC_GTMSPE0IRQ, SRC_CPU0, SRC_SPE_INT_PRIO);       // ����SPE���ж����ȼ� �Լ������жϵ�CPU
    IfxSrc_enable(&SRC_GTMSPE0IRQ);                                 // �ж�ʹ��

    GTM_SPE0_IRQ_EN.U   = SRC_ENABLE_ALL;                           // SPEģ���ж�ʹ��
    GTM_SPE0_EIRQ_EN.U  = SRC_ENABLE_ALL;                           // �����ж�ʹ��
    GTM_SPE0_IRQ_MODE.U = SRC_PULSE_NOTIFY;                         // �ж�����
}



/*    SPE0ģ���ͨ����ʼ��  */

static void spe0_cfg (void)
{
    spe0_irq_init();

    GTM_SPE0_PAT.B.IP0_VAL = VALID_IN_PATTERN;      // ����ģʽ��Ч
    GTM_SPE0_PAT.B.IP1_VAL = VALID_IN_PATTERN;
    GTM_SPE0_PAT.B.IP2_VAL = VALID_IN_PATTERN;
    GTM_SPE0_PAT.B.IP3_VAL = VALID_IN_PATTERN;
    GTM_SPE0_PAT.B.IP4_VAL = VALID_IN_PATTERN;
    GTM_SPE0_PAT.B.IP5_VAL = VALID_IN_PATTERN;
    GTM_SPE0_PAT.B.IP6_VAL = INVALID_IN_PATTERN;    // ����ģʽ��Ч
    GTM_SPE0_PAT.B.IP7_VAL = INVALID_IN_PATTERN;


    GTM_SPE0_PAT.B.IP0_PAT = HALL_HARDWARE_ORDER_0; // ����TIMx y z��������ֵ  ʵ�ʾ��ǻ�������ֵ
    GTM_SPE0_PAT.B.IP1_PAT = HALL_HARDWARE_ORDER_1;
    GTM_SPE0_PAT.B.IP2_PAT = HALL_HARDWARE_ORDER_2;
    GTM_SPE0_PAT.B.IP3_PAT = HALL_HARDWARE_ORDER_3;
    GTM_SPE0_PAT.B.IP4_PAT = HALL_HARDWARE_ORDER_4;
    GTM_SPE0_PAT.B.IP5_PAT = HALL_HARDWARE_ORDER_5;
    GTM_SPE0_PAT.B.IP6_PAT = 0;//��Чֵ
    GTM_SPE0_PAT.B.IP7_PAT = 0;



    if(!motor->motor_speed.motor_dir==0)
    {
        GTM_SPE0_OUT_PAT0.U = MOTOR_FOREWARD_0;     // ��ת�����
        GTM_SPE0_OUT_PAT1.U = MOTOR_FOREWARD_1;
        GTM_SPE0_OUT_PAT2.U = MOTOR_FOREWARD_2;
        GTM_SPE0_OUT_PAT3.U = MOTOR_FOREWARD_3;
        GTM_SPE0_OUT_PAT4.U = MOTOR_FOREWARD_4;
        GTM_SPE0_OUT_PAT5.U = MOTOR_FOREWARD_5;
    }
    else
    {
        GTM_SPE0_OUT_PAT0.U = MOTOR_REVERSAL_0;     // ��ת�����
        GTM_SPE0_OUT_PAT1.U = MOTOR_REVERSAL_1;
        GTM_SPE0_OUT_PAT2.U = MOTOR_REVERSAL_2;
        GTM_SPE0_OUT_PAT3.U = MOTOR_REVERSAL_3;
        GTM_SPE0_OUT_PAT4.U = MOTOR_REVERSAL_4;
        GTM_SPE0_OUT_PAT5.U = MOTOR_REVERSAL_5;
    }
    GTM_SPE0_OUT_PAT6.U = 0x0;                      // ��Чֵ
    GTM_SPE0_OUT_PAT7.U = 0x0;

    // ÿ����10�δ����жϣ���Ҫ�����Ǳ���ͨ��һ������ʱ��ȥ�����ٶȣ��Ӷ����¼�����ٶ��ǲ����ġ�
    // �ٶȲ�������Ϊ������װ���µĻ���ʱ���ǲ�һ���ġ�
    GTM_SPE0_CMP.B.CMP = REV_CNT_VAL & 0xFFFFFF;
    spe0_pattern_init_cfg();

    GTM_SPE0_CTRL_STAT.B.SPE_EN   = TRUE;
    GTM_SPE0_CTRL_STAT.B.SIE0     = TRUE;                               // xͨ��ʹ��
    GTM_SPE0_CTRL_STAT.B.SIE1     = TRUE;                               // yͨ��ʹ��
    GTM_SPE0_CTRL_STAT.B.SIE2     = TRUE;                               // zͨ��ʹ��
    GTM_SPE0_CTRL_STAT.B.FSOM     = 1;                                  // ���ٹر�ģʽʹ��
    GTM_SPE0_CTRL_STAT.B.PDIR     = 0;
    GTM_SPE0_CTRL_STAT.B.ADIR     = 0;
    GTM_SPE0_CTRL_STAT.B.FSOL     = 0xFF;                               // �رյ�ʱ��8��ͨ�������ֵ�趨
    GTM_SPE0_CTRL_STAT.B.TRIG_SEL = 2;                                  // ʹ��TOM[i] CH1��ʱ��������
    GTM_SPE0_CTRL_STAT.B.TIM_SEL  = 0;                                  // ʹ��TIM0 ͨ��0-ͨ��2��ΪSPE����
}







static void spe0_pattern_init_cfg (void)
{


    switch (GTM_SPE0_CTRL_STAT.B.NIP)
    {
        case HALL_HARDWARE_ORDER_0:
            GTM_SPE0_CTRL_STAT.B.SPE_PAT_PTR        = PAT_PTR_0;
            GTM_SPE0_CTRL_STAT.B.PIP                = HALL_HARDWARE_ORDER_5;
            GTM_SPE0_CTRL_STAT.B.AIP                = HALL_HARDWARE_ORDER_0;
            GTM_SPE0_OUT_CTRL.B.SPE_OUT_CTRL        = MOTOR_FOREWARD_0;
            break;
        case HALL_HARDWARE_ORDER_1:
            GTM_SPE0_CTRL_STAT.B.SPE_PAT_PTR        = PAT_PTR_1;
            GTM_SPE0_CTRL_STAT.B.PIP                = HALL_HARDWARE_ORDER_0;
            GTM_SPE0_CTRL_STAT.B.AIP                = HALL_HARDWARE_ORDER_1;
            GTM_SPE0_OUT_CTRL.B.SPE_OUT_CTRL        = MOTOR_FOREWARD_1;
            break;
        case HALL_HARDWARE_ORDER_2:
            GTM_SPE0_CTRL_STAT.B.SPE_PAT_PTR        = PAT_PTR_2;
            GTM_SPE0_CTRL_STAT.B.PIP                = HALL_HARDWARE_ORDER_1;
            GTM_SPE0_CTRL_STAT.B.AIP                = HALL_HARDWARE_ORDER_2;
            GTM_SPE0_OUT_CTRL.B.SPE_OUT_CTRL        = MOTOR_FOREWARD_2;
            break;
        case HALL_HARDWARE_ORDER_3:
            GTM_SPE0_CTRL_STAT.B.SPE_PAT_PTR        = PAT_PTR_3;
            GTM_SPE0_CTRL_STAT.B.PIP                = HALL_HARDWARE_ORDER_2;
            GTM_SPE0_CTRL_STAT.B.AIP                = HALL_HARDWARE_ORDER_3;
            GTM_SPE0_OUT_CTRL.B.SPE_OUT_CTRL        = MOTOR_FOREWARD_3;
            break;
        case HALL_HARDWARE_ORDER_4:
            GTM_SPE0_CTRL_STAT.B.SPE_PAT_PTR        = PAT_PTR_4;
            GTM_SPE0_CTRL_STAT.B.PIP                = HALL_HARDWARE_ORDER_3;
            GTM_SPE0_CTRL_STAT.B.AIP                = HALL_HARDWARE_ORDER_4;
            GTM_SPE0_OUT_CTRL.B.SPE_OUT_CTRL        = MOTOR_FOREWARD_4;
            break;
        case HALL_HARDWARE_ORDER_5:
            GTM_SPE0_CTRL_STAT.B.SPE_PAT_PTR        = PAT_PTR_5;
            GTM_SPE0_CTRL_STAT.B.PIP                = HALL_HARDWARE_ORDER_4;
            GTM_SPE0_CTRL_STAT.B.AIP                = HALL_HARDWARE_ORDER_5;
            GTM_SPE0_OUT_CTRL.B.SPE_OUT_CTRL        = MOTOR_FOREWARD_5;
            break;
        default:
            break;
    }

}


//���ڻ���������ʱ
void spe0_pattern_cfg (void)
{


    switch ( GTM_SPE0_CTRL_STAT.B.NIP)
    {
        case HALL_HARDWARE_ORDER_0:
            GTM_SPE0_CTRL_STAT.B.SPE_PAT_PTR        = PAT_PTR_0;
            GTM_SPE0_CTRL_STAT.B.PIP                = HALL_HARDWARE_ORDER_5;
            GTM_SPE0_CTRL_STAT.B.AIP                = HALL_HARDWARE_ORDER_0;
            GTM_SPE0_OUT_CTRL.B.SPE_OUT_CTRL        = MOTOR_FOREWARD_0;
            break;
        case HALL_HARDWARE_ORDER_1:
            GTM_SPE0_CTRL_STAT.B.SPE_PAT_PTR        = PAT_PTR_1;
            GTM_SPE0_CTRL_STAT.B.PIP                = HALL_HARDWARE_ORDER_0;
            GTM_SPE0_CTRL_STAT.B.AIP                = HALL_HARDWARE_ORDER_1;
            GTM_SPE0_OUT_CTRL.B.SPE_OUT_CTRL        = MOTOR_FOREWARD_1;
            break;
        case HALL_HARDWARE_ORDER_2:
            GTM_SPE0_CTRL_STAT.B.SPE_PAT_PTR        = PAT_PTR_2;
            GTM_SPE0_CTRL_STAT.B.PIP                = HALL_HARDWARE_ORDER_1;
            GTM_SPE0_CTRL_STAT.B.AIP                = HALL_HARDWARE_ORDER_2;
            GTM_SPE0_OUT_CTRL.B.SPE_OUT_CTRL        = MOTOR_FOREWARD_2;
            break;
        case HALL_HARDWARE_ORDER_3:
            GTM_SPE0_CTRL_STAT.B.SPE_PAT_PTR        = PAT_PTR_3;
            GTM_SPE0_CTRL_STAT.B.PIP                = HALL_HARDWARE_ORDER_2;
            GTM_SPE0_CTRL_STAT.B.AIP                = HALL_HARDWARE_ORDER_3;
            GTM_SPE0_OUT_CTRL.B.SPE_OUT_CTRL        = MOTOR_FOREWARD_3;
            break;
        case HALL_HARDWARE_ORDER_4:
            GTM_SPE0_CTRL_STAT.B.SPE_PAT_PTR        = PAT_PTR_4;
            GTM_SPE0_CTRL_STAT.B.PIP                = HALL_HARDWARE_ORDER_3;
            GTM_SPE0_CTRL_STAT.B.AIP                = HALL_HARDWARE_ORDER_4;
            GTM_SPE0_OUT_CTRL.B.SPE_OUT_CTRL        = MOTOR_FOREWARD_4;
            break;
        case HALL_HARDWARE_ORDER_5:
            GTM_SPE0_CTRL_STAT.B.SPE_PAT_PTR        = PAT_PTR_5;
            GTM_SPE0_CTRL_STAT.B.PIP                = HALL_HARDWARE_ORDER_4;
            GTM_SPE0_CTRL_STAT.B.AIP                = HALL_HARDWARE_ORDER_5;
            GTM_SPE0_OUT_CTRL.B.SPE_OUT_CTRL        = MOTOR_FOREWARD_5;
            break;
        default:
            break;
    }
}



/*   SPE0-TOMģ���ͨ������ */

static void spe0_tom_tgc_cfg (void)
{
    GTM_TOM0_TGC0_FUPD_CTRL.B.FUPD_CTRL0 = FORCE_UPDATE_EN  & 0x3;      // ǿ�Ƹ���
    GTM_TOM0_TGC0_FUPD_CTRL.B.FUPD_CTRL2 = FORCE_UPDATE_EN  & 0x3;
    GTM_TOM0_TGC0_FUPD_CTRL.B.FUPD_CTRL1 = FORCE_UPDATE_DIS & 0x3;      // ����
    GTM_TOM0_TGC0_FUPD_CTRL.B.FUPD_CTRL3 = FORCE_UPDATE_DIS & 0x3;
    GTM_TOM0_TGC0_FUPD_CTRL.B.FUPD_CTRL4 = FORCE_UPDATE_DIS & 0x3;
    GTM_TOM0_TGC0_FUPD_CTRL.B.FUPD_CTRL5 = FORCE_UPDATE_DIS & 0x3;
    GTM_TOM0_TGC0_FUPD_CTRL.B.FUPD_CTRL6 = FORCE_UPDATE_DIS & 0x3;
    GTM_TOM0_TGC0_FUPD_CTRL.B.FUPD_CTRL7 = FORCE_UPDATE_DIS & 0x3;

    GTM_TOM0_TGC0_OUTEN_CTRL.B.OUTEN_CTRL0 = TOM_OUTPUT_ENABLE & 0x3;   // �ڸ��´�����ʱ��ʹ�����
    GTM_TOM0_TGC0_OUTEN_CTRL.B.OUTEN_CTRL1 = TOM_OUTPUT_ENABLE & 0x3;
    GTM_TOM0_TGC0_OUTEN_CTRL.B.OUTEN_CTRL2 = TOM_OUTPUT_ENABLE & 0x3;
    GTM_TOM0_TGC0_OUTEN_CTRL.B.OUTEN_CTRL3 = TOM_OUTPUT_ENABLE & 0x3;
    GTM_TOM0_TGC0_OUTEN_CTRL.B.OUTEN_CTRL4 = TOM_OUTPUT_ENABLE & 0x3;
    GTM_TOM0_TGC0_OUTEN_CTRL.B.OUTEN_CTRL5 = TOM_OUTPUT_ENABLE & 0x3;
    GTM_TOM0_TGC0_OUTEN_CTRL.B.OUTEN_CTRL6 = TOM_OUTPUT_ENABLE & 0x3;
    GTM_TOM0_TGC0_OUTEN_CTRL.B.OUTEN_CTRL7 = TOM_OUTPUT_ENABLE & 0x3;

    GTM_TOM0_TGC0_ENDIS_CTRL.B.ENDIS_CTRL0 = ENDIS_EN;
    GTM_TOM0_TGC0_ENDIS_CTRL.B.ENDIS_CTRL2 = ENDIS_EN;
    GTM_TOM0_TGC0_ENDIS_CTRL.B.ENDIS_CTRL1 = ENDIS_DIS;
    GTM_TOM0_TGC0_ENDIS_CTRL.B.ENDIS_CTRL3 = ENDIS_DIS;
    GTM_TOM0_TGC0_ENDIS_CTRL.B.ENDIS_CTRL4 = ENDIS_DIS;
    GTM_TOM0_TGC0_ENDIS_CTRL.B.ENDIS_CTRL5 = ENDIS_DIS;
    GTM_TOM0_TGC0_ENDIS_CTRL.B.ENDIS_CTRL6 = ENDIS_DIS;
    GTM_TOM0_TGC0_ENDIS_CTRL.B.ENDIS_CTRL7 = ENDIS_DIS;

    GTM_TOM0_TGC0_GLB_CTRL.B.UPEN_CTRL0 = ENDIS_EN  & 0x3;
    GTM_TOM0_TGC0_GLB_CTRL.B.UPEN_CTRL2 = ENDIS_EN  & 0x3;
    GTM_TOM0_TGC0_GLB_CTRL.B.UPEN_CTRL1 = ENDIS_DIS & 0x3;
    GTM_TOM0_TGC0_GLB_CTRL.B.UPEN_CTRL3 = ENDIS_DIS & 0x3;
    GTM_TOM0_TGC0_GLB_CTRL.B.UPEN_CTRL4 = ENDIS_DIS & 0x3;
    GTM_TOM0_TGC0_GLB_CTRL.B.UPEN_CTRL5 = ENDIS_DIS & 0x3;
    GTM_TOM0_TGC0_GLB_CTRL.B.UPEN_CTRL6 = ENDIS_DIS & 0x3;
    GTM_TOM0_TGC0_GLB_CTRL.B.UPEN_CTRL7 = ENDIS_DIS & 0x3;
}









/*   ��ʼ��TOM0���ģ��   */

static void spe0_pwm_tom_cfg (void)
{
    spe0_tom_tgc_cfg ();

    GTM_TOM0_CH2_CTRL.B.OSM        = 1;                 // �����ر�one shot mode
    GTM_TOM0_CH2_CTRL.B.SPEM       = 1;
    GTM_TOM0_CH2_CTRL.B.CLK_SRC_SR = TOM_CH_CMU_FXCLK0;
    GTM_TOM0_CH2_SR0.U = 10;
    GTM_TOM0_CH2_SR1.U = 10;                            // �����ӳٻ���ʱ��

    GTM_TOM0_CH0_CTRL.B.SL         = TOM_CH_SL_HIGH;    // ռ�ձȱ�ʾ���Ǹߵ�ƽ��ռ��
    GTM_TOM0_CH0_CTRL.B.OSM        = OSM_DIS;           // �ر�one shot mode
    GTM_TOM0_CH0_CTRL.B.SPEM       = SPEM_DIS;
    GTM_TOM0_CH0_CTRL.B.TRIGOUT    = TRIG_CCU0;
    GTM_TOM0_CH0_CTRL.B.CLK_SRC_SR = TOM_CH_CMU_FXCLK0;
    GTM_TOM0_CH0_CTRL.B.RST_CCU0   = CM0_MATCHING;

    /*****************�ٶ�����*****************/


    GTM_TOM0_CH0_SR1.U = motor->motor_speed.motor_duty_output;
    GTM_TOM0_CH0_SR0.U =  motor->motor_speed.motor_period_output;
    /****************************************/
    GTM_TOM0_CH1_CTRL.B.OSM        = OSM_DIS;
    GTM_TOM0_CH1_CTRL.B.SPEM       = SPEM_EN;
    GTM_TOM0_CH1_CTRL.B.TRIGOUT    = TRIG_CCU0;
    GTM_TOM0_CH1_CTRL.B.CLK_SRC_SR = TOM_CH_CMU_FXCLK0;

    GTM_TOM0_CH3_CTRL.B.OSM        = OSM_DIS;
    GTM_TOM0_CH3_CTRL.B.SPEM       = SPEM_EN;
    GTM_TOM0_CH3_CTRL.B.TRIGOUT    = TRIG_CCU0;
    GTM_TOM0_CH3_CTRL.B.CLK_SRC_SR = TOM_CH_CMU_FXCLK0;

    GTM_TOM0_CH4_CTRL.B.OSM        = OSM_DIS;
    GTM_TOM0_CH4_CTRL.B.SPEM       = SPEM_EN;
    GTM_TOM0_CH4_CTRL.B.TRIGOUT    = TRIG_CCU0;
    GTM_TOM0_CH4_CTRL.B.CLK_SRC_SR = TOM_CH_CMU_FXCLK0;

    GTM_TOM0_CH5_CTRL.B.OSM        = OSM_DIS;
    GTM_TOM0_CH5_CTRL.B.SPEM       = SPEM_EN;
    GTM_TOM0_CH5_CTRL.B.TRIGOUT    = TRIG_CCU0;
    GTM_TOM0_CH5_CTRL.B.CLK_SRC_SR = TOM_CH_CMU_FXCLK0;

    GTM_TOM0_CH6_CTRL.B.OSM        = OSM_DIS;
    GTM_TOM0_CH6_CTRL.B.SPEM       = SPEM_EN;
    GTM_TOM0_CH6_CTRL.B.TRIGOUT    = TRIG_CCU0;
    GTM_TOM0_CH6_CTRL.B.CLK_SRC_SR = TOM_CH_CMU_FXCLK0;

    GTM_TOM0_CH7_CTRL.B.OSM        = OSM_DIS;
    GTM_TOM0_CH7_CTRL.B.SPEM       = SPEM_EN;
    GTM_TOM0_CH7_CTRL.B.TRIGOUT    = TRIG_CCU0;
    GTM_TOM0_CH7_CTRL.B.CLK_SRC_SR = TOM_CH_CMU_FXCLK0;

    GTM_TOM0_TGC0_GLB_CTRL.B.HOST_TRIG = 1;             // �����TOMģ��ȫ�ִ�������
}




static void spe0_pwm_mux_outport_cfg(void)
{
    IfxGtm_PinMap_setTomTout(&A_PHASE_PIN_H,    IfxPort_OutputMode_pushPull, IfxPort_PadDriver_cmosAutomotiveSpeed1);
    IfxGtm_PinMap_setTomTout(&A_PHASE_PIN_L,    IfxPort_OutputMode_pushPull, IfxPort_PadDriver_cmosAutomotiveSpeed1);
    IfxGtm_PinMap_setTomTout(&B_PHASE_PIN_H,    IfxPort_OutputMode_pushPull, IfxPort_PadDriver_cmosAutomotiveSpeed1);
    IfxGtm_PinMap_setTomTout(&B_PHASE_PIN_L,    IfxPort_OutputMode_pushPull, IfxPort_PadDriver_cmosAutomotiveSpeed1);
    IfxGtm_PinMap_setTomTout(&C_PHASE_PIN_H,    IfxPort_OutputMode_pushPull, IfxPort_PadDriver_cmosAutomotiveSpeed1);
    IfxGtm_PinMap_setTomTout(&C_PHASE_PIN_L,    IfxPort_OutputMode_pushPull, IfxPort_PadDriver_cmosAutomotiveSpeed1);
}






void gtm_bldc_init(void)
{
    motor_administer_init();
    clock_cfg();                // GTMģ��ʱ�����ó�ʼ��     ok
    ccm_cfg();                  // GTM�ж�ˢ��              ok
    hall_tim_input_cfg();       // ��ʼ�����������Լ�ɲ������ ok
    spe0_cfg();                 // ��ʼ��SPE0ģ��ͨ��        ok
    spe0_pwm_tom_cfg();         // ��ʼ��TOM0���ģ��        ok
    spe0_pwm_mux_outport_cfg(); // ѡ���������PWM������     ok
}





/**************control*******************/



void motor_administer_init(void)
{
    motor=&motor_administer;
    motor->motor_speed.target_speed=1200;//rpm
    motor->motor_speed.current_speed=0;
    motor->motor_speed.last_speed=0;
    /***�ٶ�*****/
    motor->motor_speed.motor_duty_output=100;
    motor->motor_speed.last_motor_duty_output=100;
    motor->motor_speed.motor_period_output=5000;

    motor->motor_speed.max_duty=1200;
    /***����*****/
    motor->motor_speed.motor_dir=0;//0��ת��1��ת
    /***********/
    motor->motor_speed.motor_brake=1;//ɲ��
    /************/
    motor->pid.p=0.6;
    motor->pid.i=0.001;
    motor->pid.d=0.002;
    motor->pid.current_error=0;
    motor->pid.last_error=0;
    motor->pid.i_output=0;
    motor->pid.output=0;
    /************/
    motor->get_speed.phase=0;
    motor->get_speed.time_sum=0;

    for(uint8 i=0;i<6;i++)
        motor->get_speed.time_save[i]=COMMUTATION_TIMEOUT;
    /*************/

    motor->control_mode=1;

    /************/




}




void motor_set_pwm(MOTOR_Administer *MA)//���ռ�ձȣ����ڲ���
{

    if(MA->motor_speed.motor_duty_output>motor->motor_speed.max_duty)
        MA->motor_speed.motor_duty_output=motor->motor_speed.max_duty;
    else if(MA->motor_speed.motor_duty_output<=0)
        MA->motor_speed.motor_duty_output=0;
    MA->motor_speed.last_motor_duty_output=MA->motor_speed.motor_duty_output;

    if( MA->motor_speed.motor_duty_output <= MA->motor_speed.motor_period_output )
    {
            GTM_TOM0_CH0_SR1.U = MA->motor_speed.motor_duty_output     & 0xFFFF;
            GTM_TOM0_CH0_SR0.U = MA->motor_speed.motor_period_output   & 0xFFFF;
    }
}


void motor_set_dir(MOTOR_Administer *MA)
{
    GTM_SPE0_CTRL_STAT.B.SPE_EN   = FALSE;

    if(!MA->motor_speed.motor_dir==0)
    {
        GTM_SPE0_OUT_PAT0.U = MOTOR_FOREWARD_0;     // ��ת�����
        GTM_SPE0_OUT_PAT1.U = MOTOR_FOREWARD_1;
        GTM_SPE0_OUT_PAT2.U = MOTOR_FOREWARD_2;
        GTM_SPE0_OUT_PAT3.U = MOTOR_FOREWARD_3;
        GTM_SPE0_OUT_PAT4.U = MOTOR_FOREWARD_4;
        GTM_SPE0_OUT_PAT5.U = MOTOR_FOREWARD_5;
    }
    else if(!MA->motor_speed.motor_dir==1)
    {
        GTM_SPE0_OUT_PAT0.U = MOTOR_REVERSAL_0;     // ��ת�����
        GTM_SPE0_OUT_PAT1.U = MOTOR_REVERSAL_1;
        GTM_SPE0_OUT_PAT2.U = MOTOR_REVERSAL_2;
        GTM_SPE0_OUT_PAT3.U = MOTOR_REVERSAL_3;
        GTM_SPE0_OUT_PAT4.U = MOTOR_REVERSAL_4;
        GTM_SPE0_OUT_PAT5.U = MOTOR_REVERSAL_5;
    }
    if(MA->motor_speed.motor_brake)
    {
       GTM_SPE0_OUT_PAT0.U = MOTOR_BRAKE;          // ɲ��
       GTM_SPE0_OUT_PAT1.U = MOTOR_BRAKE;
       GTM_SPE0_OUT_PAT2.U = MOTOR_BRAKE;
       GTM_SPE0_OUT_PAT3.U = MOTOR_BRAKE;
       GTM_SPE0_OUT_PAT4.U = MOTOR_BRAKE;
       GTM_SPE0_OUT_PAT5.U = MOTOR_BRAKE;
    }
    GTM_SPE0_CTRL_STAT.B.SPE_EN   = TRUE;
}





void motor_set_stop(MOTOR_Administer *MA,uint8 brake)//ɲ��(�������͵�ѹ����Ϊ���á���ת��)ɲ���Ժ�ռ�ձ�Ϊ0�������Ϊɲ��״̬�����Ʋ�����ʼ����
{
    if(brake==1)
    {
        if( MA->motor_speed.motor_brake==1) return;

        MA->motor_speed.motor_brake=1;

        GTM_SPE0_CTRL_STAT.B.SPE_EN   = FALSE;

        GTM_SPE0_OUT_PAT0.U = MOTOR_BRAKE;          // ɲ��
        GTM_SPE0_OUT_PAT1.U = MOTOR_BRAKE;
        GTM_SPE0_OUT_PAT2.U = MOTOR_BRAKE;
        GTM_SPE0_OUT_PAT3.U = MOTOR_BRAKE;
        GTM_SPE0_OUT_PAT4.U = MOTOR_BRAKE;
        GTM_SPE0_OUT_PAT5.U = MOTOR_BRAKE;

        GTM_SPE0_CTRL_STAT.B.SPE_EN   = TRUE;;


        //MA->motor_speed.target_speed=0;//rpm
        MA->motor_speed.current_speed=0;
        MA->motor_speed.last_speed=0;
        /***�ٶ�*****/
        MA->motor_speed.motor_duty_output=0;
        MA->motor_speed.last_motor_duty_output=0;
        MA->motor_speed.motor_period_output=5000;
        /***pid***/
        MA->pid.current_error=0;
        MA->pid.last_error=0;
        MA->pid.i_output=0;
        MA->pid.output=0;
        /************/
        MA->get_speed.phase=0;
        MA->get_speed.time_sum=0;

        for(uint8 i=0;i<6;i++)
            MA->get_speed.time_save[i]=COMMUTATION_TIMEOUT;
        motor_set_pwm(MA);
    }else
    {
        MA->motor_speed.motor_brake=0;//���ɲ����־

        motor_set_dir(MA);//���û����

        spe0_pattern_cfg();



    }



}








void motor_get_speed(MOTOR_Administer *MA)
{
   MA->get_speed.time_save[MA->get_speed.phase++]=system_getval();
   system_start();
   if(MA->get_speed.phase>5)
       MA->get_speed.phase=0;
   MA->get_speed.time_sum=0;

   for(uint8 i=0;i<6;i++)
       MA->get_speed.time_sum+=MA->get_speed.time_save[i];

   MA->motor_speed.current_speed= (uint16)(FCY / POLEPAIRS / MA->get_speed.time_sum * 60)/4;
   if(MA->motor_speed.motor_dir==1)
   {
           MA->motor_speed.current_speed = -MA->motor_speed.current_speed;
   }
   MA->motor_speed.current_speed=(MA->motor_speed.current_speed)*0.8+(MA->motor_speed.last_speed)*0.2;
   MA->motor_speed.last_speed=MA->motor_speed.current_speed;

}




void motor_control_speed_output(MOTOR_Administer *MA)//pid������
{
    MA->pid.current_error=MA->motor_speed.target_speed-MA->motor_speed.current_speed;

    MA->pid.i_output+=(float)MA->pid.current_error*MA->pid.i;
    if(MA->pid.i_output>I_OUTPUT_MAX)
           MA->pid.i_output=I_OUTPUT_MAX;
    if(MA->pid.i_output<I_OUTPUT_MIN)
           MA->pid.i_output=I_OUTPUT_MIN;

    MA->motor_speed.motor_duty_output=MA->motor_speed.target_speed*MA->pid.p+(int16)MA->pid.i_output+(MA->pid.current_error-MA->pid.last_error)*MA->pid.d;
    long int error=MA->motor_speed.motor_duty_output-MA->motor_speed.last_motor_duty_output;
    if(error>MOTOR_SCOPE)
        MA->motor_speed.motor_duty_output=MA->motor_speed.last_motor_duty_output+MOTOR_SCOPE;
    else if(error<-200)
         MA->motor_speed.motor_duty_output=MA->motor_speed.last_motor_duty_output-200;


    MA->pid.last_error=MA->pid.current_error;

    motor_set_pwm(MA);

}



void motor_set_target_speed(MOTOR_Administer *MA,int16 target_speed)
{
    MA->motor_speed.target_speed=target_speed;
}



void motor_set_pwm_from_out(MOTOR_Administer *MA,uint16 target_pwm)
{
    MA->motor_speed.motor_duty_output=target_pwm;
    motor_set_pwm(MA);

}



void motor_open_loop_switch_dir(MOTOR_Administer *MA)
{
    if(MA->motor_speed.motor_brake==0)
    {
        lora_send_str(&lora,"please brake");
        return;
    }
    if(MA->control_mode==0) return;

    if(MA->motor_speed.motor_dir==1)
    {
       MA->motor_speed.motor_dir=0;
       lora_send_str(&lora,"forward");
    }else if(MA->motor_speed.motor_dir==0)
    {
       MA->motor_speed.motor_dir=1;
       lora_send_str(&lora,"backward");
    }
}

void motor_switch_control_mode(MOTOR_Administer *MA)
{
    if(MA->motor_speed.motor_brake==0)
    {
        lora_send_str(&lora,"please brake");
        return;
    }
    if(MA->control_mode==0)
    {
        MA->control_mode=1;
        control.imu_solution_flag=0;
        lora_send_str(&lora,"openloop");
    }else if(MA->control_mode==1)
    {

        MA->control_mode=0;
        control.imu_solution_flag=1;
        lora_send_str(&lora,"closeloop");
    }
}






void motor_set_pwm_ud(MOTOR_Administer *MA,int16 target_pwm)//���˵�����������⣬�������motor_set_pwm()
{
    if(target_pwm<0)
    {
        MA->motor_speed.motor_dir=0;
        MA->motor_speed.motor_duty_output=-target_pwm;
    }else
    {
        MA->motor_speed.motor_dir=1;
        MA->motor_speed.motor_duty_output=target_pwm;
    }

    motor_set_pwm(MA);
    motor_set_dir(MA);


}





void phase_change_time_check(MOTOR_Administer *MA)//��ת����Ӧ��ת��ʱ��ת���ᵼ�µ�������
{
    static uint16 stop_times_count=0;
    if(motor->motor_speed.motor_brake==1) return;
    if(GTM_TOM0_CH0_SR1.U>400)//
    {
        if(motor->motor_speed.current_speed<20 && motor->motor_speed.current_speed>-20 )//��ʱ
        {
            stop_times_count++;
        }

        if(stop_times_count>20)
        {

            MA->get_speed.time_sum = COMMUTATION_TIMEOUT;
            MA->get_speed.time_save[0] = COMMUTATION_TIMEOUT;
            MA->get_speed.time_save[1] = COMMUTATION_TIMEOUT;
            MA->get_speed.time_save[2] = COMMUTATION_TIMEOUT;
            MA->get_speed.time_save[3] = COMMUTATION_TIMEOUT;
            MA->get_speed.time_save[4] = COMMUTATION_TIMEOUT;
            MA->get_speed.time_save[5] = COMMUTATION_TIMEOUT;

            stop_times_count=0;
            motor_set_stop(MA,1);
            lora_send_str(&lora,"brake(time)");

        }

    }
}





















