/**********************************************************************************
 *  项目编号:    AC213-04
 *  项目名称:    25低功耗
 *  客户名称:                                    
 *  程序编写:    符小玲
 *  启动日期:    2012-09-15
 *  完成日期:    
 *  版本号码:    V2.4
 *
 *  .c file
 *  功能描述: 
**********************************************************************************/
#include "Define.h"                                          /* 全局宏定义 ------*/
#include "Extern.h"                                          /* 全局变量及函数 --*/
/* 内部宏 -----------------------------------------------------------------------*/
#define TIME0_INT_NUMBER       3                             // Time0中断次数

#define HALL1                  0
#define HALL2                  1
#define HALL_SR1               BIT0
#define HALL_SR2               BIT1
#define HALL1_IN()            (P2IN & HALL_SR1)              /* P2.0 ------------*/
#define HALL2_IN()            (P2IN & HALL_SR2)              /* P2.1 ------------*/
/* 内部变量 ---------------------------------------------------------------------*/
static u8 Time2_Int_Cnt = TIME0_INT_NUMBER - 2;
static u8 RF_Signal_Sample1;                                 /* 数据电平采样 ----*/
static u8 RF_Signal_Sample2;                                 /* 数据电平采样 ----*/

u8 High_Time_Cnt;                                /* 数据时间缓存 ----*/
u8 Low_Time_Cnt;                                 /* 数据时间缓存 ----*/

u8 RF_High_Cnt_Buffer;                                /* 数据时间缓存 ----*/
u8 RF_Low_Cnt_Buffer;                                 /* 数据时间缓存 ----*/


void INT_CHARGE(u8 int_enable)//充电中断设置
{
	P1IE = int_enable;//使能  
	P1IES = CHARGE_ON;//0是_|-;1是-|_;P13充电器拔插判断中断，插上是高电平，拔掉低电平。
	if(CHARGE_DETECT())//充电器插上
	{
		P1IES = CHARGE_OFF;
	}
	//YE P1IES |= (1<<CHARGE_DET);//P15中断0-1，只是充满电才启动中断。充电中低电平，充满电高电平
	P1IFG = 0;
  
 
}

/*********************************************************************************
  函数名:   DetectHallSignal
  功  能:   检测霍尔信号
  输  入:   霍尔号
  输  出:   霍尔脉冲宽度，边沿标志，电机实际运行方向标志
*********************************************************************************/
void DetectHallSignal (void)
{
  u8 hall_value;

  hall_value = P2IN & HALL_SR1;
  
  if (Hall_Time_Cnt < 65000)
  {
    Hall_Time_Cnt++;
  }
  
  if (Hall_Status_Buffer != hall_value)
  {
    if (++hall_change_cnt >= 3)
    {
      hall_change_cnt = 0;
      
      MotorFlag.Motor_Fact_Direction = 0;
      if (hall_value != 0)   // 0 -> 1
      {
        if (!(P2IN & HALL_SR2))
        {
          MotorFlag.Motor_Fact_Direction = TRUE;
        }
      }
      else   // 1 -> 0
      {
        if (P2IN & HALL_SR2)
        {
          MotorFlag.Motor_Fact_Direction = TRUE;
        }
      }
      

      
      Hall_Pulse_Width = Hall_Time_Cnt;
      Hall_Time_Cnt = 0;
      MotorFlag.Hall_Pulse_edge = TRUE;
      
      MotorFlag.EE_Save = TRUE;
      if (MotorFlag.Motor_Fact_Direction ^ MotorFlag.Journey_Direction)
      {
        // 是否在最大范围内
        if (Hall_Pulse_Amount_Cnt < HALL_PULSE_MAX)
        {
          ++Hall_Pulse_Amount_Cnt;
          //test();
        }
      }
      else
      {
        // 是否在最大范围内
        if (Hall_Pulse_Amount_Cnt > HALL_PULSE_MIN)
        {
          Hall_Pulse_Amount_Cnt--;
          //test();
        }
      }
      Hall_Status_Buffer = hall_value;
    }
  }
}
/**********************************************************************************
  函数名:  Tim2Interrupt
  功  能:  Tim2中断函
  输  入:  空
  输  出:  空    
  返  回:  空
  描  述：   
**********************************************************************************/
// Timer_A3 Interrupt Vector (TA0IV) handler
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0(void)
{
  //test_1();
  if (Work_Status == NORMAL_MODE)            /* 正常模式 --------*/   
  {
    TA0CCR0 += RF_DECODE_TIME * SYSTEM_CLOCK;   // 40 uS
    
    if ((RF_SIGNAL_SAMPLE()))                                  /* 第一次采样 ------*/
    {
      RF_Signal_Sample1++;
    }
    /*-----------------------------------------------------------------------------*/
    if (MotorFlag.Hall_Power)
    {
      DetectHallSignal();
    }
    /*-----------------------------------------------------------------------------*/
    if ((RF_SIGNAL_SAMPLE()))                                  /* 第二次采样 ------*/
    {
      RF_Signal_Sample1++;
    }
    /*-----------------------------------------------------------------------------*/

      // 是否要响蜂鸣器
    if(! CHARGE_DETECT())  //ye
    {
       if (Flag.Buzzer_Hint)
        {
          static u8 cycle;
          
          if (++cycle >= 5)//6
          {
            cycle = NULL;
          }
          else if (cycle > 3)//4
          {
            LED_RED_OFF();  //YE
          }
          else
          {
            LED_RED_ON();
          }
        }
        else
        {
         LED_RED_OFF();    
        }
    }
     
    /*-----------------------------------------------------------------------------*/
    if ((RF_SIGNAL_SAMPLE()))                                  /* 第三次采样 ------*/
    {
      RF_Signal_Sample1++;
    }
    
    if (RF_Signal_Sample1 >> 1)                                // 是否两次为高 ----
    {
      RF_Signal_Sample2++;
      Low_Time_Cnt = 0;
      if (++High_Time_Cnt >= 3)
      {
        if (!Flag_RF.Signal_Status)
        {
          Flag.RF_Raise_Edge = TRUE;                           // 置RF上升沿标志  
          RF_High_Cnt = RF_High_Cnt_Buffer;
          RF_Low_Cnt = RF_Low_Cnt_Buffer;
          RF_High_Cnt_Buffer = NULL;
          RF_Low_Cnt_Buffer = NULL;
        }
        Flag_RF.Signal_Status = TRUE;
      }
    }
    else
    {
      if (++Low_Time_Cnt >= 3)
      {
        Flag_RF.Signal_Status = NULL;
      }
      High_Time_Cnt = 0;
    }
    RF_Signal_Sample1 = 0;
    
    
    if (--Time2_Int_Cnt == NULL)
    {
      Time2_Int_Cnt = TIME0_INT_NUMBER;
      if (RF_Signal_Sample2 >> 1)
      {
        RF_High_Cnt_Buffer++;
      }
      else
      {
        RF_Low_Cnt_Buffer++;
      }
      RF_Signal_Sample2 = NULL;
    }
  }
  else if (Work_Status == POWER_DOWN_MODE)            /* 掉电模式 --------*/   
  {
    //test();
    if (Flag.Power_Down)
    {
      TA0CCR0 += (RF_DECODE_TIME * SYSTEM_CLOCK) >> 1;  // 40 uS
    }
    else
    {
      TA0CCR0 += RF_DECODE_TIME * SYSTEM_CLOCK;  // 40 uS
    }
    
    if (MotorFlag.Hall_Power)
    {
      DetectHallSignal();
    }
  }
  else if (Work_Status == SLEEP_MODE)            /* 睡眠模式 --------*/
  {
    TA0CCR0 += (RF_DECODE_TIME * SYSTEM_CLOCK) >> 2;  // 40 uS
    
    //test();
    if ((RF_SIGNAL_SAMPLE()))
    {
      RF_Signal_Sample2++;
      Low_Time_Cnt = 0;
      if (++High_Time_Cnt >= 3)
      {
        if (!Flag_RF.Signal_Status)
        {
          Flag.RF_Raise_Edge = TRUE;                           // 置RF上升沿标志  
          RF_High_Cnt = RF_High_Cnt_Buffer;
          RF_Low_Cnt = RF_Low_Cnt_Buffer;
          RF_High_Cnt_Buffer = NULL;
          RF_Low_Cnt_Buffer = NULL;
        }
        Flag_RF.Signal_Status = TRUE;
      }
    }
    else
    {
      if (++Low_Time_Cnt >= 3)
      {
        Flag_RF.Signal_Status = NULL;
      }
      High_Time_Cnt = 0;
    }
    
    if (--Time2_Int_Cnt == NULL)
    {
      Time2_Int_Cnt = TIME0_INT_NUMBER;
      if (RF_Signal_Sample2 >> 1)
      {
        RF_High_Cnt_Buffer++;
      }
      else
      {
        RF_Low_Cnt_Buffer++;
      }
      RF_Signal_Sample2 = NULL;
    }
  }
    
} //中断结束并返回
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    _BIC_SR_IRQ(LPM3_bits);   
    if (Work_Status == SLEEP_MODE)
    {
      WorkModeSetting(NORMAL_MODE);  // 进入工作模式
      Flag.Key_Wakeup = TRUE;
      Key_Wakeup_time_cnt = 0;
    }
	//充上电，从掉电模式检查电量再到正常模式
	//关机模式一直再关机模式
    P1IFG = 0;
}
/**********************************************************************************
  函数名:  Port_2
  功  能:  中断函
**********************************************************************************/
// Port 1 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
  _BIC_SR_IRQ(LPM3_bits);                       // Clear LPM3 bits from 0(SR)
  
  if (P2IFG & BIT6)  //断电
  {
    Goto_Sleep_Cnt = 0;
  }
  
  if (P2IFG & BIT7)    // 按键
  {
    if (Work_Status == SLEEP_MODE)
    {
      WorkModeSetting(NORMAL_MODE);  // 进入工作模式
      Flag.Key_Wakeup = TRUE;
      Key_Wakeup_time_cnt = 0;
    }
  }
  
  if (P2IFG & BIT3)  // RF
  {
   // RF_WORK();                                  // RF 进入一直工作模式
  }
  
  P2IFG = 0;
}
/**********************************************************************************
  函数名:  
  功能:    
**********************************************************************************/
void WorkTimeControl (void)
{
  #define MOTOR_STOP_TIME        (2000  / (TIME_10MS_VALUE * SYSTEM_TIME))
  #define GOTO_SLEEP_TIME        (500   / SYSTEM_TIME)  
  #define KEY_WAKEUP_TIME        (2000  / SYSTEM_TIME)
  
  static u16 work_time_cnt;
  
  if (Flag.Key_Wakeup)
  {
    if (++Key_Wakeup_time_cnt > KEY_WAKEUP_TIME)
    {
      Key_Wakeup_time_cnt = 0;
      Flag.Key_Wakeup = 0;
    }
  }
  
  if ((Motor_Status == MOTOR_STOP) && (!Flag.Del_Code_Finish) && (!Flag.Learn_Code_Statu)&&(!Flag.Clear_status)
  && (!Flag.Opretion_Finish) && (!Flag.Set_Key_ON) && (!Flag.Set_Limit_Point)
  && (!Flag.Delete_single_Code) && (Flag.Electrify_Reset) && (!KeyFlag.CLR_Status)
  && (!KeyFlag.CLR_Status) && (motor_off_time > MOTOR_STOP_TIME) && (!Flag.Key_Wakeup) && (!Flag.Joint_Level_Adj)
  &&(!Clear_Status_Time_Cnt)
    )//ye
  {
    if (++work_time_cnt > GOTO_SLEEP_TIME)
    {
      work_time_cnt = 0;
      if ( ! CHARGE_DETECT())//ye
      {
      WorkModeSetting(SLEEP_MODE);  // 进入睡眠模式 
      }
    }
  }
  else
  {
    work_time_cnt = 0;
  }
}
/**********************************************************************************
  函数名:  PowerControl
  功能:    
**********************************************************************************/
void SleepControl (void)
{
  #define POWER_DOWN_TIME        (10 / SYSTEM_TIME)
  
  if (++Goto_Sleep_Cnt > POWER_DOWN_TIME)
  {
    RF_SLEEP();
    HALL_POWEER_OFF();                      // 关霍尔电源
    MotorFlag.Hall_Power = NULL;            // 不检测霍尔
    ErrorDisposal();
    RF_SLEEP();
   //   LED_BLUE_OFF();//蓝灯灭 
   //   LED_RED_OFF();//红灯灭
    _DINT();                            // 关总中断 
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;

    BCSCTL2 = 0x24;                           // 8M 4分频  主频2M
    P2IES = BIT3 + BIT7 + BIT6;        // Bit = 0: The PxIFGx flag is set with a low-to-high transition
    P2IE  = BIT3 + BIT7 + BIT6;        // 开P2.3  P2.5  P2.6 中断
    INT_CHARGE(INT_OPEN);//P1中断设置,  ye


    P2IFG = 0;
    P1IFG = 0;

    if (P2IE  == (BIT3 + BIT7 + BIT6))
    {
      P2IE  =BIT3 +  BIT7 + BIT6;            // 开P2.3  P2.5  P2.6 中断    
   //   INT_CHARGE(INT_OPEN);//P1中断设置,
      _EINT();                               // 开总中断 
      _BIS_SR(LPM3_bits + GIE);              // Enter LPM3  
    }

    _NOP();
    
    P2IE  = BIT7 + BIT6;                       // 禁止RF信号中断
    P2IFG = 0;
    Goto_Sleep_Cnt = 0;
  }
}
/**********************************************************************************
  函数名:  PowerOffControl
  功能:    
**********************************************************************************/
void PowerOffControl (void)
{
  #define POWER_OFF_TIME        (40 / SYSTEM_TIME)
  
  if (!Flag.Set_Key_ON)
  {
    if (++Goto_Sleep_Cnt > POWER_OFF_TIME)
    {
      BRAKE_ON();                         // 刹车，断开继电器
      BUZZER_OFF();
      MotorStop();                        // 马达停止
      RF_POWER_OFF();                     // 关闭RF模块电源
      RF_SLEEP();
      PowerDownSaveData();               //睡眠前先保存数据
      MotorFlag.Hall_Power = NULL;            // 不检测霍尔
      HALL_POWEER_OFF();                 // 关霍尔电源
     //       LED_BLUE_OFF();//蓝灯灭 
    //  LED_RED_OFF();//红灯灭
      _DINT();                            // 关总中断 
      // Stop watchdog timer to prevent time out reset
      WDTCTL = WDTPW + WDTHOLD;
      _NOP();
      
      P2IES = BIT7 + BIT6;        // Bit = 0: The PxIFGx flag is set with a low-to-high transition
      P2IE  = BIT7 + BIT6;        // 开P2.6  P2.7  断电、按键中断
      P2IFG = 0;
      INT_CHARGE(INT_OPEN);//P1中断设置, ye

      BCSCTL2 = 0x24;                           // 8M 4分频  主频2M
      
      if (P2IE  == (BIT7 + BIT6))
      {
          P2IE  = BIT7 + BIT6;                      // 开P2.5  P2.6 中断
          //INT_CHARGE(INT_OPEN);//P1中断设置,
          _EINT();                                  // 开总中断 
          _BIS_SR(LPM3_bits + GIE);                 // Enter LPM3  
      }
      
      _NOP();
      
      P2IFG = 0;
      Goto_Sleep_Cnt = 0;
      KeyFlag.CLR_Status = 0;
      KeyFlag.SET_Dispose = 0;
      Set_Key_Cnt = 0;
    }
  }
}  

/**********************************************************************************
  函数名:  Tim2Interrupt
  功  能:  Tim2中断函
  输  入:  空
  输  出:  空    
  返  回:  空
  描  述：   
**********************************************************************************/
// Timer_A3 Interrupt Vector (TA0IV) handler
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer1_A0(void)
{
  _BIC_SR_IRQ(LPM3_bits);                       // Clear LPM3 bits from 0(SR)
}