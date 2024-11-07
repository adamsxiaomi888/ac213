/**********************************************************************************
 *  项目编号:    AC213-04
 *  项目名称:    25低功耗
 *  客户名称:                                    
 *  程序编写:    符小玲
 *  启动日期:    2012-09-15
 *  完成日期:    
 *  版本号码:    V2.4
 *  
 *  Samping.c file
 *  功能描述: AD及电机速度采样
**********************************************************************************/
#include "Define.h"                                          /* 全局宏定义 ------*/
#include "Extern.h"                                          /* 全局变量及函数 --*/
/* 内部宏 -----------------------------------------------------------------------*/
/********************* 计算电压 ***************************************************
3.3/1024=0.00322265625 0.1/0.00322265625=31
**********************************************************************************/
//#define VOLTAGE_RECKON(ad_val) ((ad_val) + (((ad_val) * 47) / 100))     //((((ad_val)*60)) / 28)    //31
#define VOLTAGE_RECKON(ad_val)   ((ad_val) + ((ad_val) / 3) + ((ad_val) / 5))     //((((ad_val)*60)) / 28)    //31

#define CURRENT_RECKON(ad_val) ((u16) ((ad_val) << 4))              //  电流放大16倍

/* 内部变量 ---------------------------------------------------------------------*/
static u16 ADC_VOL_Cnt;
static u16 ADC_VOL_Buffer;
static u16 ADC_VOL_Array[4];
static u8 ADC_VOL_Array_Index;
static u8 ADC_CUR_Cnt;
static u16 ADC_CUR_Buffer;
static u16 ADC_CUR_Array[4];
static u8 ADC_CUR_Array_Index;


/* 内部函数 ---------------------------------------------------------------------*/
void VoltageProtect (void);
/**********************************************************************************
  函数名:   ADCSetting
  功  能:   ADC 设置
  输  入:   空
  输  出:   空    
  返  回:   空
  描  述：
**********************************************************************************/
void ADCSetting (void)
{
  P1SEL |= BIT4 + BIT6;                            //  a4 a6
  //ADC10CTL0 = ADC10SHT_2 + ADC10ON;              // 8 * ADC10CLKs  ADC10ON
  
  ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + ADC10ON + REF2_5V;
  //ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + ADC10ON;
  
  
  ADC10CTL1 = INCH_6;                            // input A6
  ADC10AE0 |= BIT6;                              // P1.6 ADC option select
  
  ADC10CTL0 |= ENC + ADC10SC;                    // Sampling and conversion start
  
  ADC_VOL_Cnt = 0;
  ADC_VOL_Buffer = 0;
  ADC_VOL_Array[0] = 0;
  ADC_VOL_Array[1] = 0;
  ADC_VOL_Array[2] = 0;
  ADC_VOL_Array[3] = 0;
  ADC_VOL_Array_Index = 0;
}
/**********************************************************************************
  函数名:   SampleAverage
  功  能:   AD值求平均
  输  入:   数组，数组的大小
  输  出:   空
  返  回:   平均值
  描  述：  去最大和最小值，中值求平均
 *********************************************************************************/
u16 SampleAverage (u16 array[],u8 num)
{
    u8 max;
    u8 min;
    u8 i;
    u8 j;
    u16 value;
    
    j = 0;
    max = 0;
    min = 0;
    value = 0;
    
    for (i = num - 1; i > 0; i--)  
    {
        j++;
        if (array[j] > array[max])   
        {
            max = j;
        }
        if (array[j] < array[min])   
        {
            min = j;
        }
    }
    for  (i = NULL; i < num; i++)   
    {
        if ((i != max) && (i != min))     
        {
            value += array[i];  
        }    
    }    
    
    if (max == min)      
    {
        value = array[max];  
    }
    else
    {
        value = value / (num - 2);
    }    
    return (value);
}
/**********************************************************************************
  函数名:   ADSamping
  功  能:   AD采样
  输  入:   空
  输  出:   空
  返  回:   空
  描  述：  
 *********************************************************************************/
void ADSamping (void)
{
  u16 buffer;
  
  if (!(ADC10CTL1 & ADC10BUSY))	
  {
    buffer = ADC10MEM; 
    
    if (ADC10CTL1 == INCH_6) 
    {
      ADC_VOL_Buffer += buffer; 
      ADC_VOL_Cnt++;       
      if (ADC_VOL_Cnt >> 2)  
      {
        ADC_VOL_Cnt = NULL;
        
        ADC_VOL_Array[ADC_VOL_Array_Index] = ADC_VOL_Buffer >> 2;
        ADC_VOL_Buffer = NULL;
        ADC_VOL_Array_Index++;
        if (ADC_VOL_Array_Index >> 2)  
        {
          u16 temp;
          temp = SampleAverage(ADC_VOL_Array,ADC_VOL_Array_Index);
          Current_Voltage = VOLTAGE_RECKON(temp);  
          ADC_VOL_Array_Index = NULL;
          
          Power_Down_Voltage = Current_Voltage;
        }
      }
      
      ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + ADC10ON + REF2_5V;
      ADC10CTL1 = INCH_4;                            // input A4
      ADC10AE0 |= BIT4;                              // P1.4 ADC option select
    }
    else if (ADC10CTL1 == INCH_4)       
    {
      ADC_CUR_Buffer += buffer;                              /* AD值累加 --------*/
      ADC_CUR_Cnt++;                                         /* 累加次数加1 -----*/
      if (ADC_CUR_Cnt >> 2)                                  /* 是否满32次 ------*/
      {
        ADC_CUR_Cnt = NULL;
        ADC_CUR_Array[ADC_CUR_Array_Index] = ADC_CUR_Buffer >> 2;
        ADC_CUR_Buffer = NULL;
        ADC_CUR_Array_Index++;
        if (ADC_CUR_Array_Index >> 2)                        /* 是否存过4次 -----*/
        {
          u16 temp;
          temp = SampleAverage(ADC_CUR_Array,ADC_CUR_Array_Index);
          
          /* 根据AD值计算电流 */
          Current_Current_mA = CURRENT_RECKON(temp);
          ADC_CUR_Array_Index = NULL;
          
	  if (Current_Current_mA > MAX_OUT_CURRENT)
          {
            Duty_Sub(&Motor_Duty,20);
            if (MotorFlag.Motor_Direction)
            {
              MotorUp(Motor_Duty);
            }
            else
            {
              MotorDown(Motor_Duty);
            }
          }
        }
      }
      
      ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + ADC10ON + REF2_5V;
      ADC10CTL1 = INCH_6;                            // input A6
      ADC10AE0 |= BIT6;                              // P1.6 ADC option select
    }
    
   ADC10CTL0 |= ENC + ADC10SC;                    // Sampling and conversion start
  }
}
/**********************************************************************************
  函数名:   PowerDownSamping
  功  能:   断电检测
 *********************************************************************************/
void PowerDownSamping (void)
{
  u16 buffer;
  
  if (!(ADC10CTL1 & ADC10BUSY))	
  {
    buffer = ADC10MEM; 
    
    if (ADC10CTL1 == INCH_6) 
    {
      ADC_VOL_Buffer += buffer; 
      ADC_VOL_Cnt++;       
      if (ADC_VOL_Cnt >> 2)  
      {
        ADC_VOL_Cnt = NULL;
        
        ADC_VOL_Array[ADC_VOL_Array_Index] = ADC_VOL_Buffer >> 2;
        ADC_VOL_Buffer = NULL;
        ADC_VOL_Array_Index++;
        if (ADC_VOL_Array_Index >> 2)  
        {
          u16 temp;
          temp = SampleAverage(ADC_VOL_Array,ADC_VOL_Array_Index);
          Power_Down_Voltage = VOLTAGE_RECKON(temp);  
          ADC_VOL_Array_Index = NULL;
        }
      }
      
      ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + ADC10ON + REF2_5V;
      ADC10CTL1 = INCH_6;                            // input A4
      ADC10AE0 |= BIT6;                              // P1.4 ADC option select
    }
    else if (ADC10CTL1 == INCH_4)       
    {
      ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + ADC10ON + REF2_5V;
      ADC10CTL1 = INCH_6;                            // input A6
      ADC10AE0 |= BIT6;                              // P1.6 ADC option select
    }
    
   ADC10CTL0 |= ENC + ADC10SC;                    // Sampling and conversion start
  }
}
/**********************************************************************************
  函数名:   VolSamping
  功  能:   AD采样
  输  入:   空
  输  出:   空
  返  回:   空
  描  述：  
 *********************************************************************************/
void VolSamping (void)
{
  u16 buffer;
  
  if (!(ADC10CTL1 & ADC10BUSY))	
  {
    buffer = ADC10MEM; 
    
    if (ADC10CTL1 == INCH_6) 
    {
        ADC_VOL_Array[ADC_VOL_Array_Index] = buffer;
        ADC_VOL_Array_Index++;
        if (ADC_VOL_Array_Index >> 2)  
        {
          u16 temp;
          temp = SampleAverage(ADC_VOL_Array,ADC_VOL_Array_Index);
          Current_Voltage = VOLTAGE_RECKON(temp);  
          ADC_VOL_Array_Index = NULL;
          
          if (Flag.Power_Down)
          {
            if (Current_Voltage >= VOL_RECOVERY_VALUE)     // 电压是否高于正常电压6.7V
            {
              Flag.Low_VOL_Jiggle = 0;                    //　清除低压提示
              WorkModeSetting(NORMAL_MODE);               // 从掉电状态进入工作模式
              Flag.Learn_Key_Hint = TRUE;
            }
          }
        }
      
      ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + ADC10ON + REF2_5V;
      ADC10CTL1 = INCH_6;                            // input A4
      ADC10AE0 |= BIT6;                              // P1.4 ADC option select
    }
    
   ADC10CTL0 |= ENC + ADC10SC;                    // Sampling and conversion start
  }
}
/**********************************************************************************
  函数名:   PowerDownSaveData
  功  能:   
 *********************************************************************************/
void PowerDownSaveData (void)
{
  if ((MotorFlag.EE_Save) && (Limit_Point[0].Flag.F1.Limit_Activate)) // 是否需要保存并已经有行程了
  {
    
    FlashWriteData(0Xee);
    MotorFlag.Hall_Power = NULL;
    HALL_POWEER_OFF();
    MotorFlag.EE_Save = NULL;
    Flag.Del_Code_Finish = NULL;
    Flag.Opretion_Finish = NULL;
    Flag.Learn_Code_Statu = NULL;
    Opretion_Event = NULL;
  }
}
/**********************************************************************************
  函数名:   VoltageProtect
  功  能:   
 *********************************************************************************/
void VoltageProtect (void)
{
  #define POWER_DOWN_SAMPING_TIME  20 / SYSTEM_TIME
  
  static u8 Power_Down_Samping_Cnt;
  static u8 Power_Normal_Cnt;
  
  
  if (Power_Down_Voltage <= POWER_DOWN_VAULE)        // 是否断电
  {
    
    if (++Power_Down_Samping_Cnt >= POWER_DOWN_SAMPING_TIME)
    {
      Power_Normal_Cnt = 0;
      Power_Down_Samping_Cnt = 0;
      Current_Voltage = POWER_DOWN_VAULE;
      
      BUZZER_OFF();
      MotorStop();                                 // 马达停止
      MotorFlag.Motor_Run = 0;
      Motor_Status = MOTOR_STOP;
      Motor_Status_Buffer = MOTOR_STOP;              
      
      if (Work_Status == SLEEP_MODE)
      {
        PowerDown_Time_Cnt = 350 / SYSTEM_TIME;  // 睡眠中检测到断电马上保存
      }
      // 进入掉电模式
      if (Work_Status != POWER_DOWN_MODE)
      {
          WorkModeSetting(POWER_DOWN_MODE);            
      }
    }
  }
  else if (Power_Down_Voltage >= VOL_NORMAL_VALUE)     // 电压是否高于正常电压6.7V
  {
    if (++Power_Normal_Cnt >= POWER_DOWN_SAMPING_TIME)
    {
      Power_Normal_Cnt = 0;
      Power_Down_Samping_Cnt = 0;
    }
  }
  
  if (Current_Voltage <= POWER_DOWN_VAULE)        // 是否断电
  {
    if (++Power_Down_Cnt > (50 / SYSTEM_TIME))   // 100mS
    {
      Power_Down_Cnt = 0;
      Low_VOL_Cnt = 0;
      VOL_Recover_Cnt = 0;
      
      //BRAKE_ON();                                // 刹车，断开继电器
      BUZZER_OFF();
      MotorStop();                                 // 马达停止
      MotorFlag.Motor_Run = 0;
      Motor_Status = MOTOR_STOP;
      Motor_Status_Buffer = MOTOR_STOP;
      
      if (Work_Status != POWER_DOWN_MODE)
      {
         WorkModeSetting(POWER_DOWN_MODE);            // 进入掉电模式
      }
    }
  }
  else if (Current_Voltage <= LOW_VOL_HINT_VALUE)   // 电压是否低于6.5V
  {
    if (++Low_VOL_Cnt > (2000 / SYSTEM_TIME))   // 2S
    {
      Power_Down_Cnt = 0;
      Low_VOL_Cnt = 0;
      VOL_Recover_Cnt = 0;
      Flag.Low_VOL_Jiggle = TRUE;                    //　低压提示
    }
  }
  else if (Current_Voltage >= VOL_NORMAL_VALUE)     // 电压是否高于正常电压6.7V
  {
    if (++VOL_Recover_Cnt > (2000 / SYSTEM_TIME))   // 2S
    {
      Power_Down_Cnt = 0;
      Low_VOL_Cnt = 0;
      VOL_Recover_Cnt = 0;
      Flag.Low_VOL_Jiggle = 0;                    //　清除低压提示
      
      if (Work_Status != NORMAL_MODE)
      {
        WorkModeSetting(NORMAL_MODE);            // 进入工作模式
      }
    }
  }
}
/**********************************************************************************
  函数名:   PowerDownDetect
  功  能:   掉电检测
 *********************************************************************************/
void PowerDownDetect (void)
{
  #define POWER_DOWN_TIME  20 / SYSTEM_TIME
  static u8 Power_Down_Detect_Cnt;
  
  if (POWER_DOWN_DETECT())
  {
    Power_Down_Detect_Cnt = 0;
  }
  else
  {
    if (++Power_Down_Detect_Cnt >= POWER_DOWN_TIME)
    {
      Power_Down_Detect_Cnt = 0;
      Current_Voltage = POWER_DOWN_VAULE;
      BUZZER_OFF();
      MotorStop();                       // 马达停止
      
      MotorFlag.Motor_Run = 0;
      Motor_Status = MOTOR_STOP;
      Motor_Status_Buffer = MOTOR_STOP;
      
      if (Work_Status == SLEEP_MODE)
      {
        PowerDown_Time_Cnt = 350 / SYSTEM_TIME;  // 睡眠中检测到断电马上保存
      }
      // 进入掉电模式
      if (Work_Status != POWER_DOWN_MODE)
      {
        WorkModeSetting(POWER_DOWN_MODE);            
      }
    }
    
    if (Power_Down_Detect_Cnt >= 5)
    {
      Goto_Sleep_Cnt = 0;
    }
  }
}
/**********************************************************************************
  函数名:   PowerDownHandle
  功  能:   
 *********************************************************************************/
void PowerDownHandle (void)
{
  if ((PowerDown_Time_Cnt > (200 / SYSTEM_TIME)) && (PowerDown_Time_Cnt < (300 / SYSTEM_TIME)))     // 350MS
  {
    BRAKE_ON();                         // 刹车，断开继电器
    BUZZER_OFF();
    MotorStop();                        // 马达停止
    RF_POWER_OFF();                     // 关闭RF模块电源
    RF_SLEEP();
    POWEER_DETECT_OFF();               // 关电源输入电压检测
  }
  else if ((PowerDown_Time_Cnt > (350 / SYSTEM_TIME)) && (PowerDown_Time_Cnt < (360 / SYSTEM_TIME)))     // 350MS
  {
    PowerDownSaveData();
  }
  else if (PowerDown_Time_Cnt > (400 / SYSTEM_TIME))
  {
    
    MotorFlag.Hall_Power = NULL;
    HALL_POWEER_OFF();
    BRAKE_ON();                         // 刹车，断开继电器
    BUZZER_OFF();
    MotorStop();                        // 马达停止
    RF_POWER_OFF();                     // 关闭RF模块电源
    RF_SLEEP();
    POWEER_DETECT_OFF();               // 关电源输入电压检测
    
    _DINT();                            // 关总中断 
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;
    
    P2SEL = 0;                               // P2.4 options
    
    // 停止ADC采样，关闭ADC模块
    P1SEL = 0x00;                           //  a4 a6
    ADC10CTL0 &= SREF_1 + ADC10SHT_2 + REFON + ADC10ON + REF2_5V;                     //  a4 a6
    ADC10AE0 &= 0x50;
    ADC10CTL0 &= ENC + ADC10SC;             // Sampling and conversion start
    
    BCSCTL2 = 0x36;                           // 8分频
    
    P2IES = 0;        // Bit = 0: The PxIFGx flag is set with a low-to-high transition
    P2IE  = 0;        // 开P2.3  P2.5  P2.6 中断
    P2IFG = 0;
    P1IFG = 0;

    INT_CHARGE(INT_OPEN);//P1中断设置,
	
    TA1CCTL0 = CCIE;                           // CCR0 interrupt enabled
    TA1CCR0 = 5000;                            //10000
    TA1CTL = TASSEL_1 + ID_3 + MC_1;           // ACLK, Up to CCR0
    
    Flag.Power_Down = TRUE;
    
    _EINT();                               // 开总中断 
    _BIS_SR(LPM3_bits + GIE);              // Enter LPM3  
    _NOP();
    
    TA1CCTL0 = 0;
    POWEER_DETECT_ON();                         // 开电源输入电压检测
    ADCSetting();                               // ADC 设置
    P2IFG = 0;
    PowerDown_Time_Cnt = (390 / SYSTEM_TIME);
  }
  
  if (PowerDown_Time_Cnt < 60000)
  {
    PowerDown_Time_Cnt++;
  }
}