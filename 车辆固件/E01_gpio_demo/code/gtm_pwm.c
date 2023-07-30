/*
 * gtn_pwm.c
 *
 *  Created on: 2023年3月3日
 *      Author: 乘衣归
 */

#include "gtm_pwm.h"



#include "IFXGTM_TIM_IN.h"
#include "ifxGtm_Tim.h"


MOTOR_Administer motor_administer;
MOTOR_Administer*motor;





/*GTM时钟配置*/

static void clock_cfg(void)
{
    IfxGtm_enable(&MODULE_GTM);//使能GTM模块

    GTM_CMU_CLK_EN.U                    = CMU_CLK_ALL_DISABLE;      //关闭所有CMU时钟

    GTM_CMU_GCLK_NUM.B.GCLK_NUM         = 1;                        // 分频系数的分子
    GTM_CMU_GCLK_DEN.B.GCLK_DEN         = 1;                        // 分频系数的分母

    GTM_CMU_CLK_0_CTRL.B.CLK_CNT        = 0;                        // 分频系数
    GTM_CMU_CLK_1_CTRL.B.CLK_CNT        = 0;
    GTM_CMU_FXCLK_CTRL.B.FXCLK_SEL      = 0;                        // 选择FXCLK的时钟来源

    GTM_CMU_CLK_EN.B.EN_CLK0            = CMU_CLK_ENABLE;
    GTM_CMU_CLK_EN.B.EN_CLK1            = CMU_CLK_ENABLE;
    GTM_CMU_CLK_EN.B.EN_FXCLK           = CMU_CLK_ENABLE;
}





/*GTM模块中断刷新*/

static void ccm_cfg(void)
{
    GTM_CTRL.B.RF_PROT                  = TRUE;                     // 关闭全局中断 1：关闭
    GTM_CTRL.B.RF_PROT                  = FALSE;                    // 开启全局中断 0：开启
}







/*    初始化霍尔捕获以及刹车捕获*/

static void hall_tim_input_cfg(void)
{
    IfxGtm_PinMap_setTimTin(&HALL_A, IfxPort_InputMode_pullUp);     // 配置霍尔通道为上拉输入模式
    IfxGtm_PinMap_setTimTin(&HALL_B, IfxPort_InputMode_pullUp);
    IfxGtm_PinMap_setTimTin(&HALL_C, IfxPort_InputMode_pullUp);

    IfxGtm_PinMap_setTimTin(&FSOI_PIN, IfxPort_InputMode_pullDown);

    GTM_TIM0_CH0_CTRL.B.CICTRL = IfxGtm_Tim_Input_currentChannel;   // 使用当前通道
    GTM_TIM0_IN_SRC.B.MODE_0   = 0;
    GTM_TIM0_IN_SRC.B.VAL_0    = 0;

    GTM_TIM0_CH1_CTRL.B.CICTRL = IfxGtm_Tim_Input_currentChannel;   // 使用当前通道
    GTM_TIM0_IN_SRC.B.MODE_1   = 0;
    GTM_TIM0_IN_SRC.B.VAL_1    = 0;

    GTM_TIM0_CH2_CTRL.B.CICTRL = IfxGtm_Tim_Input_currentChannel;   // 使用当前通道
    GTM_TIM0_IN_SRC.B.MODE_2   = 0;
    GTM_TIM0_IN_SRC.B.VAL_2    = 0;

    //以下分别对通道0、通道1与通道2进行配置

    GTM_TIM0_CH0_CTRL.B.CLK_SEL      = IfxGtm_Cmu_Clk_0;                                //选择cmu时钟0
    GTM_TIM0_CH0_CTRL.B.FLT_CNT_FRQ  = IfxGtm_Cmu_Tim_Filter_Clk_0;                     // 滤波计数频率选择
    GTM_TIM0_CH0_CTRL.B.FLT_EN       = TRUE;                                            // 滤波使能
    GTM_TIM0_CH0_CTRL.B.FLT_MODE_FE  = IfxGtm_Tim_FilterMode_individualDeglitchTime;    // 下降沿滤波模式 立即
    GTM_TIM0_CH0_CTRL.B.FLT_MODE_RE  = IfxGtm_Tim_FilterMode_individualDeglitchTime;    // 上降沿滤波模式 立即
    GTM_TIM0_CH0_CTRL.B.FLT_CTR_FE   = IfxGtm_Tim_FilterCounter_upDown;                 // 需要FLT_MODE_RE设置为1才有效
    GTM_TIM0_CH0_CTRL.B.FLT_CTR_RE   = IfxGtm_Tim_FilterCounter_upDown;                 // 需要FLT_MODE_RE设置为1才有效
    GTM_TIM0_CH0_FLT_FE.B.FLT_FE     = FILTER_CON_NUM;                                  // 设置滤波计数值
    GTM_TIM0_CH0_FLT_RE.B.FLT_RE     = FILTER_CON_NUM;

    GTM_TIM0_CH1_CTRL.B.CLK_SEL      = IfxGtm_Cmu_Clk_0;
    GTM_TIM0_CH1_CTRL.B.FLT_CNT_FRQ  = IfxGtm_Cmu_Tim_Filter_Clk_0;                     // 滤波计数频率选择
    GTM_TIM0_CH1_CTRL.B.FLT_EN       = TRUE;                                            // 滤波使能
    GTM_TIM0_CH1_CTRL.B.FLT_MODE_FE  = IfxGtm_Tim_FilterMode_individualDeglitchTime;    // 下降沿滤波模式 立即
    GTM_TIM0_CH1_CTRL.B.FLT_MODE_RE  = IfxGtm_Tim_FilterMode_individualDeglitchTime;    // 上降沿滤波模式 立即
    GTM_TIM0_CH1_CTRL.B.FLT_CTR_FE   = IfxGtm_Tim_FilterCounter_upDown;                 // 需要FLT_MODE_RE设置为1才有效
    GTM_TIM0_CH1_CTRL.B.FLT_CTR_RE   = IfxGtm_Tim_FilterCounter_upDown;                 // 需要FLT_MODE_RE设置为1才有效
    GTM_TIM0_CH1_FLT_FE.B.FLT_FE     = FILTER_CON_NUM;                                  // 设置滤波计数值
    GTM_TIM0_CH1_FLT_RE.B.FLT_RE     = FILTER_CON_NUM;

    GTM_TIM0_CH2_CTRL.B.CLK_SEL      = IfxGtm_Cmu_Clk_0;
    GTM_TIM0_CH2_CTRL.B.FLT_CNT_FRQ  = IfxGtm_Cmu_Tim_Filter_Clk_0;                     // 滤波计数频率选择
    GTM_TIM0_CH2_CTRL.B.FLT_EN       = TRUE;                                            // 滤波使能
    GTM_TIM0_CH2_CTRL.B.FLT_MODE_FE  = IfxGtm_Tim_FilterMode_individualDeglitchTime;    // 下降沿滤波模式 立即
    GTM_TIM0_CH2_CTRL.B.FLT_MODE_RE  = IfxGtm_Tim_FilterMode_individualDeglitchTime;    // 上降沿滤波模式 立即
    GTM_TIM0_CH2_CTRL.B.FLT_CTR_FE   = IfxGtm_Tim_FilterCounter_upDown;                 // 需要FLT_MODE_RE设置为1才有效
    GTM_TIM0_CH2_CTRL.B.FLT_CTR_RE   = IfxGtm_Tim_FilterCounter_upDown;                 // 需要FLT_MODE_RE设置为1才有效
    GTM_TIM0_CH2_FLT_FE.B.FLT_FE     = FILTER_CON_NUM;                                  // 设置滤波计数值
    GTM_TIM0_CH2_FLT_RE.B.FLT_RE     = FILTER_CON_NUM;

    GTM_TIM0_CH0_CTRL.B.TIM_MODE     = IfxGtm_Tim_Mode_inputEvent;                      // 输入事件
    GTM_TIM0_CH0_CTRL.B.ISL          = TRUE;                                            // 双边沿触发
    GTM_TIM0_CH0_CTRL.B.TIM_EN       = TRUE;                                            // 使能
    GTM_TIM0_CH1_CTRL.B.TIM_MODE     = IfxGtm_Tim_Mode_inputEvent;
    GTM_TIM0_CH1_CTRL.B.ISL          = TRUE;
    GTM_TIM0_CH1_CTRL.B.TIM_EN       = TRUE;
    GTM_TIM0_CH2_CTRL.B.TIM_MODE     = IfxGtm_Tim_Mode_inputEvent;
    GTM_TIM0_CH2_CTRL.B.ISL          = TRUE;
    GTM_TIM0_CH2_CTRL.B.TIM_EN       = TRUE;

    //FSOI引脚，主要用于快速关闭输出
    GTM_TIM1_CH6_CTRL.B.TIM_MODE     = IfxGtm_Tim_Mode_inputEvent;
    GTM_TIM1_CH6_CTRL.B.ISL          = TRUE;
    GTM_TIM1_CH6_CTRL.B.TIM_EN       = TRUE;
}


//三个通道捕获霍尔信号后将送入SPE进行评估







/*    SPE0模块中断配置*/

static void spe0_irq_init(void)
{
    IfxSrc_init(&SRC_GTMSPE0IRQ, SRC_CPU0, SRC_SPE_INT_PRIO);       // 设置SPE的中断优先级 以及处理中断的CPU
    IfxSrc_enable(&SRC_GTMSPE0IRQ);                                 // 中断使能

    GTM_SPE0_IRQ_EN.U   = SRC_ENABLE_ALL;                           // SPE模块中断使能
    GTM_SPE0_EIRQ_EN.U  = SRC_ENABLE_ALL;                           // 错误中断使能
    GTM_SPE0_IRQ_MODE.U = SRC_PULSE_NOTIFY;                         // 中断类型
}



/*    SPE0模块各通道初始化  */

static void spe0_cfg (void)
{
    spe0_irq_init();

    GTM_SPE0_PAT.B.IP0_VAL = VALID_IN_PATTERN;      // 设置模式有效
    GTM_SPE0_PAT.B.IP1_VAL = VALID_IN_PATTERN;
    GTM_SPE0_PAT.B.IP2_VAL = VALID_IN_PATTERN;
    GTM_SPE0_PAT.B.IP3_VAL = VALID_IN_PATTERN;
    GTM_SPE0_PAT.B.IP4_VAL = VALID_IN_PATTERN;
    GTM_SPE0_PAT.B.IP5_VAL = VALID_IN_PATTERN;
    GTM_SPE0_PAT.B.IP6_VAL = INVALID_IN_PATTERN;    // 设置模式无效
    GTM_SPE0_PAT.B.IP7_VAL = INVALID_IN_PATTERN;


    GTM_SPE0_PAT.B.IP0_PAT = HALL_HARDWARE_ORDER_0; // 设置TIMx y z输入的组合值  实际就是霍尔的数值
    GTM_SPE0_PAT.B.IP1_PAT = HALL_HARDWARE_ORDER_1;
    GTM_SPE0_PAT.B.IP2_PAT = HALL_HARDWARE_ORDER_2;
    GTM_SPE0_PAT.B.IP3_PAT = HALL_HARDWARE_ORDER_3;
    GTM_SPE0_PAT.B.IP4_PAT = HALL_HARDWARE_ORDER_4;
    GTM_SPE0_PAT.B.IP5_PAT = HALL_HARDWARE_ORDER_5;
    GTM_SPE0_PAT.B.IP6_PAT = 0;//无效值
    GTM_SPE0_PAT.B.IP7_PAT = 0;



    if(!motor->motor_speed.motor_dir==0)
    {
        GTM_SPE0_OUT_PAT0.U = MOTOR_FOREWARD_0;     // 正转换相表
        GTM_SPE0_OUT_PAT1.U = MOTOR_FOREWARD_1;
        GTM_SPE0_OUT_PAT2.U = MOTOR_FOREWARD_2;
        GTM_SPE0_OUT_PAT3.U = MOTOR_FOREWARD_3;
        GTM_SPE0_OUT_PAT4.U = MOTOR_FOREWARD_4;
        GTM_SPE0_OUT_PAT5.U = MOTOR_FOREWARD_5;
    }
    else
    {
        GTM_SPE0_OUT_PAT0.U = MOTOR_REVERSAL_0;     // 反转换相表
        GTM_SPE0_OUT_PAT1.U = MOTOR_REVERSAL_1;
        GTM_SPE0_OUT_PAT2.U = MOTOR_REVERSAL_2;
        GTM_SPE0_OUT_PAT3.U = MOTOR_REVERSAL_3;
        GTM_SPE0_OUT_PAT4.U = MOTOR_REVERSAL_4;
        GTM_SPE0_OUT_PAT5.U = MOTOR_REVERSAL_5;
    }
    GTM_SPE0_OUT_PAT6.U = 0x0;                      // 无效值
    GTM_SPE0_OUT_PAT7.U = 0x0;

    // 每换相10次触发中断，主要作用是避免通过一个换相时间去计算速度，从而导致计算的速度是波动的。
    // 速度波动是因为霍尔安装误差导致的换相时长是不一样的。
    GTM_SPE0_CMP.B.CMP = REV_CNT_VAL & 0xFFFFFF;
    spe0_pattern_init_cfg();

    GTM_SPE0_CTRL_STAT.B.SPE_EN   = TRUE;
    GTM_SPE0_CTRL_STAT.B.SIE0     = TRUE;                               // x通道使能
    GTM_SPE0_CTRL_STAT.B.SIE1     = TRUE;                               // y通道使能
    GTM_SPE0_CTRL_STAT.B.SIE2     = TRUE;                               // z通道使能
    GTM_SPE0_CTRL_STAT.B.FSOM     = 1;                                  // 快速关闭模式使能
    GTM_SPE0_CTRL_STAT.B.PDIR     = 0;
    GTM_SPE0_CTRL_STAT.B.ADIR     = 0;
    GTM_SPE0_CTRL_STAT.B.FSOL     = 0xFF;                               // 关闭的时候8个通道的输出值设定
    GTM_SPE0_CTRL_STAT.B.TRIG_SEL = 2;                                  // 使用TOM[i] CH1延时触发换相
    GTM_SPE0_CTRL_STAT.B.TIM_SEL  = 0;                                  // 使用TIM0 通道0-通道2作为SPE输入
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


//用于霍尔检测错误时
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



/*   SPE0-TOM模块间通道配置 */

static void spe0_tom_tgc_cfg (void)
{
    GTM_TOM0_TGC0_FUPD_CTRL.B.FUPD_CTRL0 = FORCE_UPDATE_EN  & 0x3;      // 强制更新
    GTM_TOM0_TGC0_FUPD_CTRL.B.FUPD_CTRL2 = FORCE_UPDATE_EN  & 0x3;
    GTM_TOM0_TGC0_FUPD_CTRL.B.FUPD_CTRL1 = FORCE_UPDATE_DIS & 0x3;      // 忽略
    GTM_TOM0_TGC0_FUPD_CTRL.B.FUPD_CTRL3 = FORCE_UPDATE_DIS & 0x3;
    GTM_TOM0_TGC0_FUPD_CTRL.B.FUPD_CTRL4 = FORCE_UPDATE_DIS & 0x3;
    GTM_TOM0_TGC0_FUPD_CTRL.B.FUPD_CTRL5 = FORCE_UPDATE_DIS & 0x3;
    GTM_TOM0_TGC0_FUPD_CTRL.B.FUPD_CTRL6 = FORCE_UPDATE_DIS & 0x3;
    GTM_TOM0_TGC0_FUPD_CTRL.B.FUPD_CTRL7 = FORCE_UPDATE_DIS & 0x3;

    GTM_TOM0_TGC0_OUTEN_CTRL.B.OUTEN_CTRL0 = TOM_OUTPUT_ENABLE & 0x3;   // 在更新触发的时候使能输出
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









/*   初始化TOM0输出模块   */

static void spe0_pwm_tom_cfg (void)
{
    spe0_tom_tgc_cfg ();

    GTM_TOM0_CH2_CTRL.B.OSM        = 1;                 // 开启关闭one shot mode
    GTM_TOM0_CH2_CTRL.B.SPEM       = 1;
    GTM_TOM0_CH2_CTRL.B.CLK_SRC_SR = TOM_CH_CMU_FXCLK0;
    GTM_TOM0_CH2_SR0.U = 10;
    GTM_TOM0_CH2_SR1.U = 10;                            // 设置延迟换相时间

    GTM_TOM0_CH0_CTRL.B.SL         = TOM_CH_SL_HIGH;    // 占空比表示的是高电平的占比
    GTM_TOM0_CH0_CTRL.B.OSM        = OSM_DIS;           // 关闭one shot mode
    GTM_TOM0_CH0_CTRL.B.SPEM       = SPEM_DIS;
    GTM_TOM0_CH0_CTRL.B.TRIGOUT    = TRIG_CCU0;
    GTM_TOM0_CH0_CTRL.B.CLK_SRC_SR = TOM_CH_CMU_FXCLK0;
    GTM_TOM0_CH0_CTRL.B.RST_CCU0   = CM0_MATCHING;

    /*****************速度设置*****************/


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

    GTM_TOM0_TGC0_GLB_CTRL.B.HOST_TRIG = 1;             // 最后开启TOM模块全局触发请求
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
    clock_cfg();                // GTM模块时钟配置初始化     ok
    ccm_cfg();                  // GTM中断刷新              ok
    hall_tim_input_cfg();       // 初始化霍尔捕获以及刹车捕获 ok
    spe0_cfg();                 // 初始化SPE0模块通道        ok
    spe0_pwm_tom_cfg();         // 初始化TOM0输出模块        ok
    spe0_pwm_mux_outport_cfg(); // 选择最终输出PWM的引脚     ok
}





/**************control*******************/



void motor_administer_init(void)
{
    motor=&motor_administer;
    motor->motor_speed.target_speed=1200;//rpm
    motor->motor_speed.current_speed=0;
    motor->motor_speed.last_speed=0;
    /***速度*****/
    motor->motor_speed.motor_duty_output=100;
    motor->motor_speed.last_motor_duty_output=100;
    motor->motor_speed.motor_period_output=5000;

    motor->motor_speed.max_duty=1200;
    /***方向*****/
    motor->motor_speed.motor_dir=0;//0反转，1正转
    /***********/
    motor->motor_speed.motor_brake=1;//刹车
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




void motor_set_pwm(MOTOR_Administer *MA)//输出占空比，周期不变
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
        GTM_SPE0_OUT_PAT0.U = MOTOR_FOREWARD_0;     // 正转换相表
        GTM_SPE0_OUT_PAT1.U = MOTOR_FOREWARD_1;
        GTM_SPE0_OUT_PAT2.U = MOTOR_FOREWARD_2;
        GTM_SPE0_OUT_PAT3.U = MOTOR_FOREWARD_3;
        GTM_SPE0_OUT_PAT4.U = MOTOR_FOREWARD_4;
        GTM_SPE0_OUT_PAT5.U = MOTOR_FOREWARD_5;
    }
    else if(!MA->motor_speed.motor_dir==1)
    {
        GTM_SPE0_OUT_PAT0.U = MOTOR_REVERSAL_0;     // 反转换相表
        GTM_SPE0_OUT_PAT1.U = MOTOR_REVERSAL_1;
        GTM_SPE0_OUT_PAT2.U = MOTOR_REVERSAL_2;
        GTM_SPE0_OUT_PAT3.U = MOTOR_REVERSAL_3;
        GTM_SPE0_OUT_PAT4.U = MOTOR_REVERSAL_4;
        GTM_SPE0_OUT_PAT5.U = MOTOR_REVERSAL_5;
    }
    if(MA->motor_speed.motor_brake)
    {
       GTM_SPE0_OUT_PAT0.U = MOTOR_BRAKE;          // 刹车
       GTM_SPE0_OUT_PAT1.U = MOTOR_BRAKE;
       GTM_SPE0_OUT_PAT2.U = MOTOR_BRAKE;
       GTM_SPE0_OUT_PAT3.U = MOTOR_BRAKE;
       GTM_SPE0_OUT_PAT4.U = MOTOR_BRAKE;
       GTM_SPE0_OUT_PAT5.U = MOTOR_BRAKE;
    }
    GTM_SPE0_CTRL_STAT.B.SPE_EN   = TRUE;
}





void motor_set_stop(MOTOR_Administer *MA,uint8 brake)//刹车(过流、低电压、人为设置、堵转等)刹车以后占空比为0，换相表为刹车状态。控制参数初始化。
{
    if(brake==1)
    {
        if( MA->motor_speed.motor_brake==1) return;

        MA->motor_speed.motor_brake=1;

        GTM_SPE0_CTRL_STAT.B.SPE_EN   = FALSE;

        GTM_SPE0_OUT_PAT0.U = MOTOR_BRAKE;          // 刹车
        GTM_SPE0_OUT_PAT1.U = MOTOR_BRAKE;
        GTM_SPE0_OUT_PAT2.U = MOTOR_BRAKE;
        GTM_SPE0_OUT_PAT3.U = MOTOR_BRAKE;
        GTM_SPE0_OUT_PAT4.U = MOTOR_BRAKE;
        GTM_SPE0_OUT_PAT5.U = MOTOR_BRAKE;

        GTM_SPE0_CTRL_STAT.B.SPE_EN   = TRUE;;


        //MA->motor_speed.target_speed=0;//rpm
        MA->motor_speed.current_speed=0;
        MA->motor_speed.last_speed=0;
        /***速度*****/
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
        MA->motor_speed.motor_brake=0;//清除刹车标志

        motor_set_dir(MA);//设置换相表

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




void motor_control_speed_output(MOTOR_Administer *MA)//pid控制器
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






void motor_set_pwm_ud(MOTOR_Administer *MA,int16 target_pwm)//如果说正反切无问题，可以替代motor_set_pwm()
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





void phase_change_time_check(MOTOR_Administer *MA)//堵转（在应该转的时候不转，会导致电流过大）
{
    static uint16 stop_times_count=0;
    if(motor->motor_speed.motor_brake==1) return;
    if(GTM_TOM0_CH0_SR1.U>400)//
    {
        if(motor->motor_speed.current_speed<20 && motor->motor_speed.current_speed>-20 )//超时
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





















