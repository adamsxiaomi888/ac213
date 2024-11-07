/**********************************************************************************
 *  ��Ŀ���:    AC213-04
 *  ��Ŀ����:    25�͹���
 *  �ͻ�����:                                    
 *  �����д:    ��С��
 *  ��������:    2012-09-15
 *  �������:    
 *  �汾����:    V2.4
 *
 *  Extern.h file
 *  ��������: �ⲿ��������������
**********************************************************************************/
#ifndef Extern_h
  #define Extern_h

  /* ȫ��λ�������� -------------------------------------------------------------*/

  extern struct 
  {
  u8 Signal_Status             : 1;                          /* RF��ƽ״̬��־ --*/
  u8 Key_Disposal              : 1;                          /* RF�����־ ------*/
  u8 Signal_Head               : 1;                          /* RF�ź�ͷ��־ ----*/
  
  }Flag_RF;

  extern __no_init struct 
  {
  u8 Motor_Direction           : 1;                          /* ��������־ ----*/
  u8 Motor_Fact_Direction      : 1;                          /* ���ʵ�ʷ����־ */
  u8 Motor_Run                 : 1;                          /* ������б�־ ----*/
  u8 Hall_Pulse_edge           : 1;                          /* ����������ر�־ */
  u8 Direction                 : 1;                          /* �����־ --------*/
  u8 Single_Journey            : 1;                          /* �����г̱�־ ----*/
  u8 Journey_Set_OK            : 1;                          /* �г��趨OK��־ --*/
  u8 Journey_Direction         : 1;                          /* �г̷����־ ----*/
  u8 EE_Save                   : 1;                          /* �ϵ��־ --------*/
  u8 Hall_Power                : 1;                          /* ������Դ��־ ----*/
  u8 Motor_Jiggle_Status       : 1;                          /* ����㶯���б�־ */
  u8 Run_Status                : 1;                          /*�������״̬*/
  u8 StatusSwitch              : 1;                           /*����Ŀ��λ��*/
  }MotorFlag;

  extern struct 
  {
  u8 RF_Rx_Data                : 1;  
  u8 RF_Raise_Edge             : 1;                          /* RF�����ر�־ ----*/
  u8 RF_Rx_Finish              : 1;                          /* RF�������ݱ�־ --*/
  u8 RF_Data_Disposal          : 1;                          /* RF���ݴ����־ --*/
  u8 Time_2ms                  : 1;                          /* 2msʱ���־ -----*/
  u8 Time_10ms                 : 1;                          /* 10msʱ���־ ----*/
  u8 Time_50ms                 : 1;                          /* 50msʱ���־ ----*/
  u8 Time_100ms                : 1;                          /* 100msʱ���־ ---*/
  u8 Time_500ms                : 1;
  u8 Time_1s                   : 1;                          /* 1sʱ���־ ------*/
  
  u8 Learn_Code_Statu          : 1;                          /* ѧ��״̬��־ ----*/
  u8 Set_Key_ON                : 1;                          /* ���ü����±�־ --*/
  u8 Opretion_Finish           : 1;                          /* �����ɹ���־ ----*/
  
  u8 Delete_Code               : 1;                          /* ɾ���־---------*/
  u8 Del_Code_Finish           : 1;                          /* ɾ��ɹ���־-----*/
  u8 Electrify_Reset           : 1;                          /* �ϵ縴λ��־ ----*/
  u8 StatusSwitch_Delay        : 1;                          /* ��������ӳٱ�־ */
  
  u8 Delete_single_Code        : 1;                          /* ɾ���������־ --*/
  u8 Set_Limit_Point           : 1;                          /* ������λ���־ --*/
  u8 Set_Journey_Direction     : 1;                          /* �����г̷����־ */
  
  u8 RF_UP_dblclick            : 1;                          /* �������Ʊ�־ ----*/
  u8 RF_DOWN_dblclick          : 1;                          /* �������Ʊ�־ ----*/
  u8 Run_To_finishing_point    : 1;                          /* ���е��յ��־ --*/
  u8 Power_Cut                 : 1;                          /* �ϵ��־ --------*/
  
  u8 Save_Limit_Point          : 1;                          /* ������λ���־ --*/
  u8 Delete_Limit_Point        : 1;                          /* ɾ����λ���־ --*/
  
  u8 OprationCue_Run           : 1;                          /* ������б�־ ----*/
  u8 OprationCue_Stop          : 1;                          /* ������б�־ ----*/
  u8 Jiggle_Stop               : 1;                          /* �㶯ֹͣ��־ ----*/
  
  u8 Joint_Action              : 1;                          /* ������־ ----*/
  u8 Joint_Level_Adj           : 1;                          /*  ----------------*/
  
  u8 Buzzer_Hint               : 1;                          /* ��������ʾ��־ --*/
  u8 Low_VOL_Jiggle            : 1;                          /* �㶯ֹͣ��־ ----*/
  u8 Learn_Key_Hint            : 1;                          
   
  u8 RF_Wakeup                 : 1;   
  u8 Power_Down                : 1;   
  u8 Key_Wakeup                : 1; 
  
  u8 NO_Comper_Limit_Point     : 1;
  u8 Motor_Status_Switch       : 1;
  u8 slowstop                  : 1;
  u8 Uart_rx_OK                : 1;
  u8 Uart_Disposal             : 1;
  u8 Uart_Read_OK              : 1;
  u8 Goto_PCT                  : 1;
  u8 Uart_tx                   : 1;
  u8 Clear_status              : 1;
  u8 Clear_WIFI_Time_End       : 1;
  u8 Clear_status_hint         : 1;
  u8 net_successful_count      : 1;        //������ʱ��־
  
  
  }Flag;
  
  extern struct 
{
  u8 UP_Dispose                : 1;
  u8 DOWN_Dispose              : 1;
  u8 STOP_Dispose              : 1;
  u8 SET_Dispose               : 1;
  
  u8 CLR_Disposal              : 1;
  u8 CLR_Status                : 1;
  u8 DELETE_ALL                : 1;
}KeyFlag;


  /* ȫ�ֱ������� ---------------------------------------------------------------*/
  extern u8 RF_High_Cnt;                                     /* RF�ߵ�ƽʱ�� ----*/
  extern u8 RF_Low_Cnt;                                      /* RF�׵�ƽʱ�� ----*/
  
  extern u8 Time_10ms_Cnt;
  extern u8 Time_50ms_Cnt;
  extern u8 Time_100ms_Cnt;
  extern u8 Time_500ms_Cnt;
  extern u8 Time_1s_Cnt;
  extern u8 NET_Succe_Time_Cnt;     //  
  extern u8 Clear_Status_Time_Cnt;
  extern u8 Clear_Status_End_Time_Cnt;
    
  extern u8 Motor_Status;                                    /* ���״̬ --------*/
  extern u8 Motor_Status_Buffer;
  extern u8 Old_Motor_Status;

  extern u32  Hall_Pulse_Amount_PCT;                          /* �������������� */
  extern u32  OprationCue_Amount_Buf;                         /* �������������� */
  extern u32 Hall_Pulse_Amount;
  extern u32 Hall_Pulse_Jiggle_Buff;
  
  extern u8 Delete_single_Code_ID[6];
  extern u8 Opretion_Event;
  extern u8 Clear_time;
  
  extern __no_init u16 Hall_Pulse_Width;                               /* ������������� */
  extern __no_init u32  Hall_Pulse_Amount_Cnt;                          /* �������������� */
  extern __no_init Limit Limit_Point[MID_LIMIT_POINT_AMOUNT + 2];
  extern __no_init u8 Limit_Point_Amount;
  extern __no_init u8 Hall_Status_Buffer;
  extern __no_init u8 hall_change_cnt;
  extern __no_init u16 Hall_Time_Cnt;
  
  extern __no_init u16 Current_Voltage;                                /* ��ǰ�ĵ�ѹֵ ----*/
  extern __no_init u16 Power_Down_Voltage;                            /* ��ǰ�ĵ�ѹֵ ----*/
  
  extern __no_init u16 PowerDown_Time_Cnt;
  extern __no_init u8 Power_Down_Cnt;
  extern __no_init u16 Low_VOL_Cnt;
  extern __no_init u16 VOL_Recover_Cnt;
  extern __no_init u8 Joint_Time_Level;
  
  
  extern u8 Target_Limit_Point;
  extern u8 RF_Dblclick_Time;
  extern u8 Run_To_Mid_Limit_Point;
  extern u8 motor_run_time;
  extern u8 motor_off_time;
  extern u8 Electric_Quantity;
  extern u16 Current_Current_mA;                             /* ��ǰ�ĵ���ֵ ----*/
  
  extern u8 Motor_Speed;                                     /* ���ת�� --------*/
  extern u8 Target_Speed;
  extern u16 Motor_Duty;
  
  extern u8 Set_Limit_Point_Cnt;
  extern volatile u8 Motor_Max_Speed;                             /* ������ת�� ---*/
  
  extern u16 Time_1s_Hint_Cnt;
  extern u8 Delete_single_Code_Cnt;
  extern u8 PowerDown_Delay_Time;
  extern u8 Low_Vol_Hint_Event;
  extern Work_Status_TypeDef Work_Status;
  extern u8 Goto_Sleep_Cnt;
  
  extern u16 Key_Wakeup_time_cnt;
  
  extern u16 Set_Key_Cnt;
  
  extern u8 RF_High_Cnt_Buffer;                                /* ����ʱ�仺�� ----*/
  extern u8 RF_Low_Cnt_Buffer;                                 /* ����ʱ�仺�� ----*/
  extern u8 Joint_Action_Motor_Buf;
  extern u16 Joint_Action_Time_Cnt;
  extern u16 Joint_Level_ADJ_Time;
  extern u8 Uart_NO_Rx_Cnt;
  extern u8 UART_TX_num;
  extern u8 UART_TX_time;
  extern u8 Uart_ID;
  extern u8 Uart_CH_L;
  extern u8 Uart_CH_H;
  extern u8 Uart_TX_Byte_Total;
  extern u8 Uart_Tx_Array[20];
  extern u8 PercentCount;
  extern u8 xianweidian;
  /* �������� -------------------------------------------------------------------*/
  extern void Initialize        (void);                      /* ��ʼ������ ------*/
  extern void GPIOSetting       (void);
  extern void CLKSetting        (void);
  extern void ADCSetting        (void);
  extern void TIMXSetting       (void);
  extern void Delay             (register u32 i);
  
  extern void Clear_WIFI_Hint(void);
  extern void KeyManage         (void);                      /* ����ɨ�輰���� --*/
  extern void TimeControl       (void);                      /* ϵͳʱ����� ----*/
  extern void LearnDelCodeTime  (void);                      /* ѧ��ɾ��ʱ����� */
  
  
  extern void ErrorDisposal     (void);
  extern u8   RFSignalDecode    (u16 High_Time,u16 Low_Time);/* RF���뺯�� ------*/
  extern u8   RFDataVerify      (void);                      /* RF����У�� ------*/
  extern void RFDataDisposal    (void);                      /* RF������ ------*/
  extern void RFStatusInspect   (void);
  
  extern u8   CompID            (u8 array[]);                /* �Ƚ��Ƿ�����ͬID */
  extern void SaveID            (u8 array[]);                /* �����µ�ID ------*/
  extern void DeleteAllID       (u8 check);                      /* ɾ�����е�ID ----*/
  extern void DeleteChannel     (u8 array[],u8 ch_addr);     /* ɾ��ͨ�� --------*/
  extern void SaveChannel       (u8 array[],u8 ch_addr);     /* ����ͨ�� --------*/
  
  extern void MotorControl      (void);                      /* ������� --------*/
  extern void MotorStatusSwitch (void);                      /* ���״̬�л� ----*/
  extern void MotorStop         (void);
  extern void MotorDown         (u16 duty);
  extern void MotorUp           (u16 duty);
  extern void CountMotorSpeed   (void);
  extern void PowerControl      (void);
  
  extern void PowerOffControl   (void);
  extern void PowerDownDetect   (void);
  
  extern void OprationCue       (void);
  extern u8   LoopCompare        (u32 data,u8 type);
  extern void JourneyComper     (void);
  extern void DblclickTimeLimit (void);
  extern u8   MeetPlugbDetect    (void);
  extern void SetLimitPoint     (void);
  
  
  extern void FlashReadData     (void);
  extern void FlashWriteData    (u8 check);
  extern void PowerDownSaveData (void);
  extern void PowerDownSamping  (void);
  
  extern u16  SampleAverage      (u16 array[],u8 num);
  extern void ADSamping         (void);
  extern void VolSamping        (void);
  extern void VoltageProtect    (void);
  extern void WorkTimeControl   (void);
  extern void SleepControl      (void);
  extern void WorkModeSetting   (Work_Status_TypeDef mode);
  extern void PowerDownHandle   (void);                                     // ���紦��
  extern void RFWakeup          (void);
  extern u8   LimitCollator      (Limit array[]);
  extern void Duty_Sub          (u16 *p,u8 i);
  extern u8 DataCRC (u8 start_element,const u8 *array,u8 amount);
  extern void UartStartupTX (void);
  extern void Send_Pair_WIFI_Order (void);
  extern void LinUartInit (void);
  extern void LinUartInit1 (void);
  extern void UartDataDisposal (void);
  extern void Send_Clear_WIFI_Order (void);
  extern void InquirePercentCount(void);
  extern void UartUpload(u8 dataType, u8 dataLength, const u8 *data, u8 frameNum, u8 ResendTime);
  extern u8 JourneyPercentCount (void);
  extern void VariableInitialize (void);


  extern void INT_CHARGE(u8 int_enable);//����ж�����



#endif
