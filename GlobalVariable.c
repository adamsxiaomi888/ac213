/**********************************************************************************
 *  ��Ŀ���:    AC213-04
 *  ��Ŀ����:    25�͹���
 *  �ͻ�����:                                    
 *  �����д:    ��С��
 *  ��������:    2012-09-15
 *  �������:    
 *  �汾����:    V2.4
 *
 *  GlobalVariable.c file
 *  ��������: ȫ�ֱ�������
**********************************************************************************/
#include "Define.h"                                          /* ȫ�ֺ궨�� ------*/
/* ȫ��λ�������� ---------------------------------------------------------------*/
struct 
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

struct 
{
  u8 Signal_Status             : 1;                          /* RF��ƽ״̬��־ --*/
  u8 Key_Disposal              : 1;                          /* RF�����־ ------*/
  u8 Signal_Head               : 1;                          /* RF�ź�ͷ��־ ----*/
}Flag_RF;


__no_init struct 
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

struct 
{
  u8 UP_Dispose                : 1;
  u8 DOWN_Dispose              : 1;
  u8 STOP_Dispose              : 1;
  u8 SET_Dispose               : 1;
  
  u8 CLR_Disposal              : 1;
  u8 CLR_Status                : 1;
  u8 DELETE_ALL                : 1;
 
}KeyFlag;


/* ȫ�ֱ������� -----------------------------------------------------------------*/
u8 Time_10ms_Cnt = TIME_10MS_VALUE;
u8 Time_50ms_Cnt = TIME_50MS_VALUE;
u8 Time_100ms_Cnt = TIME_100MS_VALUE;
u8 Time_500ms_Cnt = TIME_500MS_VALUE;
u8 Time_1s_Cnt = TIME_1S_VALUE;



u8 NET_Succe_Time_Cnt;     //
u8 Clear_Status_Time_Cnt;     //
u8 Clear_Status_End_Time_Cnt;
u8 RF_High_Cnt;                                              /* RF�ߵ�ƽʱ�� ----*/
u8 RF_Low_Cnt;                                               /* RF�׵�ƽʱ�� ----*/
u8 Motor_Status;                                             /* ���״̬ --------*/
u8 Motor_Status_Buffer;
u8 Old_Motor_Status=0;                                         /*��һ�ε��״̬*/
u8 Clear_time;
u8 PercentCount;
u8 xianweidian;
__no_init u16 Hall_Pulse_Width;                              /* ������������� */
__no_init u32 Hall_Pulse_Amount_Cnt;                         /* �������������� */
__no_init Limit Limit_Point[MID_LIMIT_POINT_AMOUNT + 2];
__no_init u8 Limit_Point_Amount;
__no_init u8 Hall_Status_Buffer;
__no_init u8 hall_change_cnt;
__no_init u16 Hall_Time_Cnt;
__no_init u16 Current_Voltage;                               /* ��ǰ�ĵ�ѹֵ ----*/
__no_init u16 Power_Down_Voltage;                            /* ��ǰ�ĵ�ѹֵ ----*/
__no_init u16 PowerDown_Time_Cnt;

__no_init u8 Power_Down_Cnt;
__no_init u16 Low_VOL_Cnt;
__no_init u16 VOL_Recover_Cnt;
__no_init u8 Joint_Time_Level;
  
u32 Hall_Pulse_Amount_PCT = MIN_JOURNEY_VALUE;               /* �������������� */
u32 OprationCue_Amount_Buf;                                  /* �������������� */
u32 Hall_Pulse_Amount;
u32 Hall_Pulse_Jiggle_Buff;

u8 Delete_single_Code_ID[6];

u8 Target_Limit_Point;
u8 RF_Dblclick_Time;
u8 Run_To_Mid_Limit_Point;
u8 Electric_Quantity;

u16 Current_Current_mA;                                      /* ��ǰ�ĵ���ֵ ----*/

u16 Motor_Duty;
u8 Motor_Speed;                                              /* ���ת�� --------*/
u8 Target_Speed;
u8 motor_run_time;
u8 motor_off_time;

u8 Set_Limit_Point_Cnt;
u8 Opretion_Event;
volatile u8 Motor_Max_Speed;                             /* ������ת�� ---*/

u16 Time_1s_Hint_Cnt;
u8 Delete_single_Code_Cnt;
u8 PowerDown_Delay_Time;
u8 Low_Vol_Hint_Event;
Work_Status_TypeDef Work_Status = NORMAL_MODE;
u8 Goto_Sleep_Cnt;


u16 Key_Wakeup_time_cnt;
u8 Joint_Action_Motor_Buf;
u16 Joint_Action_Time_Cnt;
u16 Joint_Level_ADJ_Time;
u8 Uart_NO_Rx_Cnt = 0;
u8 UART_TX_num;
u8 UART_TX_time=3;
u8 Uart_ID;
u8 Uart_CH_L;
u8 Uart_CH_H;
u8 Uart_TX_Byte_Total;
u8 Uart_Tx_Array[20];
