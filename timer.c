/**********************************************************************************
 *  ��Ŀ���:    AC213-04
 *  ��Ŀ����:    25�͹���
 *  �ͻ�����:                                    
 *  �����д:    ��С��
 *  ��������:    2012-09-15
 *  �������:    
 *  �汾����:    V2.4
 *
 *  .c file
 *  ��������: 
**********************************************************************************/
#include "Define.h"                                          /* ȫ�ֺ궨�� ------*/
#include "Extern.h"                                          /* ȫ�ֱ��������� --*/
/* �ڲ��� -----------------------------------------------------------------------*/
#define TIME0_INT_NUMBER       3                             // Time0�жϴ���

#define HALL1                  0
#define HALL2                  1
#define HALL_SR1               BIT0
#define HALL_SR2               BIT1
#define HALL1_IN()            (P2IN & HALL_SR1)              /* P2.0 ------------*/
#define HALL2_IN()            (P2IN & HALL_SR2)              /* P2.1 ------------*/
/* �ڲ����� ---------------------------------------------------------------------*/
static u8 Time2_Int_Cnt = TIME0_INT_NUMBER - 2;
static u8 RF_Signal_Sample1;                                 /* ���ݵ�ƽ���� ----*/
static u8 RF_Signal_Sample2;                                 /* ���ݵ�ƽ���� ----*/

u8 High_Time_Cnt;                                /* ����ʱ�仺�� ----*/
u8 Low_Time_Cnt;                                 /* ����ʱ�仺�� ----*/

u8 RF_High_Cnt_Buffer;                                /* ����ʱ�仺�� ----*/
u8 RF_Low_Cnt_Buffer;                                 /* ����ʱ�仺�� ----*/


void INT_CHARGE(u8 int_enable)//����ж�����
{
	P1IE = int_enable;//ʹ��  
	P1IES = CHARGE_ON;//0��_|-;1��-|_;P13������β��ж��жϣ������Ǹߵ�ƽ���ε��͵�ƽ��
	if(CHARGE_DETECT())//���������
	{
		P1IES = CHARGE_OFF;
	}
	//YE P1IES |= (1<<CHARGE_DET);//P15�ж�0-1��ֻ�ǳ�����������жϡ�����е͵�ƽ��������ߵ�ƽ
	P1IFG = 0;
  
 
}

/*********************************************************************************
  ������:   DetectHallSignal
  ��  ��:   �������ź�
  ��  ��:   ������
  ��  ��:   ���������ȣ����ر�־�����ʵ�����з����־
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
        // �Ƿ������Χ��
        if (Hall_Pulse_Amount_Cnt < HALL_PULSE_MAX)
        {
          ++Hall_Pulse_Amount_Cnt;
          //test();
        }
      }
      else
      {
        // �Ƿ������Χ��
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
  ������:  Tim2Interrupt
  ��  ��:  Tim2�жϺ�
  ��  ��:  ��
  ��  ��:  ��    
  ��  ��:  ��
  ��  ����   
**********************************************************************************/
// Timer_A3 Interrupt Vector (TA0IV) handler
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0(void)
{
  //test_1();
  if (Work_Status == NORMAL_MODE)            /* ����ģʽ --------*/   
  {
    TA0CCR0 += RF_DECODE_TIME * SYSTEM_CLOCK;   // 40 uS
    
    if ((RF_SIGNAL_SAMPLE()))                                  /* ��һ�β��� ------*/
    {
      RF_Signal_Sample1++;
    }
    /*-----------------------------------------------------------------------------*/
    if (MotorFlag.Hall_Power)
    {
      DetectHallSignal();
    }
    /*-----------------------------------------------------------------------------*/
    if ((RF_SIGNAL_SAMPLE()))                                  /* �ڶ��β��� ------*/
    {
      RF_Signal_Sample1++;
    }
    /*-----------------------------------------------------------------------------*/

      // �Ƿ�Ҫ�������
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
    if ((RF_SIGNAL_SAMPLE()))                                  /* �����β��� ------*/
    {
      RF_Signal_Sample1++;
    }
    
    if (RF_Signal_Sample1 >> 1)                                // �Ƿ�����Ϊ�� ----
    {
      RF_Signal_Sample2++;
      Low_Time_Cnt = 0;
      if (++High_Time_Cnt >= 3)
      {
        if (!Flag_RF.Signal_Status)
        {
          Flag.RF_Raise_Edge = TRUE;                           // ��RF�����ر�־  
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
  else if (Work_Status == POWER_DOWN_MODE)            /* ����ģʽ --------*/   
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
  else if (Work_Status == SLEEP_MODE)            /* ˯��ģʽ --------*/
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
          Flag.RF_Raise_Edge = TRUE;                           // ��RF�����ر�־  
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
    
} //�жϽ���������
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    _BIC_SR_IRQ(LPM3_bits);   
    if (Work_Status == SLEEP_MODE)
    {
      WorkModeSetting(NORMAL_MODE);  // ���빤��ģʽ
      Flag.Key_Wakeup = TRUE;
      Key_Wakeup_time_cnt = 0;
    }
	//���ϵ磬�ӵ���ģʽ�������ٵ�����ģʽ
	//�ػ�ģʽһֱ�ٹػ�ģʽ
    P1IFG = 0;
}
/**********************************************************************************
  ������:  Port_2
  ��  ��:  �жϺ�
**********************************************************************************/
// Port 1 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
  _BIC_SR_IRQ(LPM3_bits);                       // Clear LPM3 bits from 0(SR)
  
  if (P2IFG & BIT6)  //�ϵ�
  {
    Goto_Sleep_Cnt = 0;
  }
  
  if (P2IFG & BIT7)    // ����
  {
    if (Work_Status == SLEEP_MODE)
    {
      WorkModeSetting(NORMAL_MODE);  // ���빤��ģʽ
      Flag.Key_Wakeup = TRUE;
      Key_Wakeup_time_cnt = 0;
    }
  }
  
  if (P2IFG & BIT3)  // RF
  {
   // RF_WORK();                                  // RF ����һֱ����ģʽ
  }
  
  P2IFG = 0;
}
/**********************************************************************************
  ������:  
  ����:    
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
      WorkModeSetting(SLEEP_MODE);  // ����˯��ģʽ 
      }
    }
  }
  else
  {
    work_time_cnt = 0;
  }
}
/**********************************************************************************
  ������:  PowerControl
  ����:    
**********************************************************************************/
void SleepControl (void)
{
  #define POWER_DOWN_TIME        (10 / SYSTEM_TIME)
  
  if (++Goto_Sleep_Cnt > POWER_DOWN_TIME)
  {
    RF_SLEEP();
    HALL_POWEER_OFF();                      // �ػ�����Դ
    MotorFlag.Hall_Power = NULL;            // ��������
    ErrorDisposal();
    RF_SLEEP();
   //   LED_BLUE_OFF();//������ 
   //   LED_RED_OFF();//�����
    _DINT();                            // �����ж� 
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;

    BCSCTL2 = 0x24;                           // 8M 4��Ƶ  ��Ƶ2M
    P2IES = BIT3 + BIT7 + BIT6;        // Bit = 0: The PxIFGx flag is set with a low-to-high transition
    P2IE  = BIT3 + BIT7 + BIT6;        // ��P2.3  P2.5  P2.6 �ж�
    INT_CHARGE(INT_OPEN);//P1�ж�����,  ye


    P2IFG = 0;
    P1IFG = 0;

    if (P2IE  == (BIT3 + BIT7 + BIT6))
    {
      P2IE  =BIT3 +  BIT7 + BIT6;            // ��P2.3  P2.5  P2.6 �ж�    
   //   INT_CHARGE(INT_OPEN);//P1�ж�����,
      _EINT();                               // �����ж� 
      _BIS_SR(LPM3_bits + GIE);              // Enter LPM3  
    }

    _NOP();
    
    P2IE  = BIT7 + BIT6;                       // ��ֹRF�ź��ж�
    P2IFG = 0;
    Goto_Sleep_Cnt = 0;
  }
}
/**********************************************************************************
  ������:  PowerOffControl
  ����:    
**********************************************************************************/
void PowerOffControl (void)
{
  #define POWER_OFF_TIME        (40 / SYSTEM_TIME)
  
  if (!Flag.Set_Key_ON)
  {
    if (++Goto_Sleep_Cnt > POWER_OFF_TIME)
    {
      BRAKE_ON();                         // ɲ�����Ͽ��̵���
      BUZZER_OFF();
      MotorStop();                        // ���ֹͣ
      RF_POWER_OFF();                     // �ر�RFģ���Դ
      RF_SLEEP();
      PowerDownSaveData();               //˯��ǰ�ȱ�������
      MotorFlag.Hall_Power = NULL;            // ��������
      HALL_POWEER_OFF();                 // �ػ�����Դ
     //       LED_BLUE_OFF();//������ 
    //  LED_RED_OFF();//�����
      _DINT();                            // �����ж� 
      // Stop watchdog timer to prevent time out reset
      WDTCTL = WDTPW + WDTHOLD;
      _NOP();
      
      P2IES = BIT7 + BIT6;        // Bit = 0: The PxIFGx flag is set with a low-to-high transition
      P2IE  = BIT7 + BIT6;        // ��P2.6  P2.7  �ϵ硢�����ж�
      P2IFG = 0;
      INT_CHARGE(INT_OPEN);//P1�ж�����, ye

      BCSCTL2 = 0x24;                           // 8M 4��Ƶ  ��Ƶ2M
      
      if (P2IE  == (BIT7 + BIT6))
      {
          P2IE  = BIT7 + BIT6;                      // ��P2.5  P2.6 �ж�
          //INT_CHARGE(INT_OPEN);//P1�ж�����,
          _EINT();                                  // �����ж� 
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
  ������:  Tim2Interrupt
  ��  ��:  Tim2�жϺ�
  ��  ��:  ��
  ��  ��:  ��    
  ��  ��:  ��
  ��  ����   
**********************************************************************************/
// Timer_A3 Interrupt Vector (TA0IV) handler
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer1_A0(void)
{
  _BIC_SR_IRQ(LPM3_bits);                       // Clear LPM3 bits from 0(SR)
}