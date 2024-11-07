/**********************************************************************************
 *  ��Ŀ���:    AC213-04
 *  ��Ŀ����:    25�͹���
 *  �ͻ�����:                                    
 *  �����д:    ��С��
 *  ��������:    2012-09-15
 *  �������:    
 *  �汾����:    V2.4
 
 *  Time.C file
 *  ��������: 
**********************************************************************************/
#include "Define.h"                                          /* ȫ�ֺ궨�� ------*/
#include "Extern.h"                                          /* ȫ�ֱ��������� --*/
/* �ڲ��� -----------------------------------------------------------------------*/
/* Ϊ�˴�������һ��ѭ����ͬʱ����50��100MS�����飬���Խ�ʱ����΢�� --*/

/* �ڲ����� ---------------------------------------------------------------------*/

/**********************************************************************************
  ������:  Timer0_A1
  ����:    Timer0_A1�ж�
**********************************************************************************/
// Timer_A3 Interrupt Vector (TA0IV) handler
#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer0_A1 (void)
{
  if (TA0IV == 2)
  {
    if (Work_Status == NORMAL_MODE)            // ����ģʽ
    {
      TA0CCR1 += SYSTEM_TIME_VALUE * SYSTEM_CLOCK;     // 2mS
    }
    else if (Flag.Power_Down)
    {
      TA0CCR1 += (SYSTEM_TIME_VALUE * SYSTEM_CLOCK) >> 3;     // 2mS
    }
    else
    {
      TA0CCR1 += (SYSTEM_TIME_VALUE * SYSTEM_CLOCK) >> 2;     // 2mS
    }
    Flag.Time_2ms = TRUE;
  }
}
/**********************************************************************************
  ������:   TimeControl
  ��  ��:   
  ��  ��:   ��
  ��  ��:   ��
  ��  ��:   ��
  ��  ����  
 *********************************************************************************/
void TimeControl (void)
{
  if (--Time_10ms_Cnt == NULL)                               /* �Ƿ�10MS       */
  {
    Time_10ms_Cnt = TIME_10MS_VALUE;
    Flag.Time_10ms = TRUE;           /* ��10MS��־       */

  }
  if (--Time_50ms_Cnt == NULL)                               /* �Ƿ�50MS       */
  {
    Time_50ms_Cnt = TIME_50MS_VALUE;
    Flag.Time_50ms = TRUE;                                   /* ��50MS��־       */
  }
  if (--Time_100ms_Cnt == NULL)                              /* �Ƿ�100MS      */
  {
    Time_100ms_Cnt = TIME_100MS_VALUE;
    Flag.Time_100ms = TRUE;                                  /* ��100MS��־      */
  }
  if (--Time_500ms_Cnt == NULL)                              /* �Ƿ�500MS      */
  {
    Time_500ms_Cnt = TIME_500MS_VALUE;
    Flag.Time_500ms = TRUE;
    if (--Time_1s_Cnt == NULL)                               /* �Ƿ�1S         */
    {
      Time_1s_Cnt = TIME_1S_VALUE;
      Flag.Time_1s = TRUE;                                   /* ��1S��־         */
    }
  }
}
/**********************************************************************************
  ������:   LEDDisplay
  ��  ��:   
  ��  ��:   ��
  ��  ��:   ��
  ��  ��:   ��
  ��  ����  
 *********************************************************************************/
void OprationCue (void)
{
  if (Flag.Learn_Key_Hint)
  {
    Flag.Buzzer_Hint = TRUE;
    Flag.Learn_Key_Hint = NULL;
  }
  else if (Flag.Low_VOL_Jiggle)
  {
    if (MotorFlag.Motor_Run)
    {
      switch (Low_Vol_Hint_Event++)
      {
        case 0:
          Flag.Buzzer_Hint = TRUE;
        break;
        case 1:
          Flag.Buzzer_Hint = NULL;
        break;
        case 10:
          Low_Vol_Hint_Event = NULL;
        break;
      }
    }
    else 
    {
      Flag.Buzzer_Hint = NULL;
    }
  }
  else
  {
    Flag.Buzzer_Hint = NULL;
  }
}
/**********************************************************************************
  ������:   LearnDelCodeTime
  ��  ��:   
  ��  ��:   ��
  ��  ��:   ��
  ��  ��:   ��
  ��  ����  
 *********************************************************************************/
void LearnDelCodeTime (void)
{
  #define LEARN_CODE_TIME             10
  #define DELETE_CODE_TIME            10
  #define SET_LIMIT_POINT_TIME        30
  #define DELETE_SINGLE_CODE_TIME     10
  #define JOINT_ADJ_TIME              30
  
  static u8 Learn_Time_Cnt;
  
  if (Flag.Learn_Code_Statu)
  {
    if (++Learn_Time_Cnt >= LEARN_CODE_TIME)
    {
      Flag.Learn_Code_Statu = NULL;
      Learn_Time_Cnt = NULL;
    }
  }
  else
  {
    Learn_Time_Cnt = NULL;
  }
  
  if (Flag.Delete_single_Code)
  {
    if (++Delete_single_Code_Cnt >= DELETE_SINGLE_CODE_TIME)
    {
      Flag.Delete_single_Code = NULL;
      Delete_single_Code_Cnt = NULL;
    }
  }
  else
  {
    Delete_single_Code_Cnt = NULL;
  }
  
  if ((Flag.Set_Limit_Point) && (Motor_Status == MOTOR_STOP))
  {
    if (++Set_Limit_Point_Cnt >= SET_LIMIT_POINT_TIME)
    {
      u8 i;
      
      Set_Limit_Point_Cnt = NULL;
      Flag.Set_Limit_Point = NULL;
      Flag.Opretion_Finish = TRUE;
      Opretion_Event = NULL;
      
      for (i = 0; i < (MID_LIMIT_POINT_AMOUNT + 2); i++)
      {
        Limit_Point[i].Flag.F1.Fine_Adjust = NULL;
      }
    }
  }
  else
  {
    Set_Limit_Point_Cnt = NULL;
  }
  
  
  if ((Flag.Joint_Level_Adj)  && (Motor_Status == MOTOR_STOP))
  {
    if (++Joint_Level_ADJ_Time >= JOINT_ADJ_TIME)
    {
      Joint_Level_ADJ_Time = 0;
      Flag.Joint_Level_Adj = NULL;
    }
  }
  else
  {
    Joint_Level_ADJ_Time = 0;
  }
}
/**********************************************************************************
  ������:   DblclickTimeLimit
  ��  ��:   
  ��  ��:   ��
  ��  ��:   ��
  ��  ��:   ��
  ��  ����  
 *********************************************************************************/
void DblclickTimeLimit (void)
{
  #define RF_DBLCLICK_TIME      1500 / 100
  
  if ((Flag.RF_UP_dblclick) || (Flag.RF_DOWN_dblclick))
  {
    if (++RF_Dblclick_Time >= RF_DBLCLICK_TIME)
    {
      RF_Dblclick_Time = NULL;
      Flag.RF_UP_dblclick = NULL;
      Flag.RF_DOWN_dblclick = NULL;
    }
  }
  else
  {
    RF_Dblclick_Time = NULL;
  }
}
