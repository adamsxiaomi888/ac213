/**********************************************************************************
 *  ��Ŀ���:    AC213-04
 *  ��Ŀ����:    25�͹���
 *  �ͻ�����:                                    
 *  �����д:    ��С��
 *  ��������:    2012-09-15
 *  �������:    
 *  �汾����:    V2.4
 
 *  Initialize.C file
 *  ��������: ϵͳ��ʼ��
 *********************************************************************************/
#include "Define.h"                                          /* ȫ�ֺ궨�� ------*/
#include "Extern.h"                                          /* ȫ�ֱ��������� --*/
#include <string.h>
/**********************************************************************************
  ������:   Delay
  ��  ��:    
*********************************************************************************/
void Delay (register u32 i)
{
  for (; i > 0; i--)
  {
    //WDTCTL = WDT_ARST_16;
  }
}
/*********************************************************************************
  ������:   CLKSetting
  ��  ��:    
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
  ������:   TIMXSetting
  ��  ��:    
*********************************************************************************/
void TIMXSetting (void)
{
  TA0CTL = TACLR;       // ��ʱ������
  
  // Time0_A1 ����
  P2SEL |= BIT2 + BIT5;                      // P2.2 and P2.5 TA1/2 options
  P3SEL |= BIT2 + BIT3;                      // P3.2 and P3.3 TA1/2 options
  
  TA1CCTL0 = 0;                              // CCR0 interrupt enabled
  TA1CCR0 = MAX_DUTY + 2;                    // CCR1 PWM duty cycle 2:100
  TA1CCTL1 = OUTMOD_2;                       // CCR1 toggle/reset
  TA1CCTL2 = OUTMOD_2;                       // CCR2 toggle/reset
  MotorStop();
  TA1CTL = TASSEL_2 + ID_0 + MC_3;           // SMCLK, ����Ƶ up down mode
}
/*********************************************************************************
  ������:   GPIOSetting
  ��  ��:    
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
  ������:   VariableInitialize
  ��  ��:    
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
    
  MotorFlag.Motor_Direction           = 0;                          /* ��������־ ----*/
  MotorFlag.Motor_Fact_Direction      = 0;                          /* ���ʵ�ʷ����־ */
  MotorFlag.Motor_Run                 = 0;                          /* ������б�־ ----*/
  MotorFlag.Hall_Pulse_edge           = 0;                          /* ����������ر�־ */
  MotorFlag.Direction                 = 0;                          /* �����־ --------*/
  //MotorFlag.Single_Journey            = 0;                          /* �����г̱�־ ----*/
//  MotorFlag.Journey_Set_OK            = 0;                          /* �г��趨OK��־ --*/
  MotorFlag.Journey_Direction         = 0;                          /* �г̷����־ ----*/
  MotorFlag.EE_Save                   = 0;                          /* �ϵ��־ --------*/
  MotorFlag.Hall_Power                = 0;                          /* ������Դ��־ ----*/

}
/*********************************************************************************
  ������:   Initialize
  ��  ��:    
  ��  ��:   ��
  ��  ��:   ��    
  ��  ��:   ��
  ��  ����
*********************************************************************************/
void Initialize (void)
{
  #define MOTOR_RUN                 (200 / (TIME_10MS_VALUE * SYSTEM_TIME))
  
  _DINT();                            // �����ж� 
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  LED_RED_ON();   
  LED_BLUE_ON();

  IN1_H_0();
  IN1_L_0();
  IN2_H_0();
  IN2_L_0();                          // �ر��������
  
  BRAKE_ON();                         // ɲ�����Ͽ��̵���
  RF_POWER_OFF();                     // �ر�RFģ���Դ
  RF_SLEEP();
  BUZZER_OFF();                       // �������ر�
  POWEER_DETECT_OFF();                // �رյ�Դ�����ѹ���
  
  CLKSetting();                       // ʱ������ 
  GPIOSetting();                      // IO ������
  
  
  HALL_POWEER_ON();                   // ��������Դ
  RF_POWER_ON();                      // ��RFģ���Դ
  POWEER_DETECT_ON();                 // ����Դ�����ѹ���

    // �Ƿ��ϵ縴λ
  if(IFG1 & PORIFG)
  {
    Delay(60000);                     // �ϵ��ӳ�
    
    ADCSetting();                       // ADC ����
    VariableInitialize();
    
    FlashReadData();
    Flag.Learn_Key_Hint = TRUE;         // ��������һ����ʾ����
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
  
  TIMXSetting();                      // ��ʱ������
  
  WorkModeSetting(NORMAL_MODE);       // ���빤��ģʽ
  MotorFlag.Motor_Run = NULL;
  Motor_Status        = MOTOR_STOP;
  Motor_Status_Buffer = MOTOR_STOP;
  
  MotorFlag.Hall_Power = TRUE;
  

  LinUartInit();
  /*
  for(u8 i=0;i<255;i++)
  Delay(60000);                     // �ϵ��ӳ�
  */
  Delay(60000);                     // �ϵ��ӳ�
 // Send_Pair_WIFI_Order();
  IFG1 = 0;
  _EINT();                            // �����ж� 

  //u8 data[] = {0x02};
  //UartUpload(0xe1, sizeof(data), data, 3, 0);
}

