/**********************************************************************************
 *  ��Ŀ���:    AC213-04
 *  ��Ŀ����:    25�͹���
 *  �ͻ�����:                                    
 *  �����д:    ��С��
 *  ��������:    2012-09-15
 *  �������:    
 *  �汾����:    V2.4
 *  
 *  Samping.c file
 *  ��������: AD������ٶȲ���
**********************************************************************************/
#include "Define.h"                                          /* ȫ�ֺ궨�� ------*/
#include "Extern.h"                                          /* ȫ�ֱ��������� --*/
/* �ڲ��� -----------------------------------------------------------------------*/
/********************* �����ѹ ***************************************************
3.3/1024=0.00322265625 0.1/0.00322265625=31
**********************************************************************************/
//#define VOLTAGE_RECKON(ad_val) ((ad_val) + (((ad_val) * 47) / 100))     //((((ad_val)*60)) / 28)    //31
#define VOLTAGE_RECKON(ad_val)   ((ad_val) + ((ad_val) / 3) + ((ad_val) / 5))     //((((ad_val)*60)) / 28)    //31

#define CURRENT_RECKON(ad_val) ((u16) ((ad_val) << 4))              //  �����Ŵ�16��

/* �ڲ����� ---------------------------------------------------------------------*/
static u16 ADC_VOL_Cnt;
static u16 ADC_VOL_Buffer;
static u16 ADC_VOL_Array[4];
static u8 ADC_VOL_Array_Index;
static u8 ADC_CUR_Cnt;
static u16 ADC_CUR_Buffer;
static u16 ADC_CUR_Array[4];
static u8 ADC_CUR_Array_Index;


/* �ڲ����� ---------------------------------------------------------------------*/
void VoltageProtect (void);
/**********************************************************************************
  ������:   ADCSetting
  ��  ��:   ADC ����
  ��  ��:   ��
  ��  ��:   ��    
  ��  ��:   ��
  ��  ����
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
  ������:   SampleAverage
  ��  ��:   ADֵ��ƽ��
  ��  ��:   ���飬����Ĵ�С
  ��  ��:   ��
  ��  ��:   ƽ��ֵ
  ��  ����  ȥ������Сֵ����ֵ��ƽ��
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
  ������:   ADSamping
  ��  ��:   AD����
  ��  ��:   ��
  ��  ��:   ��
  ��  ��:   ��
  ��  ����  
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
      ADC_CUR_Buffer += buffer;                              /* ADֵ�ۼ� --------*/
      ADC_CUR_Cnt++;                                         /* �ۼӴ�����1 -----*/
      if (ADC_CUR_Cnt >> 2)                                  /* �Ƿ���32�� ------*/
      {
        ADC_CUR_Cnt = NULL;
        ADC_CUR_Array[ADC_CUR_Array_Index] = ADC_CUR_Buffer >> 2;
        ADC_CUR_Buffer = NULL;
        ADC_CUR_Array_Index++;
        if (ADC_CUR_Array_Index >> 2)                        /* �Ƿ���4�� -----*/
        {
          u16 temp;
          temp = SampleAverage(ADC_CUR_Array,ADC_CUR_Array_Index);
          
          /* ����ADֵ������� */
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
  ������:   PowerDownSamping
  ��  ��:   �ϵ���
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
  ������:   VolSamping
  ��  ��:   AD����
  ��  ��:   ��
  ��  ��:   ��
  ��  ��:   ��
  ��  ����  
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
            if (Current_Voltage >= VOL_RECOVERY_VALUE)     // ��ѹ�Ƿ����������ѹ6.7V
            {
              Flag.Low_VOL_Jiggle = 0;                    //�������ѹ��ʾ
              WorkModeSetting(NORMAL_MODE);               // �ӵ���״̬���빤��ģʽ
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
  ������:   PowerDownSaveData
  ��  ��:   
 *********************************************************************************/
void PowerDownSaveData (void)
{
  if ((MotorFlag.EE_Save) && (Limit_Point[0].Flag.F1.Limit_Activate)) // �Ƿ���Ҫ���沢�Ѿ����г���
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
  ������:   VoltageProtect
  ��  ��:   
 *********************************************************************************/
void VoltageProtect (void)
{
  #define POWER_DOWN_SAMPING_TIME  20 / SYSTEM_TIME
  
  static u8 Power_Down_Samping_Cnt;
  static u8 Power_Normal_Cnt;
  
  
  if (Power_Down_Voltage <= POWER_DOWN_VAULE)        // �Ƿ�ϵ�
  {
    
    if (++Power_Down_Samping_Cnt >= POWER_DOWN_SAMPING_TIME)
    {
      Power_Normal_Cnt = 0;
      Power_Down_Samping_Cnt = 0;
      Current_Voltage = POWER_DOWN_VAULE;
      
      BUZZER_OFF();
      MotorStop();                                 // ���ֹͣ
      MotorFlag.Motor_Run = 0;
      Motor_Status = MOTOR_STOP;
      Motor_Status_Buffer = MOTOR_STOP;              
      
      if (Work_Status == SLEEP_MODE)
      {
        PowerDown_Time_Cnt = 350 / SYSTEM_TIME;  // ˯���м�⵽�ϵ����ϱ���
      }
      // �������ģʽ
      if (Work_Status != POWER_DOWN_MODE)
      {
          WorkModeSetting(POWER_DOWN_MODE);            
      }
    }
  }
  else if (Power_Down_Voltage >= VOL_NORMAL_VALUE)     // ��ѹ�Ƿ����������ѹ6.7V
  {
    if (++Power_Normal_Cnt >= POWER_DOWN_SAMPING_TIME)
    {
      Power_Normal_Cnt = 0;
      Power_Down_Samping_Cnt = 0;
    }
  }
  
  if (Current_Voltage <= POWER_DOWN_VAULE)        // �Ƿ�ϵ�
  {
    if (++Power_Down_Cnt > (50 / SYSTEM_TIME))   // 100mS
    {
      Power_Down_Cnt = 0;
      Low_VOL_Cnt = 0;
      VOL_Recover_Cnt = 0;
      
      //BRAKE_ON();                                // ɲ�����Ͽ��̵���
      BUZZER_OFF();
      MotorStop();                                 // ���ֹͣ
      MotorFlag.Motor_Run = 0;
      Motor_Status = MOTOR_STOP;
      Motor_Status_Buffer = MOTOR_STOP;
      
      if (Work_Status != POWER_DOWN_MODE)
      {
         WorkModeSetting(POWER_DOWN_MODE);            // �������ģʽ
      }
    }
  }
  else if (Current_Voltage <= LOW_VOL_HINT_VALUE)   // ��ѹ�Ƿ����6.5V
  {
    if (++Low_VOL_Cnt > (2000 / SYSTEM_TIME))   // 2S
    {
      Power_Down_Cnt = 0;
      Low_VOL_Cnt = 0;
      VOL_Recover_Cnt = 0;
      Flag.Low_VOL_Jiggle = TRUE;                    //����ѹ��ʾ
    }
  }
  else if (Current_Voltage >= VOL_NORMAL_VALUE)     // ��ѹ�Ƿ����������ѹ6.7V
  {
    if (++VOL_Recover_Cnt > (2000 / SYSTEM_TIME))   // 2S
    {
      Power_Down_Cnt = 0;
      Low_VOL_Cnt = 0;
      VOL_Recover_Cnt = 0;
      Flag.Low_VOL_Jiggle = 0;                    //�������ѹ��ʾ
      
      if (Work_Status != NORMAL_MODE)
      {
        WorkModeSetting(NORMAL_MODE);            // ���빤��ģʽ
      }
    }
  }
}
/**********************************************************************************
  ������:   PowerDownDetect
  ��  ��:   ������
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
      MotorStop();                       // ���ֹͣ
      
      MotorFlag.Motor_Run = 0;
      Motor_Status = MOTOR_STOP;
      Motor_Status_Buffer = MOTOR_STOP;
      
      if (Work_Status == SLEEP_MODE)
      {
        PowerDown_Time_Cnt = 350 / SYSTEM_TIME;  // ˯���м�⵽�ϵ����ϱ���
      }
      // �������ģʽ
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
  ������:   PowerDownHandle
  ��  ��:   
 *********************************************************************************/
void PowerDownHandle (void)
{
  if ((PowerDown_Time_Cnt > (200 / SYSTEM_TIME)) && (PowerDown_Time_Cnt < (300 / SYSTEM_TIME)))     // 350MS
  {
    BRAKE_ON();                         // ɲ�����Ͽ��̵���
    BUZZER_OFF();
    MotorStop();                        // ���ֹͣ
    RF_POWER_OFF();                     // �ر�RFģ���Դ
    RF_SLEEP();
    POWEER_DETECT_OFF();               // �ص�Դ�����ѹ���
  }
  else if ((PowerDown_Time_Cnt > (350 / SYSTEM_TIME)) && (PowerDown_Time_Cnt < (360 / SYSTEM_TIME)))     // 350MS
  {
    PowerDownSaveData();
  }
  else if (PowerDown_Time_Cnt > (400 / SYSTEM_TIME))
  {
    
    MotorFlag.Hall_Power = NULL;
    HALL_POWEER_OFF();
    BRAKE_ON();                         // ɲ�����Ͽ��̵���
    BUZZER_OFF();
    MotorStop();                        // ���ֹͣ
    RF_POWER_OFF();                     // �ر�RFģ���Դ
    RF_SLEEP();
    POWEER_DETECT_OFF();               // �ص�Դ�����ѹ���
    
    _DINT();                            // �����ж� 
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;
    
    P2SEL = 0;                               // P2.4 options
    
    // ֹͣADC�������ر�ADCģ��
    P1SEL = 0x00;                           //  a4 a6
    ADC10CTL0 &= SREF_1 + ADC10SHT_2 + REFON + ADC10ON + REF2_5V;                     //  a4 a6
    ADC10AE0 &= 0x50;
    ADC10CTL0 &= ENC + ADC10SC;             // Sampling and conversion start
    
    BCSCTL2 = 0x36;                           // 8��Ƶ
    
    P2IES = 0;        // Bit = 0: The PxIFGx flag is set with a low-to-high transition
    P2IE  = 0;        // ��P2.3  P2.5  P2.6 �ж�
    P2IFG = 0;
    P1IFG = 0;

    INT_CHARGE(INT_OPEN);//P1�ж�����,
	
    TA1CCTL0 = CCIE;                           // CCR0 interrupt enabled
    TA1CCR0 = 5000;                            //10000
    TA1CTL = TASSEL_1 + ID_3 + MC_1;           // ACLK, Up to CCR0
    
    Flag.Power_Down = TRUE;
    
    _EINT();                               // �����ж� 
    _BIS_SR(LPM3_bits + GIE);              // Enter LPM3  
    _NOP();
    
    TA1CCTL0 = 0;
    POWEER_DETECT_ON();                         // ����Դ�����ѹ���
    ADCSetting();                               // ADC ����
    P2IFG = 0;
    PowerDown_Time_Cnt = (390 / SYSTEM_TIME);
  }
  
  if (PowerDown_Time_Cnt < 60000)
  {
    PowerDown_Time_Cnt++;
  }
}