/**********************************************************************************
 *  ��Ŀ���:    AC213-04
 *  ��Ŀ����:    25�͹���
 *  �ͻ�����:                                    
 *  �����д:    ��С��
 *  ��������:    2012-09-15
 *  �������:    
 *  �汾����:    V2.4
 *  ��������:    ���ӵ�������ѹ��⣬PWM����          
 *  оƬѡ��:    MSP430G2553IPW28R
 *  ��Ƶ��:    �ڲ� RC16.0M
 *  Watchdog:    Enable
 *  MAIN.C file
**********************************************************************************/
#include "Define.h"                                          /* ȫ�ֺ궨�� ------*/
#include "Extern.h"                                          /* ȫ�ֱ��������� --*/
/* �ڲ����� ---------------------------------------------------------------------*/
void NormalMode     (void);
void SleepMode      (void);
void PowerOffMode   (void);
void PowerDownMode  (void);
/* �ڲ����� ---------------------------------------------------------------------*/
WorkStatusManage const Work_Status_Array [WORK_STATUS_NUM] =
{
  NormalMode,
  SleepMode,
  PowerOffMode,
  PowerDownMode,
};
/**********************************************************************************
  ������:  main
  ��  ��:  
  ��  ��:  ��
  ��  ��:  ��
  ��  ��:  ��
  ��  ���� 
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
  ������:  NormalMode
  ��  ��:  
**********************************************************************************/
void NormalMode (void)
{
  // �ܿ��������˲��  AD����
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
  if((Old_Motor_Status != Motor_Status)&&(!Flag.Jiggle_Stop))                  //���״̬�л���ʱ�����շ��Ͱٷֱ�λ��
  {
    u8 data[] = {Motor_Status, JourneyPercentCount(), 0x00, 0x00};
    UartUpload(0xA1, sizeof(data), data, 3, 0);
    Old_Motor_Status = Motor_Status;
  }
  /* RF���� -------------------------------------------------------------------*/
  if (Flag.RF_Raise_Edge)                                    /* �Ƿ��������� ----*/
  {
    Flag.RF_Raise_Edge = NULL;
    if (RFSignalDecode(RF_High_Cnt,RF_Low_Cnt))              /* RF���� ----------*/
    {
      if (RFDataVerify())                                    /* �����Ƿ���ȷ ----*/
      {
        RFDataDisposal();                                    /* RF���ݴ��� ------*/
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
      JourneyComper();                                         /* �Ƚ��г� --------*/
    VoltageProtect();
    KeyManage();                                             /* ����ɨ�輰���� --*/
    WorkTimeControl();                                      // ����ʱ�����
    PowerDownDetect();
    
  }
  /* 10ms ---------------------------------------------------------------------*/
  if (Flag.Time_10ms)
  {
    Flag.Time_10ms = NULL;

    /* ����ת�� ---------------------------------------------------------------*/
    if (MotorFlag.Hall_Pulse_edge)
    {
      MotorFlag.Hall_Pulse_edge = NULL;
      CountMotorSpeed();                                   /* ����������ת�� */
    }
    MotorControl();                                        /* ������� --------*/
    MotorStatusSwitch();                                   /* ���״̬�л� ----*/
  }
  /* 50ms ---------------------------------------------------------------------*/
  if (Flag.Time_50ms)
  {
    Flag.Time_50ms = NULL;
    RFStatusInspect();
    SetLimitPoint();
    if (Flag.Delete_Code)                                  /* �Ƿ�ɾȫ�� ------*/
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
    OprationCue();                                    // ��ʾ
    DblclickTimeLimit();                              // ����λ��˫��
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
    
    if(NET_Succe_Time_Cnt> 36)   //û�������ɹ�36������͹���
    {
    Flag.net_successful_count = 0;    //���ʱ��־
    NET_Succe_Time_Cnt = 0;             //��ʱ������0
    Clear_Status_Time_Cnt = 0;      //����͹���
    }
      
  }
}
/**********************************************************************************
  ������:  SleepMode
  ��  ��:  
**********************************************************************************/
void SleepMode (void)
{
  /* RF���� -------------------------------------------------------------------*/
  if (Flag.RF_Raise_Edge)                                    /* �Ƿ��������� ----*/
  {
    Flag.RF_Raise_Edge = NULL;
    if (RFSignalDecode(RF_High_Cnt,RF_Low_Cnt))              /* RF���� ----------*/
    {
      if (RFDataVerify())                                    /* �����Ƿ���ȷ ----*/
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
    SleepControl();                                        // ˯�߿���
    
//    if(!Clear_Status_Time_Cnt)
//    {
//          LED_BLUE_OFF();
//      Flag.Clear_WIFI_Time_End = 1;
//      Clear_Status_End_Time_Cnt = 2;
//      Flag.Clear_status_hint = NULL;
 
    
    Clear_WIFI_Hint();                //LIU��ֹ����ʱ���̽���͹���
    
    
  }
  
  
}
/**********************************************************************************
  ������:  PowerOffMode
  ��  ��:  
**********************************************************************************/
void PowerOffMode  (void)
{
  /* 2ms ----------------------------------------------------------------------*/
  if (Flag.Time_2ms)
  {
    Flag.Time_2ms = NULL;
    PowerDownDetect();
    PowerOffControl();                                       // ˯�߿���
    KeyManage();                                             // ����ɨ�輰����
  }
}
/**********************************************************************************
  ������:  PowerDownMode
  ��  ��:  
**********************************************************************************/
void PowerDownMode  (void)
{
  VolSamping();
  /* 2ms ----------------------------------------------------------------------*/
  if (Flag.Time_2ms)
  {
    Flag.Time_2ms = NULL;
    if(CHARGE_DETECT())//���ϳ����         
    {
      LED_RED_ON();//�����
      LED_BLUE_OFF();//������  
      if(CHARGE_STATUS())//������
      {
          LED_RED_OFF();//�����
          LED_BLUE_ON();//������
      }
    }
    else//�ε������
    {
      LED_BLUE_OFF();//������  
      LED_RED_OFF();//�����
    }
    PowerDownDetect();
    PowerDownHandle();                                     // ���紦��
  }
}
/**********************************************************************************
  ������:  LowPowerSet
  ��  ��:  �ػ���TX��RX��IO������
**********************************************************************************/
void LowPowerSet(void)
{
    P1SEL = 0x00;                           //  a4 a6
    P1SEL2 =0x00; 
    P1REN |= 0x06;
    P1OUT |= 0x06; //P11��P12����
    P1OUT &= 0xfb; //P12����
}
/**********************************************************************************
  ������:  WorkModeSetting
  ��  ��:  
**********************************************************************************/
void FallPower(void)
{
//  Flag.Power_on = 0;
//  Time_Power_On = 0;
    RF_POWER_OFF();                     // �ر�RFģ���Դ
    POWEER_DETECT_OFF();               // �ص�Դ�����ѹ���
    MotorStop();     
    BUZZER_OFF();
    RF_SLEEP();
}

/**********************************************************************************
  ������:  WorkModeSetting
  ��  ��:  
**********************************************************************************/
void WorkModeSetting  (Work_Status_TypeDef mode)
{
  if (mode == NORMAL_MODE)                      // �Ƿ�����ģʽ
  {
    GPIOSetting();
    CLKSetting();                               // ʱ������ 
    TIMXSetting();                              // ��ʱ������
    ADCSetting();                               // ADC ����
    LinUartInit();
    
    RF_POWER_ON();                              // ��RFģ���Դ
    POWEER_DETECT_ON();                         // ����Դ�����ѹ���
    Flag.Power_Down = 0;
    
    
    _DINT();                            // �����ж� 
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;
    
    Work_Status = NORMAL_MODE;                  // ��������ģʽ
    
    P2IFG = 0;
    P2IES = BIT6;                               // P2.6 �½����ж�
    P2IE  = BIT6;                               // ��P2.6(�ϵ���)�жϣ��ذ�����RF_DATA�ж�
    P2IFG = 0;

    INT_CHARGE(INT_CLOSE);//P1�ж�����,ֻ����ж�ʹ��
	
    BCSCTL2 = 0;                                // Ƶ�ʻָ�16M
    
     // Time0_A0 ����
    TA0CCTL0 = CCIE;                             // CCR0 interrupt enabled
    TA0CCR0 += RF_DECODE_TIME * SYSTEM_CLOCK;    // 40 uS
    
    // Time0_A1 ����
    TA0CCTL1 = CCIE;                             // CCR1 interrupt enabled
    TA0CCR1 += SYSTEM_TIME_VALUE * SYSTEM_CLOCK;     // 2mS
    TA0CTL = TASSEL_2 + MC_2;                    // SMCLK, Continous up
    
    if (Flag.RF_Wakeup)
    {
      Flag.RF_Wakeup = 0;
      RFDataDisposal();                           // RF���ݴ���
    }
    _EINT();                                   // �����ж� 
  }
  else if (mode == SLEEP_MODE)        // ˯��ģʽ
  {
    /*RF_POWER_ON();                     // ��RFģ���Դ
    BRAKE_ON();                        // ɲ�����Ͽ��̵���
    POWEER_DETECT_OFF();               // �رյ�Դ�����ѹ���
    MotorStop();                       // ���ֹͣ
    RF_SLEEP();
    
    Flag.Power_Down = 0;
    BUZZER_OFF();                      // �ط�����
    */
    FallPower();
    RF_POWER_ON(); 
    BRAKE_ON(); 
    Flag.Power_Down = 0;
    
    // ֹͣADC�������ر�ADCģ��
    P1SEL = 0x00;                           //  a4 a6
    ADC10CTL0 &= SREF_1 + ADC10SHT_2 + REFON + ADC10ON + REF2_5V;                     //  a4 a6
    ADC10AE0 &= 0x50;
    ADC10CTL0 &= ENC + ADC10SC;             // Sampling and conversion start
    
    _DINT();                            // �����ж� 
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;
    
    Work_Status = SLEEP_MODE;          // ����˯��ģʽ
    P1SEL = 0x00;                      // No 
    P2SEL = 0x00;                      // No XTAL 
    P3SEL = 0x00;                      // No XTAL 
    P2IFG = 0;
    Goto_Sleep_Cnt = 0;
    
    BCSCTL2 = 0x24;                    // 8M 4��Ƶ  ��Ƶ2M
    FCTL2 = FWKEY + FSSEL0 + FN4;      // MCLK/3 for Flash Timing Generator
    
    // Time0_A0 ����
    TA0CCTL0 = CCIE;                           // CCR0 interrupt enabled
    TA0CCR0 += (RF_DECODE_TIME * SYSTEM_CLOCK) >> 2;  // 40 uS

    // Time0_A1 ����
    TA0CCTL1 = CCIE;                           // CCR1 interrupt enabled
    TA0CCR1 += (SYSTEM_TIME_VALUE * SYSTEM_CLOCK) >> 2;     // 2mS
    TA0CTL = TASSEL_2 + MC_2;                  // SMCLK, Continous up
   
    LinUartInit();  
    UCA0BR0 = (((1000000/(19200)) * (SYSTEM_CLOCK>>2))) & 0xFF;         // ����������
  	UCA0BR1 = (((1000000/(19200)) * (SYSTEM_CLOCK>>2)) >> 8); 
   
    P2IES = BIT3 + BIT7 + BIT6;        // Bit = 0: The PxIFGx flag is set with a low-to-high transition
    P2IE  = BIT3 + BIT7 + BIT6;        // ��P2.3  P2.5  P2.6 �ж�
    
     P2IFG = 0;
	 
 
    
     P1IFG = 0;
    _EINT();                                   // �����ж� 
  }
  else if (mode == POWER_OFF_MODE)    /* �ػ�ģʽ --------*/
  {
    u16 i;
    
  	IE2 &= ~(UCA0RXIE + UCA0TXIE);                         // Enable USCI_A0 RX interrupt
	/*	
    RF_POWER_OFF();                     // ��RFģ���Դ
    BRAKE_ON();                        // ɲ�����Ͽ��̵���
    POWEER_DETECT_OFF();               // �رյ�Դ�����ѹ���
    MotorStop();                       // ���ֹͣ
    BUZZER_OFF();                      // �ط�����
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

     // ֹͣADC�������ر�ADCģ��
    LowPowerSet();
    ADC10CTL0 &= SREF_1 + ADC10SHT_2 + REFON + ADC10ON + REF2_5V;                     //  a4 a6
    ADC10AE0 &= 0x50;
    ADC10CTL0 &= ENC + ADC10SC;             // Sampling and conversion start
    

    _DINT();                            // �����ж� 
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;
    
    Work_Status = POWER_OFF_MODE;
    
    //P1SEL = 0x00;                      // No 
    P2SEL = 0x00;                      // No XTAL 

    P3SEL = 0x00;                      // No XTAL 
    P2IFG = 0;
    Goto_Sleep_Cnt = 0;
    
    BCSCTL2 = 0x24;                    // 8M 4��Ƶ  ��Ƶ2M
    FCTL2 = FWKEY + FSSEL0 + FN4;      // MCLK/3 for Flash Timing Generator
    
    // Time0_A0 ����
    TA0CCTL0 = CCIE;              // CCR0 interrupt enabled
    TA0CCR0 += (RF_DECODE_TIME * SYSTEM_CLOCK) >> 2;  // 40 uS

    // Time0_A1 ����
    TA0CCTL1 = CCIE;             // CCR1 interrupt enabled
    TA0CCR1 += (SYSTEM_TIME_VALUE * SYSTEM_CLOCK) >> 2;     // 2mS
    TA0CTL = TASSEL_2 + MC_2;   // SMCLK, Continous up
    
    P2IES = BIT7 + BIT6;        // Bit = 0: The PxIFGx flag is set with a low-to-high transition
    P2IE  = BIT7 + BIT6;        // ��P2.3  P2.5  P2.6 �ж�
    P2IFG = 0;
    
    _EINT();                                   // �����ж� 
    Flag.Set_Key_ON = 0;
    
     for (i = 0; i < 3; i++)
    {
      LED_RED_ON();
      Delay(330000);                       // �ӳ�          
      LED_RED_OFF();      
      Delay(330000);                       // �ӳ�
    }
	
	_NOP();
  }
  else if (mode == POWER_DOWN_MODE)    /* ����ģʽ --------*/
  {
    //u8 i;
    /*
    BUZZER_OFF();
    MotorStop();                        // ���ֹͣ
    RF_POWER_OFF();                     // �ر�RFģ���Դ
    RF_SLEEP();
    POWEER_DETECT_OFF();               // �ص�Դ�����ѹ���
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
    // ֹͣADC�������ر�ADCģ��
    P1SEL = 0x00;                           //  a4 a6
    ADC10CTL0 &= SREF_1 + ADC10SHT_2 + REFON + ADC10ON + REF2_5V;                     //  a4 a6
    ADC10AE0 &= 0x50;
    ADC10CTL0 &= ENC + ADC10SC;             // Sampling and conversion start
    
    
    _DINT();                            // �����ж� 
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;
    
    BCSCTL2 = 0x24;                    // 8M 4��Ƶ
    FCTL2 = FWKEY + FSSEL0 + FN4;      // MCLK/3 for Flash Timing Generator
    
    P2IES = 0;                         // Bit = 0: The PxIFGx flag is set with a low-to-high transition
    P2IE  = 0;                         // ��P2.3  P2.6  P2.7�ж�
    P2IFG = 0;
    
    GPIOSetting();
    LowPowerSet();
    // Time0_A0 ����
    TA0CCTL0 = CCIE;                           // CCR0 interrupt enabled
    TA0CCR0 += RF_DECODE_TIME * SYSTEM_CLOCK;  // 

    // Time0_A1 ����
    TA0CCTL1 = CCIE;                           // CCR1 interrupt enabled
    TA0CCR1 += (SYSTEM_TIME_VALUE * SYSTEM_CLOCK) >> 2;     // 2mS
    TA0CTL = TASSEL_2 + MC_2;                  // SMCLK, Continous up
    IE2 &= ~(UCA0RXIE + UCA0TXIE);
    Work_Status = POWER_DOWN_MODE;
    _EINT();                                   // �����ж� 
    
    PowerDown_Time_Cnt = 0;
  }
}