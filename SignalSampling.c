/**********************************************************************************
 *  ��Ŀ���:    AC213-04
 *  ��Ŀ����:    25�͹���
 *  �ͻ�����:                                    
 *  �����д:    ��С��
 *  ��������:    2012-09-15
 *  �������:    
 *  �汾����:    V2.4
 *
 *  Signal_Sampling.c file
 *  ��������: �źŲ���
**********************************************************************************/
#include "Define.h"                                          /* ȫ�ֺ궨�� ------*/
#include "Extern.h"                                          /* ȫ�ֱ��������� --*/
/* �ڲ��� -----------------------------------------------------------------------*/
#define RF_MAX_BYTE_VALUE      8                             // �����¼����ֵ
#define DATA_BIT_AMOUNT        8                             // ����BIT����
#define AD_TIME                RF_DECODE_TIME * 3
#define RF_NO_RX_TIME          200/50

// �ź�ͷ���ֵ50us*3*106=16MS
#define HEAD_LONG_MAX          ((5500+((AD_TIME)/2))/(AD_TIME))
#define HEAD_LONG_MIN          ((4500+((AD_TIME)/2))/(AD_TIME))
#define HEAD_SHORT_MAX         ((1500+((AD_TIME)/2))/(AD_TIME))
#define HEAD_SHORT_MIN         ((450+((AD_TIME)/2))/(AD_TIME))

#define SIGNAL_LONG_MAX        ((1000+((AD_TIME)/2))/(AD_TIME))
#define SIGNAL_LONG_MIN        ((450+((AD_TIME)/2))/(AD_TIME))

#define SIGNAL_SHORT_MAX       ((400+((AD_TIME)/2))/(AD_TIME))
#define SIGNAL_SHORT_MIN       ((150+((AD_TIME)/2))/(AD_TIME))

#define SIGNAL_BIT_MAX         ((1300+((AD_TIME)/2))/(AD_TIME))
#define SIGNAL_BIT_MIN         ((650+((AD_TIME)/2))/(AD_TIME))

/* �ڲ����� ---------------------------------------------------------------------*/
u8 RF_Data_OverTime_Cnt;                                     /* ʱ����������� --*/
u8 RF_Bit_cnt;                                               /* Bit������ -------*/
u8 RF_Byte_Cnt;                                              /* Byte������ ------*/
u8 RF_KeyUnloosen_Cnt;                                       /* �ɼ������� ------*/
u8 RF_RxData[RF_MAX_BYTE_VALUE];                             /* RF�������ݻ��� --*/
u8 RF_RxData_Buf[RF_MAX_BYTE_VALUE];                         /* RF�������ݻ��� --*/

u8 * Rx_Data_P;                                              /* RF���ݽ���ָ�� --*/
u8 RF_NO_Rx_Cnt;

/* �ڲ����� ---------------------------------------------------------------------*/
u8 RFDataVerify (void);
void ErrorDisposal (void);
void RFDataDisposal (void);

/*********************************************************************************
  ������:   
  ��  ��:   
  ��  ��:   ��
  ��  ��:   ��    
  ��  ��:   ��
  ��  ����
*********************************************************************************/
void RFStatusInspect (void)
{
  if (Flag.RF_Rx_Finish)
  {
    Flag.RF_Rx_Finish = NULL;
    RF_NO_Rx_Cnt = NULL;
  }
  else
  {
    if (++RF_NO_Rx_Cnt > RF_NO_RX_TIME)
    {
      RF_NO_Rx_Cnt = NULL;
      Flag.RF_Data_Disposal = NULL;
    }
  }
}
/**********************************************************************************
  ������:  	ErrorDisposal
  ��  ��:   
**********************************************************************************/
void ErrorDisposal (void)
{
    RF_Bit_cnt = NULL;                                       /* ��BIT������ -----*/
    RF_Byte_Cnt = NULL;
    Flag_RF.Signal_Head = NULL;                              /* RF�ź�ͷ��־---- */
}
/**********************************************************************************
  ������:  	LimitLLC
  ��  ��:       
**********************************************************************************/
void LimitLLC (Limit array[],u8 place)
{
  array[place].Flag.F1.Fine_Adjust = 0;
  array[place].Flag.F1.Limit_Activate = 0;
  array[place].Limit_Place = 0; 
  
  for (; place < (MID_LIMIT_POINT_AMOUNT + 2); place++)   // ��������һ������ǰ��
  {
    if (place == (MID_LIMIT_POINT_AMOUNT + 1))
    {
      // ���һ���Ѿ��Ƶ�ǰ���ˣ�ֱ��ɾ���˳�
      Limit_Point[place].Flag.F1.Fine_Adjust = 0;
      Limit_Point[place].Flag.F1.Limit_Activate = 0;
      Limit_Point[place].Limit_Place = 0; 
      break;
    }
    else if (array[place + 1].Flag.F1.Limit_Activate)                // ������λ���Ƿ�������
    {
      // ��������λ�򽫺������ǰ��
      array[place].Flag.F1.Fine_Adjust = array[place + 1].Flag.F1.Fine_Adjust; 
      array[place].Flag.F1.Limit_Activate = array[place + 1].Flag.F1.Limit_Activate;
      array[place].Limit_Place = Limit_Point[place + 1].Limit_Place;
    }
    else
    {
      // ����û����λ������������
      // ��Ϊ�����Ѿ��Ƶ�ǰ��ȥ��
      Limit_Point[place].Flag.F1.Fine_Adjust = 0;
      Limit_Point[place].Flag.F1.Limit_Activate = 0;
      Limit_Point[place].Limit_Place = 0; 
    }
  }
}
/**********************************************************************************
  ������:   LimitCollator
  ��  ��:   ��λ������
  ��  ��:   ��
  ��  ��:   ��    
  ��  ��:   �����Ƿ���ȷ��־
  ��  ����  
**********************************************************************************/
u8 LimitCollator (Limit array[])
{
    u8 i;
  u8 j;
  u8 flag = 1;
  u32 mid;
  
  // �����������λ��
  j = 0;
  for (i = 0; i < (MID_LIMIT_POINT_AMOUNT + 2); i++)
  {
    if (!array[j].Flag.F1.Limit_Activate)   // �Ƿ�û��������λ��
    {
      LimitLLC (array,j);
    }
    else // ��������λ
    {
       // �Ƿ񳬳�λ��
      if ((array[j].Limit_Place > HALL_PULSE_MAX) || (array[j].Limit_Place < HALL_PULSE_MIN))
      {
        LimitLLC (array,j);
      }
      else
      {
        j++;
      }
    }
  }
  
  // ����λ������С����ǰ��
  for (i = 1; ((i <= (MID_LIMIT_POINT_AMOUNT + 2)) && (flag)); i++)   //ע���ж����� С����λ���������������н���λ��
  {
    flag = 0;        //ÿ����Ȧʱ��ʼ��
    for (j = 0; (j < (MID_LIMIT_POINT_AMOUNT + 2) - i); j++)  // �Ƿ�С�������λ������ - i
    {
      if ((array[j].Flag.F1.Limit_Activate) && (array[j + 1].Flag.F1.Limit_Activate))
      {
        if (array[j].Limit_Place > array[j+1].Limit_Place)
        {
          mid = array[j].Limit_Place;                     // ������Ƶ�����
          array[j].Limit_Place = array[j+1].Limit_Place;  // ��С������
          array[j+1].Limit_Place = mid;                   // ���������
          flag = 1;                                       //����н�������ô���Խ�����һȦѭ��
        }
      }
    }
  }
  
  // ����λ�������
  flag = 0;
  for (i = 0; i < (MID_LIMIT_POINT_AMOUNT + 2); i++)
  {
    if (Limit_Point[i].Flag.F1.Limit_Activate)   // �Ƿ�������λ��
    {
      flag++;
    }
  }
 
  return (flag);
}
/**********************************************************************************
  ������:  RFDataVerify
  ��  ��:  Ч���յ��������Ƿ���ȷ
  ��  ��:  ��
  ��  ��:  ��
  ��  ��:  �����Ƿ���ȷ��־
  ��  ����  
**********************************************************************************/
u8 RFDataVerify (void)
{
  u8 flag = NULL;
  u8 i;
  
  if (RF_RxData_Buf[0] == RF_DATA_HEAD)                        /* ͷ���Ƿ���ȷ ----*/
  {
    i = RF_RxData_Buf[1] + RF_RxData_Buf[2] + RF_RxData_Buf[3] 
      + RF_RxData_Buf[4] + RF_RxData_Buf[5] + RF_RxData_Buf[6];
    
    if (RF_RxData_Buf[7] == i)                               /* У����Ƿ���� --*/
    {
      flag = TRUE;   
      for (i = 0; i < RF_MAX_BYTE_VALUE; i++)
      {
        RF_RxData[i] = RF_RxData_Buf[i];
      }
    }
  }
  
  return (flag);
}
/**********************************************************************************
  ������:   RFDataDisposal
  ��  ��:   RF���ݴ���
  ��  ��:   ��
  ��  ��:   ��    
  ��  ��:   ��
  ��  ����  
**********************************************************************************/
void RFDataDisposal (void)
{
  u8 ch_addr;
  
  if ((!Flag.RF_Data_Disposal) && (Flag.Delete_single_Code))
  {
    if ((Delete_single_Code_ID[1] == RF_RxData[1]) &&
    (Delete_single_Code_ID[2] == RF_RxData[2]) &&
    (Delete_single_Code_ID[3] == RF_RxData[3]))
    {
      if (RF_RxData[6] == RF_DATA_CLEAR)  //  ɾȫ��
      {
        Flag.Delete_Code = TRUE;
        Flag.Delete_single_Code = NULL;
      }
      else if (RF_RxData[6] == RF_DATA_LEARN)
      {
        Delete_single_Code_Cnt = NULL;  //  �ٴΰ�ѧ����Ļ����¼�ʱ���������˳���
      }
    }
  }
  
  if ((!Flag.Set_Key_ON) && (!Flag.Opretion_Finish) && (!Flag.Del_Code_Finish) && (Flag.Electrify_Reset))
  {
    ch_addr = CompID(RF_RxData);                             /* �Ƚ�ID ----------*/
    /* �Ƿ�����ͬ��ID��ͨ�������Ҳ���ɾ��״̬ -----------------------------------*/
    if (ch_addr & 0x80)
    {
      u8 i;
   if (Flag.Goto_PCT)
  {
    Flag.Goto_PCT = NULL;
    MotorFlag.StatusSwitch = TRUE;  //����Ŀ��λ��
  }
      Flag.RF_Rx_Finish = TRUE;
      /* ����ѧϰ״̬ ----*/
      if (Flag.Learn_Code_Statu)
      {
        switch (RF_RxData[6])                                /* ��ʲô���� ------*/
        {
          case RF_DATA_UP: // ���� ѧ�루����ID��ѧͨ����
            //SaveChannel(RF_RxData,ch_addr);
            Flag.Learn_Code_Statu = NULL;
            Flag.Opretion_Finish = TRUE;
            Opretion_Event = NULL;
          break;
          case RF_DATA_STOP: // ֹͣ �㶯-�����л�
            MotorFlag.Motor_Jiggle_Status = !MotorFlag.Motor_Jiggle_Status;
            FlashWriteData(0Xee);
            Flag.Learn_Code_Statu = NULL;
          break;
          case RF_DATA_DOWN: // ���� ����
            MotorFlag.Direction = !MotorFlag.Direction;
            FlashWriteData(0Xee);
            Flag.Learn_Code_Statu = NULL;
          break;
          case RF_DATA_LEARN: // ѧ��  ɾ����
            Flag.Delete_single_Code = TRUE;
            Flag.RF_Data_Disposal = TRUE;
            
            Delete_single_Code_ID[0] = ch_addr;
            Delete_single_Code_ID[1] = RF_RxData[1];
            Delete_single_Code_ID[2] = RF_RxData[2];
            Delete_single_Code_ID[3] = RF_RxData[3];
            Delete_single_Code_ID[4] = RF_RxData[4];
            Delete_single_Code_ID[5] = RF_RxData[5];
            Flag.Learn_Code_Statu = NULL;
            
            DeleteChannel(Delete_single_Code_ID,Delete_single_Code_ID[0]);
          break;
        }
      }
      else                                                   /* ����ѧϰ״̬ ----*/
      {
        switch (RF_RxData[6])                                /* ��ʲô���� -------*/
        {
          case RF_DATA_UP: // ����
          if (!Flag.RF_Data_Disposal)
          {
            // ������λ������״̬���Ȱ�������Ϊ�趨���г�
            if (Flag.Set_Journey_Direction)
            {
              Flag.Set_Journey_Direction = NULL;
              MotorFlag.Journey_Direction = !MotorFlag.Direction;      /* ȷ���г̷��� -----*/
            }
            Motor_Status = MOTOR_UP;
            Flag.Jiggle_Stop = NULL;

            if (Limit_Point[2].Flag.F1.Limit_Activate)       /*�Ƿ����ж����λ�� */
            {
              if (!Flag.RF_Data_Disposal)
              {
                Flag.RF_DOWN_dblclick = NULL;
                Flag.RF_Data_Disposal = TRUE;
                if (Flag.RF_UP_dblclick)
                {
                  Flag.RF_UP_dblclick = NULL;
                  Flag.Run_To_finishing_point = TRUE;
                }
                else
                {
                  Flag.RF_UP_dblclick = TRUE;
                  RF_Dblclick_Time = TRUE;
                }
              }
            }
            
            #ifdef INCHING_FIXED_ANGLE
            if ((MotorFlag.Motor_Jiggle_Status) || (Flag.Set_Limit_Point) || (Flag.Low_VOL_Jiggle)
             || (!Limit_Point[1].Flag.F1.Limit_Activate))                  // Ĭ��Ϊ�㶯ģʽ
            {
              Flag.Jiggle_Stop = TRUE;
              Flag.RF_Data_Disposal = TRUE;
              Hall_Pulse_Jiggle_Buff = Hall_Pulse_Amount_Cnt;        // ���浱ǰλ��
              
              Joint_Action_Motor_Buf = Motor_Status;
              Flag.Joint_Action = TRUE;
            }
            #endif
          }
          break;
          
          case RF_CONSECUTIVE_UP: // ����
          Motor_Status = MOTOR_UP;
          Flag.Jiggle_Stop = NULL;
          Flag.RF_Data_Disposal = NULL;
          break;
          
          case RF_DATA_STOP: // ֹͣ
            Motor_Status = MOTOR_STOP;
            
            Joint_Action_Time_Cnt = 0;
            Joint_Action_Motor_Buf = MOTOR_STOP;
            Flag.Joint_Action = NULL;
          break;
          case RF_DATA_DOWN: // ����
          if (!Flag.RF_Data_Disposal)
          {
            // ������λ������״̬���Ȱ�������Ϊ�趨���г�
            if (Flag.Set_Journey_Direction)
            {
              Flag.Set_Journey_Direction = NULL;
              MotorFlag.Journey_Direction = MotorFlag.Direction;
            }
            Motor_Status = MOTOR_DOWN;
            Flag.Jiggle_Stop = NULL;
            if (Limit_Point[2].Flag.F1.Limit_Activate)       /*�Ƿ����ж����λ�� */
            {
              if (!Flag.RF_Data_Disposal)
              {
                Flag.RF_UP_dblclick = NULL;
                Flag.RF_Data_Disposal = TRUE;
                if (Flag.RF_DOWN_dblclick)
                {
                  Flag.RF_DOWN_dblclick = NULL;
                  Flag.Run_To_finishing_point = TRUE;
                }
                else
                {
                  Flag.RF_DOWN_dblclick = TRUE;
                  RF_Dblclick_Time = TRUE;
                }
              }
            }
            
            #ifdef INCHING_FIXED_ANGLE
            if ((MotorFlag.Motor_Jiggle_Status) || (Flag.Set_Limit_Point) || (Flag.Low_VOL_Jiggle)
             || (!Limit_Point[1].Flag.F1.Limit_Activate))                  // Ĭ��Ϊ�㶯ģʽ
            {
              Flag.Jiggle_Stop = TRUE;
              Flag.RF_Data_Disposal = TRUE;
              Hall_Pulse_Jiggle_Buff = Hall_Pulse_Amount_Cnt;        // ���浱ǰλ��
              
              Joint_Action_Motor_Buf = Motor_Status;
              Flag.Joint_Action = TRUE;
            }
            #endif
          }
          break;
          case RF_CONSECUTIVE_DOWN: // ����
            Motor_Status = MOTOR_DOWN;
            Flag.Jiggle_Stop = NULL;
            Flag.RF_Data_Disposal = NULL;
          break;
          case RF_JIGGLE_STOP: //  �㶯ֹͣ
          #ifndef INCHING_FIXED_ANGLE
            if ((MotorFlag.Motor_Jiggle_Status) || (Flag.Set_Limit_Point) || (Flag.Low_VOL_Jiggle)
             || (!Limit_Point[1].Flag.F1.Limit_Activate))                  // Ĭ��Ϊ�㶯ģʽ
            {
              Flag.Jiggle_Stop = TRUE;
            }
          #endif
          Joint_Action_Time_Cnt = 0;
          Joint_Action_Motor_Buf = MOTOR_STOP;
          Flag.Joint_Action = NULL;
          Flag.RF_Data_Disposal = NULL;
          break;
          
          case RF_DATA_AWOKEN: //  ����
            if ((!Flag.Set_Limit_Point) && (!Flag.Learn_Code_Statu))
            {
              Motor_Status = MOTOR_STOP;
              Motor_Status_Buffer = MOTOR_STOP;
              MotorFlag.Motor_Run = NULL;
              Flag.Learn_Code_Statu = TRUE;                    /* ����ѧϰ״̬ ----*/
              Flag.Opretion_Finish = TRUE;
              Opretion_Event = NULL;
            }
          break;
          case RF_DATA_LEARN: //  ������λ������״̬
          if (!Flag.RF_Data_Disposal)
          {
            Flag.RF_Data_Disposal = TRUE;
            Motor_Status = MOTOR_STOP;
            MotorFlag.Motor_Run = NULL;
            
            if (Flag.Set_Limit_Point)                          /* ����������λ״̬ */
            {
              Flag.Save_Limit_Point = TRUE;
              Flag.Time_50ms = NULL;
              Time_50ms_Cnt = TIME_50MS_VALUE;
            }
            else
            {
              if (!Limit_Point[0].Flag.F1.Limit_Activate)    /*�Ƿ���δ�趨��λ��*/
              {
                Flag.Set_Journey_Direction = TRUE;           /*���趨�г̷����־*/
                Flag.Set_Limit_Point = TRUE;                   /* ����������λ״̬ */
                Flag.Opretion_Finish = TRUE;
                Opretion_Event = NULL;
              }
              else                                           /* �Ѿ�����λ�� ----*/
              {
                // �Ƚϵ�ǰλ���Ƿ���ĳ����λ�����
                i = LoopCompare(Hall_Pulse_Amount_Cnt,LIMIT_PLACE);
                if (i != NULL)                               /* �Ƿ�Ҫ΢����λ�� */
                {
                  Limit_Point[--i].Flag.F1.Fine_Adjust = TRUE;/* ��΢����λ���־ */
                  Flag.Set_Limit_Point = TRUE;                   /* ����������λ״̬ */
                  Flag.Opretion_Finish = TRUE;
                  Opretion_Event = NULL;
                }
                else
                {
                  // �Ƿ���δ������6����λ��
                  if (Limit_Point_Amount < (MID_LIMIT_POINT_AMOUNT + 2))  // �Ƿ���λ����δ������
                  {
                    // �Ƿ���ĳ����λ��̫��
                    if (!(LoopCompare(Hall_Pulse_Amount_Cnt,LIMIT_SET_RANGE))) // �Ƿ����������õķ�Χ��
                    {
                      // �Ƿ�������λ������
                      if ((Hall_Pulse_Amount_Cnt > (Limit_Point[0].Limit_Place - 10)))
                      {
                        // �Ƿ��Ѿ�����������λ��
                        if (Limit_Point[1].Flag.F1.Limit_Activate)
                        {
                          // �Ƿ�������λ������
                          if (Hall_Pulse_Amount_Cnt < (Limit_Point[Limit_Point_Amount-1].Limit_Place + 10))
                          {
                            Flag.Set_Limit_Point = TRUE;                          // ����������λ״̬
                            Flag.Opretion_Finish = TRUE;
                            Opretion_Event = NULL;
                          }
                        }
                        else
                        {
                          Flag.Set_Limit_Point = TRUE;                          // ����������λ״̬
                          Flag.Opretion_Finish = TRUE;
                          Opretion_Event = NULL;
                        }
                      }
                    }
                  }
                }
              }
            }
          }
          break;
          
          case RF_DATA_CLEAR: //  ɾ��λ��
            if (Flag.Set_Limit_Point)                        /* ����������λ״̬ */
            {
              Flag.Set_Limit_Point = NULL;
              Flag.Delete_Limit_Point = TRUE;
            }
          break;
          
          case RF_UP_DOWN_5S: //  ����㶯�Ƕȵ���
            if (!Flag.Joint_Level_Adj)
            {
              Flag.Joint_Level_Adj = TRUE;
              Flag.Opretion_Finish = TRUE;
              Opretion_Event = NULL;
            }
          break;
          case RF_UP_STOP:
            if (Flag.Joint_Level_Adj)
            {
              if (Joint_Time_Level <= 9)
              {
                Joint_Time_Level++;
                FlashWriteData(0Xee);
                Motor_Status = MOTOR_UP;
                Flag.Jiggle_Stop = TRUE;
              }
            }
          break;
          
          case RF_DOWN_STOP:
            if (Flag.Joint_Level_Adj)
            {
              if (Joint_Time_Level >= 2)
              {
                Joint_Time_Level--;
                FlashWriteData(0Xee);
                Motor_Status = MOTOR_DOWN;
                Flag.Jiggle_Stop = TRUE;
              }
            }
          break;
          case RF_STOP_2S:
            if (Flag.Joint_Level_Adj)
            {
              Flag.Joint_Level_Adj = 0;
              Flag.Opretion_Finish = TRUE;
              Opretion_Event = NULL;
            }
          break;
        }
      }
    }
    else
    {
      if (Flag.Learn_Code_Statu)                             /* �Ƿ���ѧϰ״̬ --*/
      {
        if (RF_RxData[6] == RF_DATA_UP)                      /* �Ƿ����м� ------*/
        {
          if (NULL != ch_addr)                               /* �Ƿ�����ͬ��ID --*/
          {
            SaveChannel(RF_RxData,ch_addr);
          }
          else
          {
            SaveID(RF_RxData);                               /* ѧϰID ----------*/
          }
          Flag.Learn_Code_Statu = NULL;
        }
      }
    }
  }
}
/**********************************************************************************
  ������:   RFWakeup
  ��  ��:   RF���ݴ���
**********************************************************************************/
void RFWakeup (void)
{
  u8 ch_addr;
  ch_addr = CompID(RF_RxData);                             /* �Ƚ�ID ----------*/
  if (ch_addr & 0x80)
  {
    switch (RF_RxData[6])                                /* ��ʲô���� -------*/
    {
      case RF_DATA_UP:     // ����
      case RF_DATA_DOWN:   // ����
      case RF_DATA_STOP:
      case RF_JIGGLE_STOP: //  �㶯ֹͣ
      case RF_DATA_AWOKEN: //  ����
      case RF_DATA_LEARN:  //  ������λ������״̬
      case RF_ACCESS_LEARN:
      case RF_UP_DOWN_5S:  //  ������λ������״̬
      case RF_UP_STOP:
      case RF_DOWN_STOP:
      case RF_STOP_2S:
        
        Flag.RF_Wakeup = TRUE;
        WorkModeSetting(NORMAL_MODE);  // ���빤��ģʽ
      break;
    }
  }
}
/**********************************************************************************
  ������:  SetLimitPoint
  ��  ��:   
  ��  ��:  ��
  ��  ��:  ��
  ��  ��:	  
  ��  ����  
**********************************************************************************/
void SetLimitPoint (void)
{
  u8 i;
  
  if (Flag.Save_Limit_Point)
  {
    Flag.Save_Limit_Point = NULL;
    if (!Limit_Point[0].Flag.F1.Limit_Activate)              /*�Ƿ���δ�趨��λ��*/
    {
      Limit_Point[0].Flag.F1.Limit_Activate = TRUE;
      Limit_Point[0].Limit_Place = MIN_JOURNEY_VALUE;
      Hall_Pulse_Amount_Cnt = MIN_JOURNEY_VALUE;
      Target_Limit_Point = NULL;
      Limit_Point_Amount = TRUE;
      FlashWriteData(0Xee);
      Flag.Set_Limit_Point = NULL;                          // �˳�����״̬
    }
    else // �Ѿ�����λ��
    {
      if (!(LoopCompare(Hall_Pulse_Amount_Cnt,LIMIT_SET_RANGE)))  // ��ǰλ��û�к�ĳ����λ���غ�
      {
        i = LoopCompare(1,LIMIT_ADJUST);             
        if (i != NULL)                                          /* �Ƿ�Ҫ΢����λ�� */
        {
          Limit_Point[--i].Flag.F1.Fine_Adjust = NULL;           /* ��΢����λ���־ */
          Limit_Point[i].Limit_Place = Hall_Pulse_Amount_Cnt;    // ���浱ǰλ��
        }
        else // ��������λ��
        {
          i = LoopCompare(0,LIMIT_ACTIVATE);                     /* �Ƚ�����û����λ */
          if (i != NULL)                                         // �Ƿ���û��������λ��
          {
            Limit_Point[--i].Flag.F1.Limit_Activate = TRUE;   
            Limit_Point[i].Limit_Place = Hall_Pulse_Amount_Cnt;
          }
        }
        Limit_Point_Amount = LimitCollator(Limit_Point);
        FlashWriteData(0Xee);
        Flag.Set_Limit_Point = NULL;                          // �˳�����״̬
      }
    }
  }
  else if (Flag.Delete_Limit_Point)
  {
    Flag.Delete_Limit_Point = 0;
    
    i = LoopCompare(0,LIMIT_ADJUST);
    if (i != 0)                                               //�Ƿ���΢����λ״̬
    {
      Limit_Point[i - 1].Flag.F1.Fine_Adjust = 0;         // ɾ΢����λ���־
      // ��һ����λ�㲻ɾ
      if (i > 1)                                             //�Ƿ��ǵ�һ����λ
      {
        i--;
        Limit_Point[i].Flag.F1.Limit_Activate = 0;
        Limit_Point[i].Limit_Place = 0;                     // ɾ������λ��
        Limit_Point_Amount = LimitCollator(Limit_Point);    // ɾ����λ�����������               
        FlashWriteData(0Xee);
        Flag.Set_Limit_Point = 0;                       // �˳�����״̬
      }
    }
  }
}
/**********************************************************************************
  ������:  RFSignalDecode
  ��  ��:  RF�źŽ���
  ��  ��:  ��
  ��  ��:  ��
  ��  ��:  �Ƿ��յ���ȷ���ݱ�־
  ��  ����  
**********************************************************************************/
u8 RFSignalDecode (u16 High_Time,u16 Low_Time)
{
  u8 flag = NULL;
  u8 temp = High_Time + Low_Time;

  if (!Flag_RF.Signal_Head)                                  /* �Ƿ�û���ź�ͷ --*/
  {
    /*
    if ((High_Time <= HEAD_LONG_MAX) && (High_Time >= HEAD_LONG_MIN))
    {
      if ((Low_Time <= HEAD_SHORT_MAX) && (Low_Time >= HEAD_SHORT_MIN))
      {
        Flag_RF.Signal_Head = TRUE;
        Rx_Data_P = RF_RxData_Buf;
        RF_Bit_cnt = NULL;
        RF_Byte_Cnt = NULL;
        Goto_Sleep_Cnt = 0;
      }
    }*/
    
    if (Low_Time <= ((300+((AD_TIME)/2))/(AD_TIME)))       //(temp <= SIGNAL_BIT_MIN)
    {
      
      //RF_High_Cnt_Buffer += RF_High_Cnt;
     // RF_Low_Cnt_Buffer += RF_Low_Cnt;
      
      RF_High_Cnt_Buffer += RF_Low_Cnt_Buffer;
      return (flag);
      
    }
    
    if ((temp <= ((6500+((AD_TIME)/2))/(AD_TIME))) && (temp >= ((3000+((AD_TIME)/2))/(AD_TIME))))
    {
      Goto_Sleep_Cnt = 0;
      if (High_Time >= ((400+((AD_TIME)/2))/(AD_TIME)))
      {
        Flag_RF.Signal_Head = TRUE;
        Rx_Data_P = RF_RxData_Buf;
        RF_Bit_cnt = NULL;
        RF_Byte_Cnt = NULL;
        //Goto_Sleep_Cnt = 0;
      }
    }
    
    else if ((temp <= SIGNAL_BIT_MAX) && (temp >= SIGNAL_BIT_MIN))
    {
      //if ((High_Time >= SIGNAL_SHORT_MIN) && (Low_Time >= SIGNAL_SHORT_MIN))
      {
        Goto_Sleep_Cnt = 0;
      }
    }
    
  }
  else                                                       /* �����ź�ͷ ------*/
  {
    if (temp < SIGNAL_BIT_MIN)
    {
      //if ((High_Time < SIGNAL_SHORT_MIN) || (Low_Time < SIGNAL_SHORT_MIN))        //(temp <= SIGNAL_BIT_MIN)
      {
        RF_High_Cnt_Buffer += RF_High_Cnt;
        RF_Low_Cnt_Buffer += RF_Low_Cnt;
        return (flag);
      }
    }
    
    if ((temp <= SIGNAL_BIT_MAX) && (temp >= SIGNAL_BIT_MIN))
    {
      Goto_Sleep_Cnt = 0;
      
      *Rx_Data_P <<= 1;
      if (High_Time > Low_Time)
      {
        *Rx_Data_P |= 0x01;                                /* ����1bit --------*/
      }
      if (++RF_Bit_cnt >= DATA_BIT_AMOUNT)                 /* һ���ֽ��Ƿ����� */
      {
        RF_Bit_cnt = NULL;
        ++Rx_Data_P;                             
        if (++RF_Byte_Cnt >= RF_MAX_BYTE_VALUE)            /* �����Ƿ������� */
        {
          flag = TRUE;                                     /* �ô����־ ------*/
          RF_KeyUnloosen_Cnt = NULL;	                     /* ���ɼ������� ----*/
          RF_Byte_Cnt = NULL;
          ErrorDisposal();
        }
      }
    }
    else if ((temp <= ((6500+((AD_TIME)/2))/(AD_TIME))) && (temp >= ((3000+((AD_TIME)/2))/(AD_TIME))))
    {
      Goto_Sleep_Cnt = 0;
      if (High_Time >= ((400+((AD_TIME)/2))/(AD_TIME)))
      {
        Flag_RF.Signal_Head = TRUE;
        Rx_Data_P = RF_RxData_Buf;
        RF_Bit_cnt = NULL;
        RF_Byte_Cnt = NULL;
        //Goto_Sleep_Cnt = 0;
        
      }
      else
      {
        ErrorDisposal();
      }
    }
    else
    {
      ErrorDisposal();
    }
  }
  
  return (flag);
}
/**********************************************************************************
  ������:  LoopCompare
  ��  ��:   
  ��  ��:  ��
  ��  ��:  ��
  ��  ��:	  
  ��  ����  
**********************************************************************************/
u8 LoopCompare (u32 data,u8 type)
{
  u8 i;
  u8 flag = NULL;
  
  for (i = 0; i < (MID_LIMIT_POINT_AMOUNT + 2); i++)
  {
    if (type == LIMIT_ACTIVATE)
    {
      if (data == NULL)
      {
        if (!Limit_Point[i].Flag.F1.Limit_Activate)
        {
          flag = TRUE;
          break;
        }
      }
      else
      {
        if (Limit_Point[i].Flag.F1.Limit_Activate)
        {
          flag = TRUE;
          break;
        }
      } 
    }
    else if (type == LIMIT_ADJUST)
    {
      if (Limit_Point[i].Flag.F1.Fine_Adjust)
      {
        flag = TRUE;
        break;
      }
    }
    else if (type == LIMIT_PLACE)
    {
      if (Limit_Point[i].Flag.F1.Limit_Activate)
      {
        if ((Limit_Point[i].Limit_Place < (data + 40)) 
        && (Limit_Point[i].Limit_Place > (data - 40))) 
        {
          flag = TRUE;
          break;
        }
      }
    }
    else if (type == LIMIT_SET_RANGE)                                     // ��鵱ǰ�Ƿ�������������λ�ķ�Χ��
    {
      if (Limit_Point[i].Flag.F1.Limit_Activate)                          // �Ƿ��Ѿ�����λ
      {
        if ((Limit_Point[i].Limit_Place < (data + 40))                   // ��ǰλ���Ƿ�ӽ�����λ��
        && (Limit_Point[i].Limit_Place > (data - 40))) 
        {
          if (!Limit_Point[i].Flag.F1.Fine_Adjust)                        // ����΢��״̬
          {
            flag = TRUE;
          }
        }
      }
    }
  }
  
  return ((flag)? (i +1):NULL);
}
