/**********************************************************************************
 *  项目编号:    AC213-04
 *  项目名称:    25低功耗
 *  客户名称:                                    
 *  程序编写:    符小玲
 *  启动日期:    2012-09-15
 *  完成日期:    
 *  版本号码:    V2.4
 
 *  Initialize.C file
 *  功能描述: 系统初始化
 *********************************************************************************/
#include "Define.h"                                          /* 全局宏定义 ------*/
#include "Extern.h"                                          /* 全局变量及函数 --*/
#include <string.h>
/**********************************************************************************
  函数名:   Delay
  功  能:    
*********************************************************************************/
void Delay (register u32 i)
{
  for (; i > 0; i--)
  {
    //WDTCTL = WDT_ARST_16;
  }
}
/*********************************************************************************
  函数名:   CLKSetting
  功  能:    
*********************************************************************************/
void CLKSetting (void)
{
  BCSCTL3 |= LFXT1S_2;                       // ACLK = VLO 12kHz
  
  #if (SYSTEM_CLOCK == 1)
    BCSCTL1 = CALBC1_1MHZ;                   // Set range
    DCOCTL = CALDCO_1MHZ;                    // Set DCO step + modulation */
    FCTL2 = FWKEY + FSSEL0 + FN1;            // MCLK/3 for Flash Timing Generator
    BCSCTL2 = 0;
  #elif (SYSTEM_CLOCK == 2)
    BCSCTL1 = CALBC1_8MHZ;                   // Set range
    DCOCTL = CALDCO_8MHZ;                    // Set DCO step + modulation */
    FCTL2 = FWKEY + FSSEL0 + FN2;            // MCLK/3 for Flash Timing Generator
    BCSCTL2 = 0x24;
  #elif (SYSTEM_CLOCK == 4)
    BCSCTL1 = CALBC1_8MHZ;                   // Set range
    DCOCTL = CALDCO_8MHZ;                    // Set DCO step + modulation */
    FCTL2 = FWKEY + FSSEL0 + FN4;            // MCLK/3 for Flash Timing Generator
    BCSCTL2 = 0x12;
  #elif (SYSTEM_CLOCK == 8)
    BCSCTL1 = CALBC1_8MHZ;                   // Set range
    DCOCTL = CALDCO_8MHZ;                    // Set DCO step + modulation */
    FCTL2 = FWKEY + FSSEL0 + FN4 + FN1;      // MCLK/3 for Flash Timing Generator
    BCSCTL2 = 0;
  #elif (SYSTEM_CLOCK == 12)
    BCSCTL1 = CALBC1_12MHZ;                  // Set range
    DCOCTL = CALDCO_12MHZ;                   // Set DCO step + modulation */
    FCTL2 = FWKEY + FSSEL0 + FN5;            // MCLK/3 for Flash Timing Generator
    BCSCTL2 = 0;
  #elif (SYSTEM_CLOCK == 16)
    BCSCTL1 = CALBC1_16MHZ;                  // Set range
    DCOCTL = CALDCO_16MHZ;                   // Set DCO step + modulation */
    FCTL2 = FWKEY + FSSEL0 + FN5 + FN4;      // MCLK/3 for Flash Timing Generator
    BCSCTL2 = 0;
  #else
    #error "Not define system clock!"
  #endif
}
/*********************************************************************************
  函数名:   TIMXSetting
  功  能:    
*********************************************************************************/
void TIMXSetting (void)
{
  TA0CTL = TACLR;       // 定时器重置
  
  // Time0_A1 设置
  P2SEL |= BIT2 + BIT5;                      // P2.2 and P2.5 TA1/2 options
  P3SEL |= BIT2 + BIT3;                      // P3.2 and P3.3 TA1/2 options
  
  TA1CCTL0 = 0;                              // CCR0 interrupt enabled
  TA1CCR0 = MAX_DUTY + 2;                    // CCR1 PWM duty cycle 2:100
  TA1CCTL1 = OUTMOD_2;                       // CCR1 toggle/reset
  TA1CCTL2 = OUTMOD_2;                       // CCR2 toggle/reset
  MotorStop();
  TA1CTL = TASSEL_2 + ID_0 + MC_3;           // SMCLK, 不分频 up down mode
}
/*********************************************************************************
  函数名:   GPIOSetting
  功  能:    
*********************************************************************************/
void GPIOSetting (void)
{
  P1DIR = 0x87;                      // 0:in 1:out
  P2DIR = 0x34;                      // 0:in 1:out
  P3DIR = 0xff;                      // 0:in 1:out
  
  P1REN &= 0xd9;
  P1OUT &= 0xf1;
    
  P1SEL = 0x00;                      // No 
  P2SEL = 0x00;                      // No XTAL 
  P3SEL = 0x00;                      // No XTAL 
  
  //P3REN &= 0xa0;//ye
  
  P1IFG = 0;
  P2IFG = 0;
}
/*********************************************************************************
  函数名:   VariableInitialize
  功  能:    
*********************************************************************************/
void VariableInitialize (void)
{
  
  /*for (i = 0; i < (MID_LIMIT_POINT_AMOUNT + 2); i++)
  {
    Limit_Point[i].Limit_Place = 0;
    Limit_Point[i].Flag.F8 = 0;
  }*/
  memset(&Limit_Point,0,sizeof(Limit_Point));
  Hall_Pulse_Amount_Cnt  = MIN_JOURNEY_VALUE;
  Hall_Pulse_Width       = 0;
  Hall_Status_Buffer     = 0;
  hall_change_cnt        = 0;
  Hall_Time_Cnt          = 0;
  Current_Voltage        = 0;
  Power_Down_Voltage     = 0;
  PowerDown_Time_Cnt     = 0;
  Power_Down_Cnt         = 0;
  Limit_Point_Amount     = 0;
  Joint_Time_Level       = 4; 
    
  MotorFlag.Motor_Direction           = 0;                          /* 电机方向标志 ----*/
  MotorFlag.Motor_Fact_Direction      = 0;                          /* 电机实际方向标志 */
  MotorFlag.Motor_Run                 = 0;                          /* 电机运行标志 ----*/
  MotorFlag.Hall_Pulse_edge           = 0;                          /* 霍尔脉冲边沿标志 */
  MotorFlag.Direction                 = 0;                          /* 方向标志 --------*/
  //MotorFlag.Single_Journey            = 0;                          /* 单个行程标志 ----*/
//  MotorFlag.Journey_Set_OK            = 0;                          /* 行程设定OK标志 --*/
  MotorFlag.Journey_Direction         = 0;                          /* 行程方向标志 ----*/
  MotorFlag.EE_Save                   = 0;                          /* 上电标志 --------*/
  MotorFlag.Hall_Power                = 0;                          /* 霍尔电源标志 ----*/

}
/*********************************************************************************
  函数名:   Initialize
  功  能:    
  输  入:   空
  输  出:   空    
  返  回:   空
  描  述：
*********************************************************************************/
void Initialize (void)
{
  #define MOTOR_RUN                 (200 / (TIME_10MS_VALUE * SYSTEM_TIME))
  
  _DINT();                            // 关总中断 
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  LED_RED_ON();   
  LED_BLUE_ON();

  IN1_H_0();
  IN1_L_0();
  IN2_H_0();
  IN2_L_0();                          // 关闭马达驱动
  
  BRAKE_ON();                         // 刹车，断开继电器
  RF_POWER_OFF();                     // 关闭RF模块电源
  RF_SLEEP();
  BUZZER_OFF();                       // 蜂鸣器关闭
  POWEER_DETECT_OFF();                // 关闭电源输入电压检测
  
  CLKSetting();                       // 时钟设置 
  GPIOSetting();                      // IO 口设置
  
  
  HALL_POWEER_ON();                   // 开霍尔电源
  RF_POWER_ON();                      // 开RF模块电源
  POWEER_DETECT_ON();                 // 开电源输入电压检测

    // 是否上电复位
  if(IFG1 & PORIFG)
  {
    Delay(60000);                     // 上电延迟
    
    ADCSetting();                       // ADC 设置
    VariableInitialize();
    
    FlashReadData();
    Flag.Learn_Key_Hint = TRUE;         // 蜂鸣器响一声提示开机
    Limit_Point_Amount = LimitCollator(Limit_Point);
  }
  else
  {
    if ((MotorFlag.EE_Save) && (MotorFlag.Motor_Run))
    {
      u8 temp = 1;
      
      if (motor_run_time >= (200 / (TIME_10MS_VALUE * SYSTEM_TIME)))
      {
        temp = 5;
      }
      else if (motor_run_time >= (100 / (TIME_10MS_VALUE * SYSTEM_TIME)))
      {
        temp = 2;
      }
      
      if (MotorFlag.Motor_Fact_Direction ^ MotorFlag.Journey_Direction)
      {
        Hall_Pulse_Amount_Cnt += temp;
      }
      else
      {
        Hall_Pulse_Amount_Cnt -= temp;
      }
      PowerDownSaveData();
    }
  }
  
  TIMXSetting();                      // 定时器设置
  
  WorkModeSetting(NORMAL_MODE);       // 进入工作模式
  MotorFlag.Motor_Run = NULL;
  Motor_Status        = MOTOR_STOP;
  Motor_Status_Buffer = MOTOR_STOP;
  
  MotorFlag.Hall_Power = TRUE;
  

  LinUartInit();
  /*
  for(u8 i=0;i<255;i++)
  Delay(60000);                     // 上电延迟
  */
  Delay(60000);                     // 上电延迟
 // Send_Pair_WIFI_Order();
  IFG1 = 0;
  _EINT();                            // 开总中断 

  //u8 data[] = {0x02};
  //UartUpload(0xe1, sizeof(data), data, 3, 0);
}

