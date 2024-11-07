/**********************************************************************************
 *  ��Ŀ���:    AC213-04
 *  ��Ŀ����:    25�͹���
 *  �ͻ�����:                                    
 *  �����д:    ��С��
 *  ��������:    2012-09-15
 *  �������:    
 *  �汾����:    V2.4
 *  
 *  MotorControl.c file
 *  ��������: 
**********************************************************************************/
#include "Define.h"                                          /* ȫ�ֺ궨�� ------*/
#include "Extern.h"                                          /* ȫ�ֱ��������� --*/
/* �ڲ��� -----------------------------------------------------------------------*/
#define MEET_PLUG_TIME         2                             /* ���������ʱ�� --*/
#define NO_SPEED_TIME          (1000 / (TIME_10MS_VALUE * SYSTEM_TIME))    /* ��ת�ٱ���ʱ�� --*/
#define SLOW_SPEED_TIME        (600  / (TIME_10MS_VALUE * SYSTEM_TIME))

#define INCHING_ANGLE          30                  // ΢���ĽǶ�
#define INCHING_ANGLE_TURNS    (((INCHING_ANGLE * 10) + 7) / 14)
//#define JIGGLE_ANGLE           6                            // �����㶯�ĽǶ�
//#define JIGGLE_ANGLE_TURNS     (((JIGGLE_ANGLE * 10) + 7) / 14)
#define JIGGLE_ANGLE_TURNS(var)   (((((var) * 3) * 10) + 7) / 14)

#define REDUCTION_RATIO        1384
#define SPEED                  ((300000000 / REDUCTION_RATIO) / RF_DECODE_TIME)  // (60000000 / 2) / 138.4
#define SPEED_COUNT(var)       ((SPEED + (var >> 1)) / (var))  

#define KP                     20                            // ����ϵ�� 

u8 PID_KP = KP;
signed char Un;
signed char Un1;

static volatile u16 Meet_Plugb_Cnt;
static volatile u16 Meet_Plugb_Cnt1;
static volatile u16 Meet_Plugb_Cnt2;

static volatile u32 Hall_Pulse_Amount_Buff;

static volatile u16 Meet_Plugb_Cnt;
static volatile u16 Motor_RunTime_Cnt = MOTOR_RUN_TIME;
static volatile u16 Current_Current_mA_Buff;

u8 Motor_Speed_Buff;
u8 speed_control_cnt;                                        /* �����ٶȵ�ʱ�� --*/
u8 curr_over_on_cnt;                                         /* ����ʱ������� --*/
u8 curr_over_off_cnt;                                        /* ����ʱ������� --*/

static u16 ADC_CUR_Array[4];
static u8 ADC_CUR_Array_Index;
//static u32 Target_Journey_Point;
/* �ڲ����� ---------------------------------------------------------------------*/
void MotorStatusSwitch (void);
u8 MeetPlugbDetect (void);
void SlowStop (u32 target);

/**********************************************************************************
  ������:   CountMotorSpeed
  ��  ��:   
  ��  ��:   ��
  ��  ��:   ��
  ��  ��:   ��
  ��  ����  
 *********************************************************************************/
void CountMotorSpeed (void)
{
  if (Hall_Pulse_Width < 35)
  {
    Motor_Speed = 200;
  }
  else if (Hall_Pulse_Width > 15000)
  {
    Motor_Speed = 0;
  }
  else if (Hall_Pulse_Width > 337)
  {
    Motor_Speed = ((u8) SPEED_COUNT(Hall_Pulse_Width));    /* ����ÿ���ӵ�ת�� */
  }
  else
  {
    ADC_CUR_Array[ADC_CUR_Array_Index] = Hall_Pulse_Width;
    ADC_CUR_Array_Index++;
    if (ADC_CUR_Array_Index >> 2)                        /* �Ƿ���4�� -----*/
    {
      u16 temp;
      
      temp = SampleAverage(ADC_CUR_Array,ADC_CUR_Array_Index);
      ADC_CUR_Array_Index = NULL;
    
      Motor_Speed = ((u8) SPEED_COUNT(temp));            /* ����ÿ���ӵ�ת�� */
    }
  }
}
/**********************************************************************************
  ������:   MeetPlugb
  ��  ��:   
  ��  ��:   ��
  ��  ��:   ��
  ��  ��:   ��
  ��  ����  
 *********************************************************************************/
u8 MeetPlugbDetect (void)
{
  u8 flag = NULL;

  if (Motor_Speed < MIN_SPEED)
  {
    if (Current_Current_mA >= STARTUP_CURRENT)
    {
      if (++Meet_Plugb_Cnt >= NO_SPEED_TIME)
      {
        Meet_Plugb_Cnt = 0;
        Meet_Plugb_Cnt1 = 0;
        flag = TRUE;
      }
    }
    else
    {
      Meet_Plugb_Cnt = 0;
      Meet_Plugb_Cnt1 = 0;
    }
    
    if (++Meet_Plugb_Cnt2 >= SLOW_SPEED_TIME)
    {
      Meet_Plugb_Cnt1 = NULL;
      Meet_Plugb_Cnt2 = NULL;
        
      if (Current_Current_mA >= STARTUP_CURRENT)
      {
        if ((Hall_Pulse_Amount_Cnt < (Hall_Pulse_Amount_Buff + 3))
        && (Hall_Pulse_Amount_Cnt > (Hall_Pulse_Amount_Buff - 3)))
        {
          flag = TRUE;
        }
      }
      Hall_Pulse_Amount_Buff = Hall_Pulse_Amount_Cnt;
    }
  }
  else if (++Meet_Plugb_Cnt1 > 10)
  {
    Meet_Plugb_Cnt1 = NULL;
    Meet_Plugb_Cnt2 = NULL;
  }
  
  return (flag);
}
/**********************************************************************************
  ������:   JourneyComper
  ��  ��:   
 *********************************************************************************/
void SlowStop (u32 target)
{
  static u8 slow_speed_cnt;
  u8 temp;
  
  if ((Hall_Pulse_Amount_Cnt < (target + 120)) && (Hall_Pulse_Amount_Cnt > (target - 120)))
  {
    Flag.slowstop = TRUE;
    if (MotorFlag.Motor_Direction ^ MotorFlag.Journey_Direction)
    {
      temp = (target - Hall_Pulse_Amount_Cnt) / 6;
    }
    else
    {
      temp = (Hall_Pulse_Amount_Cnt - target) / 6;
    }
    
    if (temp > MAX_SPEED)
    {
      temp = MAX_SPEED;
    }
    else if (temp < MIN_SPEED)
    {
      temp = MIN_SPEED;
    }
    
    if (Target_Speed > temp)
    {
      if (++slow_speed_cnt >= 25)
      {
        slow_speed_cnt = NULL;
        Target_Speed--;
      }
    }
    else if (Target_Speed < temp)
    {
      if (++slow_speed_cnt >= 25)
      {
        slow_speed_cnt = NULL;
        Target_Speed++;
      }
    }
    if (motor_run_time < 3)
    {
      Target_Speed = MIN_SPEED;
    }
  }
  else
  {
    Flag.slowstop = NULL;
  }
}
/**********************************************************************************
  ������:   JourneyComper
  ��  ��:   
  ��  ��:   ��
  ��  ��:   ��
  ��  ��:   ��
  ��  ����  
 *********************************************************************************/
void JourneyComper (void)
{
  u8 flag_stop = NULL;
  
  /* �Ƿ��趨����λ�� */
  if ((MotorFlag.Motor_Run) && (Limit_Point[0].Flag.F1.Limit_Activate))
  {
    if ((!Flag.NO_Comper_Limit_Point) && (!Limit_Point[0].Flag.F1.Fine_Adjust))
    {
      // �Ƿ��ڵ�һ����λ������
      if (Hall_Pulse_Amount_Cnt < Limit_Point[0].Limit_Place)
      {
        // �Ƿ�Ҫ��������
        if (!(MotorFlag.Motor_Direction ^ MotorFlag.Journey_Direction))
        {
          flag_stop = TRUE;
        }
      }
      else if (Run_To_Mid_Limit_Point != NULL)               // �Ƿ�Ҫ���е��м���λ��
      {
        if ((Hall_Pulse_Amount_Cnt > (Limit_Point[Run_To_Mid_Limit_Point].Limit_Place -2))
        && (Hall_Pulse_Amount_Cnt < (Limit_Point[Run_To_Mid_Limit_Point].Limit_Place + 2)))  
        {
          flag_stop = TRUE;
          Run_To_Mid_Limit_Point = NULL;
        }
      }
      // �Ƿ񳬳���������λ��
      else if (Hall_Pulse_Amount_Cnt > Limit_Point[Limit_Point_Amount-1].Limit_Place)
      {
        // �Ƿ�Ҫ����λ������
        if (MotorFlag.Motor_Direction ^ MotorFlag.Journey_Direction)
        {
          flag_stop = TRUE;
        }
        else
        {
          // ������λ�� ������λ
          SlowStop (Limit_Point[Target_Limit_Point].Limit_Place);
        }
      }
      else
      {
         // ������λ
        SlowStop (Limit_Point[Target_Limit_Point].Limit_Place);
        if ((Hall_Pulse_Amount_Cnt > (Limit_Point[Target_Limit_Point].Limit_Place -2))
        && (Hall_Pulse_Amount_Cnt < (Limit_Point[Target_Limit_Point].Limit_Place + 2)))  
        {
          if ((!Flag.Del_Code_Finish) && (!Flag.Opretion_Finish))
          {
            flag_stop = TRUE;
          }
        }
      }
    }
    
    if (!(MotorFlag.Motor_Direction ^ MotorFlag.Journey_Direction))
    {
      if (Hall_Pulse_Amount_Cnt < (HALL_PULSE_MIN + 100))
      {
        flag_stop = TRUE;
      }
    }
    else
    {
      if (Hall_Pulse_Amount_Cnt > (HALL_PULSE_MAX - 100))
      {
        flag_stop = TRUE;
      }
    }
  }
  
  if ((Flag.Del_Code_Finish) || (Flag.Opretion_Finish))
  {
    if (Flag.OprationCue_Run)
    {
      if ((Hall_Pulse_Amount_Cnt > (OprationCue_Amount_Buf + INCHING_ANGLE_TURNS))
      || (Hall_Pulse_Amount_Cnt < (OprationCue_Amount_Buf - INCHING_ANGLE_TURNS)))
      {
        flag_stop = TRUE;
        Flag.OprationCue_Run = NULL;
      }
    }
    if (Flag.OprationCue_Stop)
    {
      if ((Hall_Pulse_Amount_Cnt > (OprationCue_Amount_Buf - 2))
      && (Hall_Pulse_Amount_Cnt < (OprationCue_Amount_Buf + 2)))
      {
        flag_stop = TRUE;
        Flag.OprationCue_Stop = NULL;
      }
    }
  }
  else
  {
    if (Flag.Jiggle_Stop)
    {
      if (!Flag.Low_VOL_Jiggle)
      {
        Target_Speed = Motor_Max_Speed;
        Motor_Duty = MAX_DUTY;//800;
      }
      
      if ((Hall_Pulse_Amount_Cnt > (Hall_Pulse_Jiggle_Buff + JIGGLE_ANGLE_TURNS(Joint_Time_Level)))   // 5  9��
      || (Hall_Pulse_Amount_Cnt < (Hall_Pulse_Jiggle_Buff - JIGGLE_ANGLE_TURNS(Joint_Time_Level))))
      {
       flag_stop = TRUE;
        Flag.Jiggle_Stop = NULL;
      }
    }
  }
  
  if (flag_stop)
  {
    MotorFlag.Motor_Run = NULL;
    Motor_Status = MOTOR_STOP;
    MotorStop();
  }
}
/**********************************************************************************
  ������:   MotorUp
  ��  ��:   
 *********************************************************************************/
void MotorUp (u16 duty)
{
  //SET_UP_DIRECTION();                          //
  P2SEL |= BIT2 + BIT5;                         // P2.2 and P2.5 TA1/2 options
  P3SEL &= ~(BIT2 + BIT3);                      // P3.2 and P3.3 TA1/2 options
  IN2_H_0();
  IN2_L_0();
  TA1CCR1 = duty - 10;
  TA1CCR2 = duty;
}
/**********************************************************************************
  ������:   MotorDown
  ��  ��:   
 *********************************************************************************/
void MotorDown (u16 duty)
{
  //SET_DOWN_DIRECTION();                       //
  P2SEL &= ~(BIT2 + BIT5);                     // P2.2 and P2.5 TA1/2 options
  P3SEL |= BIT2 + BIT3;                        // P3.2 and P3.3 TA1/2 options
  IN1_H_0();
  IN1_L_0();
  TA1CCR1 = duty - 10;
  TA1CCR2 = duty;
}
/**********************************************************************************
  ������:   MotorStop
  ��  ��:   
 *********************************************************************************/
void MotorStop (void)
{
  P2SEL &= ~(BIT2 + BIT5);                      // P2.2 and P2.5 TA1/2 options
  P3SEL &= ~(BIT2 + BIT3);                      // P3.2 and P3.3 TA1/2 options
  TA1CCR1 = 0;
  TA1CCR2 = 0;
  IN1_H_0();
  IN1_L_0();
  IN2_H_0();
  IN2_L_0();
  Motor_Duty = 0;
}
/**********************************************************************************
  ������:   Duty_Add
  ��  ��:   
 *********************************************************************************/
void Duty_Add (u16 *p,u8 i)
{
  *p += i;
  if (*p > MAX_DUTY)
  {
    *p = MAX_DUTY;
  }
}
/**********************************************************************************
  ������:   Duty_Sub
  ��  ��:   
 *********************************************************************************/
void Duty_Sub (u16 *p,u8 i)
{
  if (*p > i)
  {
    *p -= i;
  }
}
/**********************************************************************************
  ������:   MotorControl
  ��  ��:   
  ��  ��:   ��
  ��  ��:   ��
  ��  ��:   ��
  ��  ����  
 *********************************************************************************/
void MotorControl (void)
{
  #define OPRATION_OVERTIME                           3000 / 16
  static u8 Time_Limit_Cnt;
    /*    if ((Motor_Array[0].Motor_Flag.Journey_Set_OK) || (Motor_Array[0].Motor_Flag.Single_Journey))
        {
          // ȷ��Ŀ��λ��
          if (Flag.Goto_PCT)
          {
            Target_Journey_Point = Hall_Pulse_Amount_PCT;
          }
          else
          {
            if (MotorFlag.Motor_Fact_Direction ^ MotorFlag.Journey_Direction)
            {
              Target_Journey_Point = Hall_Pulse_Amount;
            }
            else
            {
              Target_Journey_Point = MIN_JOURNEY_VALUE;
            }
          }*/
        
  /*    if (Flag.Goto_PCT)
      {
        // �Ƿ�ӽ��г�
        if ((Hall_Pulse_Amount_Cnt > (Target_Journey_Point - 2))
        && (Hall_Pulse_Amount_Cnt < (Target_Journey_Point + 2)))
        {
          flag_stop = TRUE;
        }
      }*/
      if (Flag.Goto_PCT)
      {
        // �Ƿ�ӽ��г�
        if ((Hall_Pulse_Amount_Cnt > (Hall_Pulse_Amount_PCT - 3))
        && (Hall_Pulse_Amount_Cnt < (Hall_Pulse_Amount_PCT + 3)))
        {
          Motor_Status = MOTOR_STOP;
          Flag.Goto_PCT = NULL;
        }
      }
  if ((!Flag.Del_Code_Finish) && (!Flag.Opretion_Finish))
  {
    if (MotorFlag.Motor_Run)
    {
      motor_off_time = NULL;
      
      if  (motor_run_time < 2)
      {
        RF_WORK();                          // RFģ�鿪ʼ����
        BRAKE_OFF();                         // ��ͨ�̵���
        HALL_POWEER_ON();
        MotorStop();
        
        Motor_Speed_Buff = 0;
        Target_Speed = MIN_SPEED;
        Motor_Duty = MAX_DUTY >> 2;
      }
      else if  (motor_run_time < 4)
      {
        MotorFlag.Hall_Power = TRUE;

      }
      else
      {
        signed int En,En1;
        u8 flag;
        
        En = Target_Speed - Motor_Speed;         // �õ���ǰ�ٶ���� E(n)
        En1 = Target_Speed - Motor_Speed_Buff;   // �õ��ϴ��ٶ���� E(n-1)
        En += En1;    
        // �����ת��������
        if (En >= 0)               // ��ǰ�����Ƿ�Ϊ����
        {
          flag = 1;                // 
        }
        else                       // ����Ϊ����
        {
          En = 0 - En;             // ת��������
          flag = 0;
        }
        
        if (flag == 0)                     // ����Ϊ����
        {
          if (En > 20)
          {
            Duty_Sub(&Motor_Duty,20);
          }
          else if (En > 16)
          {
            Duty_Sub(&Motor_Duty,10);
          }
          else if (En > 12)
          {
            Duty_Sub(&Motor_Duty,5);
          }
          else if (En > 6)
          {
            Duty_Sub(&Motor_Duty,2);
          }
          else if (En > 2)
          {
            Duty_Sub(&Motor_Duty,1);
          }
        }
        else                              // ����Ϊ����
        {
          if (En > 20)
          {
            Duty_Add(&Motor_Duty,30);
          }
          else if (En > 16)
          {
            Duty_Add(&Motor_Duty,15);
          }
          else if (En > 12)
          {
            Duty_Add(&Motor_Duty,6);
          }
          else if (En > 6)
          {
            Duty_Add(&Motor_Duty,2);
          }
          else if (En > 2)
          {
            Duty_Add(&Motor_Duty,1);
          }
        }

        if (motor_run_time >= 20)
        {
          if (MeetPlugbDetect())
          {
            MotorFlag.Motor_Run = NULL;
            Motor_Status = MOTOR_STOP;
          }
        }
        else
        {
          Hall_Pulse_Amount_Buff = Hall_Pulse_Amount_Cnt;
          Meet_Plugb_Cnt = NULL;
          Meet_Plugb_Cnt1 = NULL;
          Meet_Plugb_Cnt2 = NULL;
          Flag.slowstop = NULL;
        }
        
        if (!Flag.slowstop)
        {
          if (Target_Speed < MAX_SPEED)
          {
            if (++speed_control_cnt > 2)
            {
              Target_Speed ++;
              speed_control_cnt = NULL;
            }
            if (motor_run_time < 3)
            {
              Target_Speed = MIN_SPEED;
            }
          }
        }
        
        if (Motor_Duty > MAX_DUTY)
        {
          Motor_Duty = MAX_DUTY;
        }
        if (Motor_Duty < MIN_DUTY)
        {
          Motor_Duty = MIN_DUTY;
        }
        
        if (MotorFlag.Motor_Run)
        {
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
      
      if (motor_run_time < 250)
      {
        motor_run_time++;
      }
      Motor_Speed_Buff = Motor_Speed;
    }
    else
    {
      motor_run_time = NULL;
      MotorStop();
      BRAKE_ON();                         // ɲ�����Ͽ��̵���
      
      if (motor_off_time < 250)
      {
        motor_off_time++;
      }
    }
  }
  else  // ��Ҫ΢����ʾ
  {
      static u8 Jiggle_Direction;
      
      switch (Opretion_Event)
      {
        case 0:
          HALL_POWEER_ON();
          MotorFlag.Motor_Run = NULL;
          Motor_Status = MOTOR_STOP;
          MotorStop();
          Opretion_Event++;
          Flag.StatusSwitch_Delay = TRUE; 
          Motor_RunTime_Cnt = MOTOR_SWITCH_LATIME >> 1;
          
          Target_Speed = MAX_SPEED;
          Flag.Jiggle_Stop = NULL;
          BRAKE_OFF();                         // ��ͨ�̵���
          
          Jiggle_Direction = 0;
          if ((Hall_Pulse_Amount_Cnt > (Limit_Point[0].Limit_Place - 200))
          && (Hall_Pulse_Amount_Cnt < (Limit_Point[0].Limit_Place + 200)))
          {
            if (MotorFlag.Journey_Direction)
            {
              Jiggle_Direction = 1;
            }
          }
          else
          {
            if (!MotorFlag.Journey_Direction)
            {
              Jiggle_Direction = 1;
            }
          }
        break;
        
        case 1: // ��
        case 5: // ��
          MotorFlag.Hall_Power = TRUE;
          if (!Flag.StatusSwitch_Delay)
          {
            OprationCue_Amount_Buf = Hall_Pulse_Amount_Cnt;
            Flag.OprationCue_Run = TRUE;
            Flag.StatusSwitch_Delay = TRUE;                      /* �ȴ�500MS -------*/
            Opretion_Event++;
            Time_Limit_Cnt = NULL;
          }
        break;
      
        case 2:
        case 6:
          if (Flag.OprationCue_Run)
          {
            if (++Time_Limit_Cnt > OPRATION_OVERTIME)
            {
              Time_Limit_Cnt = NULL;
              Flag.OprationCue_Run = NULL;
              MotorStop();                  // STOP
              Flag.StatusSwitch_Delay = TRUE;                      /* �ȴ�200MS -------*/
              Motor_RunTime_Cnt = MOTOR_SWITCH_LATIME >> 1;
            }
            else
            {
              // �Ƿ�����λ
              if (Limit_Point[0].Flag.F1.Limit_Activate)
              {
                if (Jiggle_Direction)
                {
                  MotorDown(MAX_DUTY); // down
                }
                else
                {
                  MotorUp(MAX_DUTY);  // UP
                }
              }
              else
              {
                MotorUp(MAX_DUTY);  // UP
              }
            } 
          }
          else if (!Flag.StatusSwitch_Delay)
          {
            Opretion_Event++;
            Flag.StatusSwitch_Delay = TRUE;                      /* �ȴ�200MS -------*/
            Motor_RunTime_Cnt = MOTOR_SWITCH_LATIME >> 1;
         }
        break;
      
        case 3: // ��
        case 7: // ��
          if (!Flag.StatusSwitch_Delay)
          {
            Opretion_Event++;
            Flag.OprationCue_Stop = TRUE;
	    Time_Limit_Cnt = NULL;
          }
        break;
      
        case 4: 
        case 8:  // OFF
          Time_Limit_Cnt++;	  
          if ((!Flag.OprationCue_Stop) || (Time_Limit_Cnt > OPRATION_OVERTIME))
          {
            Time_Limit_Cnt = NULL;
            Flag.OprationCue_Run = NULL;
            if (Flag.Del_Code_Finish)
            {
              Opretion_Event++;
              Flag.StatusSwitch_Delay = TRUE;                  /* �ȴ�500MS -------*/
            }
            else
            {
              Opretion_Event = 9;
            }
          }
          else
          {
            // �Ƿ�����λ
            if (Limit_Point[0].Flag.F1.Limit_Activate)
            {
              if (Jiggle_Direction)
              {
                MotorUp(MAX_DUTY);  // UP
              }
              else
              {
                MotorDown(MAX_DUTY); // down
              }
            }
            else
            {
              MotorDown(MAX_DUTY); // down
            }
          }
        break;
      
        default:
          Opretion_Event = NULL;
          Flag.Del_Code_Finish = NULL;
          Flag.Opretion_Finish = NULL;
          Motor_Status = MOTOR_STOP;
          MotorStop();
        break;
      }
  }
}
/**********************************************************************************
  ������:   MotorStatusSwitch
  ��  ��:   ����״̬�л�
  ��  ��:   ��
  ��  ��:   ��
  ��  ��:   ��
  ��  ����  
 *********************************************************************************/
void MotorStatusSwitch (void)
{
  if (!Flag.StatusSwitch_Delay)                              /* �Ƿ����ӳ�500mS -*/
  {
    if (Motor_Status == Motor_Status_Buffer)                 /* ״̬�Ƿ�û�иı� */
    {
      if (++Motor_RunTime_Cnt >= MOTOR_RUN_CNT_VALUE)        /*�����ʱ���Ƿ�*/
      {
        Motor_Status = MOTOR_STOP;
        Motor_RunTime_Cnt = NULL;
      }
    }
    else
    {
      Set_Limit_Point_Cnt = NULL;
      Joint_Level_ADJ_Time = 0;
      Flag.Motor_Status_Switch = TRUE;  
      MotorStop();
      
      motor_run_time = NULL;
      motor_off_time = NULL;
      Motor_RunTime_Cnt = NULL;
      Motor_Speed = NULL;

      //Hall_Pulse_Jiggle_Buff = Hall_Pulse_Amount_Cnt;                  // ���浱ǰλ��
      
      if (Flag.Low_VOL_Jiggle)
      {
        if ((Motor_Status == MOTOR_UP) || (Motor_Status == MOTOR_DOWN))  /* �Ƿ����� ----*/
        {
          Flag.Learn_Key_Hint = TRUE; 
          Low_Vol_Hint_Event = TRUE; 
        }
      }
      
      if (3 == (Motor_Status_Buffer | Motor_Status))         /* �Ƿ������л� ----*/
      {
        Flag.StatusSwitch_Delay = TRUE;                      /* �ȴ�500MS -------*/
        Motor_Status_Buffer = MOTOR_STOP;                    /* ���ֹͣ --------*/
      }
      else
      {
        Motor_RunTime_Cnt = MOTOR_RUN_TIME;
        Motor_Status_Buffer = Motor_Status;                  /* ���µ�λ --------*/
      }
    }
    
    switch (Motor_Status_Buffer)                             /*��ǰ���״̬��ʲô*/
    {
      case MOTOR_UP:                                         /* �� --------------*/
        MotorFlag.Motor_Run = TRUE;
        SET_UP_DIRECTION();
      break;
      case MOTOR_STOP:                                       /* ͣ --------------*/
        MotorFlag.Motor_Run = NULL;
      break;
      case MOTOR_DOWN:                                       /* �� --------------*/
        MotorFlag.Motor_Run = TRUE;
        SET_DOWN_DIRECTION();
      break;
    }
    
    if (Flag.Joint_Action)  // �Ƿ���Ҫ�㶯������
    {
      if (++Joint_Action_Time_Cnt > JOINT_ACTION_TIME)
      {
        Joint_Action_Time_Cnt = 0;
        Motor_Status = Joint_Action_Motor_Buf;
        Flag.Joint_Action = NULL;
      }
    }
    else
    {
      Joint_Action_Time_Cnt = 0;
    }
    
    if (Flag.Run_To_finishing_point)  // �Ƿ�Ҫ���е��յ�
    {
      Flag.Run_To_finishing_point = NULL;
      if (!(MotorFlag.Motor_Direction ^ MotorFlag.Journey_Direction))
      {
        Target_Limit_Point = NULL; // ֻ����һ����λ��
      }
      else
      {
        Target_Limit_Point = Limit_Point_Amount - 1; // ���е����һ����λ��
      }
    }
    // �Ƿ����л�״̬����������λ��
    else if ((Flag.Motor_Status_Switch) && (Limit_Point[0].Flag.F1.Limit_Activate))
    {
      u8 x;
      
      Flag.Motor_Status_Switch = NULL;
      Flag.NO_Comper_Limit_Point = NULL;
      
      // �Ƿ�͵�һ���г̷���һ��
      if (!(MotorFlag.Motor_Direction ^ MotorFlag.Journey_Direction))
      {
        Flag.NO_Comper_Limit_Point = NULL;
        if (Flag.Set_Limit_Point)  // �Ƿ���������λ״̬
        {
          Target_Limit_Point = NULL; // ֻ����һ����λ��
        }
        else
        {
          x = LoopCompare(Hall_Pulse_Amount_Cnt,LIMIT_PLACE);  /*��ǰ�Ƿ���ĳ����λ��*/
          if (x != NULL)                              
          {
            if (x > 1) // �Ƿ�ǰ�ڵ�һ����λ����
            {
              Target_Limit_Point = x - 2;  // ���е���ǰ��λ�����һ��
            }
            else
            {
              Target_Limit_Point = NULL; // ��ǰ�ڵ�һ����λ�㣬�����һ����λ
            }
          }
          else   // ��ǰ������λ��λ��
          {
            // �����һ����λ�㿪ʼ���
            for (x = MID_LIMIT_POINT_AMOUNT; x > 0; x--)
            {
              if (Limit_Point[x].Flag.F1.Limit_Activate)
              {
                // ��ǰλ���Ƿ��ڸ���λ�����
                if (Hall_Pulse_Amount_Cnt > Limit_Point[x].Limit_Place)
                {
                  break;  //�˳����е�����λ��
                }
              }
            }
            Target_Limit_Point = x;
          }
        }
      }
      else  // ����һ����λ���෴�ķ�����
      {
        if (Flag.Set_Limit_Point) // �Ƿ���������λ��
        {
          Flag.NO_Comper_Limit_Point = TRUE;  // �������λ
        }
        else
        {
          Flag.NO_Comper_Limit_Point = NULL;
          x = LoopCompare(Hall_Pulse_Amount_Cnt,LIMIT_PLACE);  /*��ǰ�Ƿ���ĳ����λ��*/
          if (x != NULL)                              
          {
            if (x >= Limit_Point_Amount) // ��ǰ�Ƿ����һ����λ����
            {
              if (Limit_Point_Amount == 1) // �Ƿ�ֻ��һ����λ��
              {
                Flag.NO_Comper_Limit_Point = TRUE;  // �������λ
              }
              else //��ֻһ����λ�㣬��ǰֹͣ������λ����
              {
                Target_Limit_Point = x - 1;  // ���ּ�����һ����λ��
              }
            }
            else // �������һ����λ����
            {
              Target_Limit_Point = x; // ���е���һ��
            }
          }
          else  // û��ͣ����λ����
          {
            // �ӵ�2����λ�㿪ʼ�Ƚ�
            for (x = 1; x < (MID_LIMIT_POINT_AMOUNT + 2); x++)
            {
              if (Limit_Point[x].Flag.F1.Limit_Activate)
              {
                // �Ƿ�ǰλ���ڸ���λ��ǰ��
                if (Hall_Pulse_Amount_Cnt < Limit_Point[x].Limit_Place)
                {
                  break; // �˳����е�����λ��
                }
              }
              else // ����û����λ����
              {
                if (x > 1) // ���ڵ�һ����λ�㷶Χ
                {
                  x--;  // ���ּ�鵱ǰ��λ��
                }
                else  // ֻ��һ����λ��
                {
                  Flag.NO_Comper_Limit_Point = TRUE;  // �������λ
                }
                break;
              }
            }
            Target_Limit_Point = x;
          }
        }
      }
    }
  }
  else if (++Motor_RunTime_Cnt > MOTOR_SWITCH_LATIME)
  {
    Motor_RunTime_Cnt = NULL;
    Flag.StatusSwitch_Delay = NULL;
  }
}
