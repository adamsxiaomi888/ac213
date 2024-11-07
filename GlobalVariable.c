/**********************************************************************************
 *  项目编号:    AC213-04
 *  项目名称:    25低功耗
 *  客户名称:                                    
 *  程序编写:    符小玲
 *  启动日期:    2012-09-15
 *  完成日期:    
 *  版本号码:    V2.4
 *
 *  GlobalVariable.c file
 *  功能描述: 全局变量定义
**********************************************************************************/
#include "Define.h"                                          /* 全局宏定义 ------*/
/* 全局位变量定义 ---------------------------------------------------------------*/
struct 
{
  u8 RF_Rx_Data                : 1;  
  u8 RF_Raise_Edge             : 1;                          /* RF上升沿标志 ----*/
  u8 RF_Rx_Finish              : 1;                          /* RF接收数据标志 --*/
  u8 RF_Data_Disposal          : 1;                          /* RF数据处理标志 --*/
  u8 Time_2ms                  : 1;                          /* 2ms时间标志 -----*/
  u8 Time_10ms                 : 1;                          /* 10ms时间标志 ----*/
  u8 Time_50ms                 : 1;                          /* 50ms时间标志 ----*/
  u8 Time_100ms                : 1;                          /* 100ms时间标志 ---*/
   u8 Time_500ms                : 1;
  u8 Time_1s                   : 1;                          /* 1s时间标志 ------*/
  
  u8 Learn_Code_Statu          : 1;                          /* 学码状态标志 ----*/
  u8 Set_Key_ON                : 1;                          /* 设置键按下标志 --*/
  u8 Opretion_Finish           : 1;                          /* 操作成功标志 ----*/
  
  u8 Delete_Code               : 1;                          /* 删码标志---------*/
  u8 Del_Code_Finish           : 1;                          /* 删码成功标志-----*/
  u8 Electrify_Reset           : 1;                          /* 上电复位标志 ----*/
  u8 StatusSwitch_Delay        : 1;                          /* 电机换挡延迟标志 */
  
  u8 Delete_single_Code        : 1;                          /* 删除单个码标志 --*/
  u8 Set_Limit_Point           : 1;                          /* 设置限位点标志 --*/
  u8 Set_Journey_Direction     : 1;                          /* 设置行程方向标志 */
  
  u8 RF_UP_dblclick            : 1;                          /* 连续控制标志 ----*/
  u8 RF_DOWN_dblclick          : 1;                          /* 连续控制标志 ----*/
  u8 Run_To_finishing_point    : 1;                          /* 运行到终点标志 --*/
  u8 Power_Cut                 : 1;                          /* 上电标志 --------*/
  
  u8 Save_Limit_Point          : 1;                          /* 保存限位点标志 --*/
  u8 Delete_Limit_Point        : 1;                          /* 删除限位点标志 --*/
  
  u8 OprationCue_Run           : 1;                          /* 电机下行标志 ----*/
  u8 OprationCue_Stop          : 1;                          /* 电机下行标志 ----*/
  u8 Jiggle_Stop               : 1;                          /* 点动停止标志 ----*/
  
  u8 Joint_Action              : 1;                          /* 连动标志 ----*/
  u8 Joint_Level_Adj           : 1;                          /*  ----------------*/
  
  u8 Buzzer_Hint               : 1;                          /* 蜂鸣器提示标志 --*/
  u8 Low_VOL_Jiggle            : 1;                          /* 点动停止标志 ----*/
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
  u8 net_successful_count      : 1;        //配网计时标志
 
}Flag;

struct 
{
  u8 Signal_Status             : 1;                          /* RF电平状态标志 --*/
  u8 Key_Disposal              : 1;                          /* RF处理标志 ------*/
  u8 Signal_Head               : 1;                          /* RF信号头标志 ----*/
}Flag_RF;


__no_init struct 
{
  u8 Motor_Direction           : 1;                          /* 电机方向标志 ----*/
  u8 Motor_Fact_Direction      : 1;                          /* 电机实际方向标志 */
  u8 Motor_Run                 : 1;                          /* 电机运行标志 ----*/
  u8 Hall_Pulse_edge           : 1;                          /* 霍尔脉冲边沿标志 */
  u8 Direction                 : 1;                          /* 方向标志 --------*/
  u8 Single_Journey            : 1;                          /* 单个行程标志 ----*/
  u8 Journey_Set_OK            : 1;                          /* 行程设定OK标志 --*/
  u8 Journey_Direction         : 1;                          /* 行程方向标志 ----*/
  u8 EE_Save                   : 1;                          /* 上电标志 --------*/
  u8 Hall_Power                : 1;                          /* 霍尔电源标志 ----*/
  u8 Motor_Jiggle_Status       : 1;                          /* 电机点动运行标志 */
  u8 Run_Status                : 1;                          /*电机运行状态*/
  u8 StatusSwitch              : 1;                           /*更新目标位置*/
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


/* 全局变量定义 -----------------------------------------------------------------*/
u8 Time_10ms_Cnt = TIME_10MS_VALUE;
u8 Time_50ms_Cnt = TIME_50MS_VALUE;
u8 Time_100ms_Cnt = TIME_100MS_VALUE;
u8 Time_500ms_Cnt = TIME_500MS_VALUE;
u8 Time_1s_Cnt = TIME_1S_VALUE;



u8 NET_Succe_Time_Cnt;     //
u8 Clear_Status_Time_Cnt;     //
u8 Clear_Status_End_Time_Cnt;
u8 RF_High_Cnt;                                              /* RF高电平时间 ----*/
u8 RF_Low_Cnt;                                               /* RF底电平时间 ----*/
u8 Motor_Status;                                             /* 电机状态 --------*/
u8 Motor_Status_Buffer;
u8 Old_Motor_Status=0;                                         /*上一次电机状态*/
u8 Clear_time;
u8 PercentCount;
u8 xianweidian;
__no_init u16 Hall_Pulse_Width;                              /* 霍尔输出脉冲宽度 */
__no_init u32 Hall_Pulse_Amount_Cnt;                         /* 霍尔输出脉冲计数 */
__no_init Limit Limit_Point[MID_LIMIT_POINT_AMOUNT + 2];
__no_init u8 Limit_Point_Amount;
__no_init u8 Hall_Status_Buffer;
__no_init u8 hall_change_cnt;
__no_init u16 Hall_Time_Cnt;
__no_init u16 Current_Voltage;                               /* 当前的电压值 ----*/
__no_init u16 Power_Down_Voltage;                            /* 当前的电压值 ----*/
__no_init u16 PowerDown_Time_Cnt;

__no_init u8 Power_Down_Cnt;
__no_init u16 Low_VOL_Cnt;
__no_init u16 VOL_Recover_Cnt;
__no_init u8 Joint_Time_Level;
  
u32 Hall_Pulse_Amount_PCT = MIN_JOURNEY_VALUE;               /* 霍尔输出脉冲计数 */
u32 OprationCue_Amount_Buf;                                  /* 霍尔输出脉冲计数 */
u32 Hall_Pulse_Amount;
u32 Hall_Pulse_Jiggle_Buff;

u8 Delete_single_Code_ID[6];

u8 Target_Limit_Point;
u8 RF_Dblclick_Time;
u8 Run_To_Mid_Limit_Point;
u8 Electric_Quantity;

u16 Current_Current_mA;                                      /* 当前的电流值 ----*/

u16 Motor_Duty;
u8 Motor_Speed;                                              /* 电机转速 --------*/
u8 Target_Speed;
u8 motor_run_time;
u8 motor_off_time;

u8 Set_Limit_Point_Cnt;
u8 Opretion_Event;
volatile u8 Motor_Max_Speed;                             /* 电机最大转速 ---*/

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
