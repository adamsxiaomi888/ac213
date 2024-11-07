/**********************************************************************************
 *  项目编号:    AC213-04
 *  项目名称:    25低功耗
 *  客户名称:                                    
 *  程序编写:    符小玲
 *  启动日期:    2012-09-15
 *  完成日期:    
 *  版本号码:    V2.4
 *  功能描述:    增加电流、电压检测，PWM调速          
 *  芯片选型:    MSP430G2553IPW28R
 *  振荡频率:    内部 RC16.0M
 *  Watchdog:    Enable
 *  MAIN.C file
**********************************************************************************/
#include "Define.h"                                          /* 全局宏定义 ------*/
#include "Extern.h"                                          /* 全局变量及函数 --*/
/* 内部函数 ---------------------------------------------------------------------*/
void NormalMode     (void);
void SleepMode      (void);
void PowerOffMode   (void);
void PowerDownMode  (void);
/* 内部变量 ---------------------------------------------------------------------*/
WorkStatusManage const Work_Status_Array [WORK_STATUS_NUM] =
{
  NormalMode,
  SleepMode,
  PowerOffMode,
  PowerDownMode,
};
/**********************************************************************************
  函数名:  main
  功  能:  
  输  入:  空
  输  出:  空
  返  回:  空
  描  述： 
**********************************************************************************/
int main (void)
{
  Initialize();
  
  while(1)
  {
    if (Work_Status < WORK_STATUS_NUM)
    {
     (*Work_Status_Array[Work_Status])();
    }
  }
}
/**********************************************************************************
  函数名:  NormalMode
  功  能:  
**********************************************************************************/
void NormalMode (void)
{
  // 避开电机启动瞬间  AD采样
  if (((!MotorFlag.Motor_Run) && (!Flag.Del_Code_Finish) && (!Flag.Opretion_Finish)) || (motor_run_time >= NO_VOL_SAMPLING_TIME))
  {
    ADSamping();
    Electric_Quantity = (Current_Voltage-POWER_DOWN_VAULE)*100/(800-POWER_DOWN_VAULE);
    if (Current_Voltage < POWER_DOWN_VAULE+5) Electric_Quantity = 0;
    if (Current_Voltage > 800) Electric_Quantity = 100;
  }
  else
  {
    PowerDownSamping();
  }
  if((Old_Motor_Status != Motor_Status)&&(!Flag.Jiggle_Stop))                  //电机状态切换的时候最终发送百分比位置
  {
    u8 data[] = {Motor_Status, JourneyPercentCount(), 0x00, 0x00};
    UartUpload(0xA1, sizeof(data), data, 3, 0);
    Old_Motor_Status = Motor_Status;
  }
  /* RF解码 -------------------------------------------------------------------*/
  if (Flag.RF_Raise_Edge)                                    /* 是否有上升沿 ----*/
  {
    Flag.RF_Raise_Edge = NULL;
    if (RFSignalDecode(RF_High_Cnt,RF_Low_Cnt))              /* RF解码 ----------*/
    {
      if (RFDataVerify())                                    /* 数据是否正确 ----*/
      {
        RFDataDisposal();                                    /* RF数据处理 ------*/
      }
    }
  }
  if (Flag.Uart_rx_OK)
  {
     UartDataDisposal(); //20MS
     Flag.Uart_rx_OK = 0;
  }
  /* 2ms ----------------------------------------------------------------------*/
  if (Flag.Time_2ms)
  {
    Flag.Time_2ms = NULL;
    //WDTCTL = WDT_ARST_16;   
    
    TimeControl();
    if(!Flag.Goto_PCT)
      JourneyComper();                                         /* 比较行程 --------*/
    VoltageProtect();
    KeyManage();                                             /* 按键扫描及处理 --*/
    WorkTimeControl();                                      // 工作时间控制
    PowerDownDetect();
    
  }
  /* 10ms ---------------------------------------------------------------------*/
  if (Flag.Time_10ms)
  {
    Flag.Time_10ms = NULL;

    /* 计算转速 ---------------------------------------------------------------*/
    if (MotorFlag.Hall_Pulse_edge)
    {
      MotorFlag.Hall_Pulse_edge = NULL;
      CountMotorSpeed();                                   /* 计算出电机的转速 */
    }
    MotorControl();                                        /* 电机控制 --------*/
    MotorStatusSwitch();                                   /* 电机状态切换 ----*/
  }
  /* 50ms ---------------------------------------------------------------------*/
  if (Flag.Time_50ms)
  {
    Flag.Time_50ms = NULL;
    RFStatusInspect();
    SetLimitPoint();
    if (Flag.Delete_Code)                                  /* 是否删全码 ------*/
    {
      DeleteAllID(0xdd);
	  Limit_Point_Amount = 0;
	  MotorFlag.Journey_Set_OK = 0;
	  Hall_Pulse_Amount = 0;
    }             
  }
  /* 100ms --------------------------------------------------------------------*/
  if (Flag.Time_100ms)
  {
    Flag.Time_100ms = NULL;
    OprationCue();                                    // 提示
    DblclickTimeLimit();                              // 多限位点双击
  }
  /*******************************/
  if(Flag.Time_500ms)
  {
    Flag.Time_500ms = NULL;
    //Clear_WIFI_Hint();//ye
  }
  /* 1s -----------------------------------------------------------------------*/
  if (Flag.Time_1s)
  {
    Flag.Time_1s = NULL;
    if(Flag.Clear_status == 1)
    {
      Send_Clear_WIFI_Order();
      Clear_time++;
    }
    if(Clear_time == 3)
    {
      Flag.Clear_status = 0;
      Clear_time= 0;
    }
    if (!Flag.Electrify_Reset)
    {
      if (++PowerDown_Delay_Time >= 2)
      {
        Flag.Electrify_Reset = TRUE;
        PowerDown_Delay_Time = NULL;
      }
    }
    LearnDelCodeTime();
    Clear_WIFI_Hint();//ye
    
    if( Flag.net_successful_count)
    NET_Succe_Time_Cnt++ ;
    else
    NET_Succe_Time_Cnt = 0;  
    
    if(NET_Succe_Time_Cnt> 36)   //没有联网成功36秒后进入低功耗
    {
    Flag.net_successful_count = 0;    //清计时标志
    NET_Succe_Time_Cnt = 0;             //计时器等于0
    Clear_Status_Time_Cnt = 0;      //进入低功耗
    }
      
  }
}
/**********************************************************************************
  函数名:  SleepMode
  功  能:  
**********************************************************************************/
void SleepMode (void)
{
  /* RF解码 -------------------------------------------------------------------*/
  if (Flag.RF_Raise_Edge)                                    /* 是否有上升沿 ----*/
  {
    Flag.RF_Raise_Edge = NULL;
    if (RFSignalDecode(RF_High_Cnt,RF_Low_Cnt))              /* RF解码 ----------*/
    {
      if (RFDataVerify())                                    /* 数据是否正确 ----*/
      {
        RFWakeup();
      }
    }
  }
  /* 2ms ----------------------------------------------------------------------*/
  if (Flag.Time_2ms)
  {
    Flag.Time_2ms = NULL;
    PowerDownDetect();
    SleepControl();                                        // 睡眠控制
    
//    if(!Clear_Status_Time_Cnt)
//    {
//          LED_BLUE_OFF();
//      Flag.Clear_WIFI_Time_End = 1;
//      Clear_Status_End_Time_Cnt = 2;
//      Flag.Clear_status_hint = NULL;
 
    
    Clear_WIFI_Hint();                //LIU防止配网时立刻进入低功耗
    
    
  }
  
  
}
/**********************************************************************************
  函数名:  PowerOffMode
  功  能:  
**********************************************************************************/
void PowerOffMode  (void)
{
  /* 2ms ----------------------------------------------------------------------*/
  if (Flag.Time_2ms)
  {
    Flag.Time_2ms = NULL;
    PowerDownDetect();
    PowerOffControl();                                       // 睡眠控制
    KeyManage();                                             // 按键扫描及处理
  }
}
/**********************************************************************************
  函数名:  PowerDownMode
  功  能:  
**********************************************************************************/
void PowerDownMode  (void)
{
  VolSamping();
  /* 2ms ----------------------------------------------------------------------*/
  if (Flag.Time_2ms)
  {
    Flag.Time_2ms = NULL;
    if(CHARGE_DETECT())//插上充电器         
    {
      LED_RED_ON();//红灯亮
      LED_BLUE_OFF();//蓝灯灭  
      if(CHARGE_STATUS())//充满电
      {
          LED_RED_OFF();//红灯灭
          LED_BLUE_ON();//蓝灯亮
      }
    }
    else//拔掉充电器
    {
      LED_BLUE_OFF();//蓝灯灭  
      LED_RED_OFF();//红灯灭
    }
    PowerDownDetect();
    PowerDownHandle();                                     // 掉电处理
  }
}
/**********************************************************************************
  函数名:  LowPowerSet
  功  能:  关机后TX和RX的IO口配置
**********************************************************************************/
void LowPowerSet(void)
{
    P1SEL = 0x00;                           //  a4 a6
    P1SEL2 =0x00; 
    P1REN |= 0x06;
    P1OUT |= 0x06; //P11和P12拉高
    P1OUT &= 0xfb; //P12拉低
}
/**********************************************************************************
  函数名:  WorkModeSetting
  功  能:  
**********************************************************************************/
void FallPower(void)
{
//  Flag.Power_on = 0;
//  Time_Power_On = 0;
    RF_POWER_OFF();                     // 关闭RF模块电源
    POWEER_DETECT_OFF();               // 关电源输入电压检测
    MotorStop();     
    BUZZER_OFF();
    RF_SLEEP();
}

/**********************************************************************************
  函数名:  WorkModeSetting
  功  能:  
**********************************************************************************/
void WorkModeSetting  (Work_Status_TypeDef mode)
{
  if (mode == NORMAL_MODE)                      // 是否正常模式
  {
    GPIOSetting();
    CLKSetting();                               // 时钟设置 
    TIMXSetting();                              // 定时器设置
    ADCSetting();                               // ADC 设置
    LinUartInit();
    
    RF_POWER_ON();                              // 开RF模块电源
    POWEER_DETECT_ON();                         // 开电源输入电压检测
    Flag.Power_Down = 0;
    
    
    _DINT();                            // 关总中断 
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;
    
    Work_Status = NORMAL_MODE;                  // 进入正常模式
    
    P2IFG = 0;
    P2IES = BIT6;                               // P2.6 下降沿中断
    P2IE  = BIT6;                               // 开P2.6(断电检测)中断，关按键、RF_DATA中断
    P2IFG = 0;

    INT_CHARGE(INT_CLOSE);//P1中断设置,只针对中断使能
	
    BCSCTL2 = 0;                                // 频率恢复16M
    
     // Time0_A0 设置
    TA0CCTL0 = CCIE;                             // CCR0 interrupt enabled
    TA0CCR0 += RF_DECODE_TIME * SYSTEM_CLOCK;    // 40 uS
    
    // Time0_A1 设置
    TA0CCTL1 = CCIE;                             // CCR1 interrupt enabled
    TA0CCR1 += SYSTEM_TIME_VALUE * SYSTEM_CLOCK;     // 2mS
    TA0CTL = TASSEL_2 + MC_2;                    // SMCLK, Continous up
    
    if (Flag.RF_Wakeup)
    {
      Flag.RF_Wakeup = 0;
      RFDataDisposal();                           // RF数据处理
    }
    _EINT();                                   // 开总中断 
  }
  else if (mode == SLEEP_MODE)        // 睡眠模式
  {
    /*RF_POWER_ON();                     // 开RF模块电源
    BRAKE_ON();                        // 刹车，断开继电器
    POWEER_DETECT_OFF();               // 关闭电源输入电压检测
    MotorStop();                       // 马达停止
    RF_SLEEP();
    
    Flag.Power_Down = 0;
    BUZZER_OFF();                      // 关蜂鸣器
    */
    FallPower();
    RF_POWER_ON(); 
    BRAKE_ON(); 
    Flag.Power_Down = 0;
    
    // 停止ADC采样，关闭ADC模块
    P1SEL = 0x00;                           //  a4 a6
    ADC10CTL0 &= SREF_1 + ADC10SHT_2 + REFON + ADC10ON + REF2_5V;                     //  a4 a6
    ADC10AE0 &= 0x50;
    ADC10CTL0 &= ENC + ADC10SC;             // Sampling and conversion start
    
    _DINT();                            // 关总中断 
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;
    
    Work_Status = SLEEP_MODE;          // 进入睡眠模式
    P1SEL = 0x00;                      // No 
    P2SEL = 0x00;                      // No XTAL 
    P3SEL = 0x00;                      // No XTAL 
    P2IFG = 0;
    Goto_Sleep_Cnt = 0;
    
    BCSCTL2 = 0x24;                    // 8M 4分频  主频2M
    FCTL2 = FWKEY + FSSEL0 + FN4;      // MCLK/3 for Flash Timing Generator
    
    // Time0_A0 设置
    TA0CCTL0 = CCIE;                           // CCR0 interrupt enabled
    TA0CCR0 += (RF_DECODE_TIME * SYSTEM_CLOCK) >> 2;  // 40 uS

    // Time0_A1 设置
    TA0CCTL1 = CCIE;                           // CCR1 interrupt enabled
    TA0CCR1 += (SYSTEM_TIME_VALUE * SYSTEM_CLOCK) >> 2;     // 2mS
    TA0CTL = TASSEL_2 + MC_2;                  // SMCLK, Continous up
   
    LinUartInit();  
    UCA0BR0 = (((1000000/(19200)) * (SYSTEM_CLOCK>>2))) & 0xFF;         // 波特率设置
  	UCA0BR1 = (((1000000/(19200)) * (SYSTEM_CLOCK>>2)) >> 8); 
   
    P2IES = BIT3 + BIT7 + BIT6;        // Bit = 0: The PxIFGx flag is set with a low-to-high transition
    P2IE  = BIT3 + BIT7 + BIT6;        // 开P2.3  P2.5  P2.6 中断
    
     P2IFG = 0;
	 
 
    
     P1IFG = 0;
    _EINT();                                   // 开总中断 
  }
  else if (mode == POWER_OFF_MODE)    /* 关机模式 --------*/
  {
    u16 i;
    
  	IE2 &= ~(UCA0RXIE + UCA0TXIE);                         // Enable USCI_A0 RX interrupt
	/*	
    RF_POWER_OFF();                     // 关RF模块电源
    BRAKE_ON();                        // 刹车，断开继电器
    POWEER_DETECT_OFF();               // 关闭电源输入电压检测
    MotorStop();                       // 马达停止
    BUZZER_OFF();                      // 关蜂鸣器
    RF_SLEEP();
    */
    FallPower();
    BRAKE_ON(); 
    MotorFlag.Motor_Run = 0;
    Motor_Status = MOTOR_STOP;
    Motor_Status_Buffer = MOTOR_STOP;
    
    Flag.Set_Limit_Point = NULL;
    Flag.Delete_Limit_Point = NULL;
    Flag.Set_Journey_Direction = NULL;
    Flag.Save_Limit_Point = NULL;
    Flag.Del_Code_Finish = NULL;
    Flag.Opretion_Finish = NULL;
    Opretion_Event = NULL;

     // 停止ADC采样，关闭ADC模块
    LowPowerSet();
    ADC10CTL0 &= SREF_1 + ADC10SHT_2 + REFON + ADC10ON + REF2_5V;                     //  a4 a6
    ADC10AE0 &= 0x50;
    ADC10CTL0 &= ENC + ADC10SC;             // Sampling and conversion start
    

    _DINT();                            // 关总中断 
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;
    
    Work_Status = POWER_OFF_MODE;
    
    //P1SEL = 0x00;                      // No 
    P2SEL = 0x00;                      // No XTAL 

    P3SEL = 0x00;                      // No XTAL 
    P2IFG = 0;
    Goto_Sleep_Cnt = 0;
    
    BCSCTL2 = 0x24;                    // 8M 4分频  主频2M
    FCTL2 = FWKEY + FSSEL0 + FN4;      // MCLK/3 for Flash Timing Generator
    
    // Time0_A0 设置
    TA0CCTL0 = CCIE;              // CCR0 interrupt enabled
    TA0CCR0 += (RF_DECODE_TIME * SYSTEM_CLOCK) >> 2;  // 40 uS

    // Time0_A1 设置
    TA0CCTL1 = CCIE;             // CCR1 interrupt enabled
    TA0CCR1 += (SYSTEM_TIME_VALUE * SYSTEM_CLOCK) >> 2;     // 2mS
    TA0CTL = TASSEL_2 + MC_2;   // SMCLK, Continous up
    
    P2IES = BIT7 + BIT6;        // Bit = 0: The PxIFGx flag is set with a low-to-high transition
    P2IE  = BIT7 + BIT6;        // 开P2.3  P2.5  P2.6 中断
    P2IFG = 0;
    
    _EINT();                                   // 开总中断 
    Flag.Set_Key_ON = 0;
    
     for (i = 0; i < 3; i++)
    {
      LED_RED_ON();
      Delay(330000);                       // 延迟          
      LED_RED_OFF();      
      Delay(330000);                       // 延迟
    }
	
	_NOP();
  }
  else if (mode == POWER_DOWN_MODE)    /* 掉电模式 --------*/
  {
    //u8 i;
    /*
    BUZZER_OFF();
    MotorStop();                        // 马达停止
    RF_POWER_OFF();                     // 关闭RF模块电源
    RF_SLEEP();
    POWEER_DETECT_OFF();               // 关电源输入电压检测
    */
    FallPower();
    
    MotorFlag.Motor_Run = 0;
    Motor_Status = MOTOR_STOP;
    Motor_Status_Buffer = MOTOR_STOP;
    
    Flag.Electrify_Reset = NULL;
    Flag.Set_Limit_Point = NULL;
    Flag.Delete_Limit_Point = NULL;
    Flag.Set_Journey_Direction = NULL;
    Flag.Save_Limit_Point = NULL;
    Flag.Del_Code_Finish = NULL;
    Flag.Opretion_Finish = NULL;
    Opretion_Event = NULL;
    
    Joint_Action_Time_Cnt = 0;
    Joint_Action_Motor_Buf = MOTOR_STOP;
    Flag.Joint_Action = NULL;
    /*
    for (i = 0; i < (MID_LIMIT_POINT_AMOUNT + 2); i++)
    {
      Limit_Point[i].Flag.F1.Fine_Adjust = NULL;
    }
    */
     Limit_Point[0].Flag.F1.Fine_Adjust = NULL;
     Limit_Point[1].Flag.F1.Fine_Adjust = NULL;
     Limit_Point[2].Flag.F1.Fine_Adjust = NULL;
     Limit_Point[3].Flag.F1.Fine_Adjust = NULL;
     Limit_Point[4].Flag.F1.Fine_Adjust = NULL;
     Limit_Point[5].Flag.F1.Fine_Adjust = NULL;
    // 停止ADC采样，关闭ADC模块
    P1SEL = 0x00;                           //  a4 a6
    ADC10CTL0 &= SREF_1 + ADC10SHT_2 + REFON + ADC10ON + REF2_5V;                     //  a4 a6
    ADC10AE0 &= 0x50;
    ADC10CTL0 &= ENC + ADC10SC;             // Sampling and conversion start
    
    
    _DINT();                            // 关总中断 
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;
    
    BCSCTL2 = 0x24;                    // 8M 4分频
    FCTL2 = FWKEY + FSSEL0 + FN4;      // MCLK/3 for Flash Timing Generator
    
    P2IES = 0;                         // Bit = 0: The PxIFGx flag is set with a low-to-high transition
    P2IE  = 0;                         // 关P2.3  P2.6  P2.7中断
    P2IFG = 0;
    
    GPIOSetting();
    LowPowerSet();
    // Time0_A0 设置
    TA0CCTL0 = CCIE;                           // CCR0 interrupt enabled
    TA0CCR0 += RF_DECODE_TIME * SYSTEM_CLOCK;  // 

    // Time0_A1 设置
    TA0CCTL1 = CCIE;                           // CCR1 interrupt enabled
    TA0CCR1 += (SYSTEM_TIME_VALUE * SYSTEM_CLOCK) >> 2;     // 2mS
    TA0CTL = TASSEL_2 + MC_2;                  // SMCLK, Continous up
    IE2 &= ~(UCA0RXIE + UCA0TXIE);
    Work_Status = POWER_DOWN_MODE;
    _EINT();                                   // 开总中断 
    
    PowerDown_Time_Cnt = 0;
  }
}