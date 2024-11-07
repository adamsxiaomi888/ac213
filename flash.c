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
/* �ڲ��� -----------------------------------------------------------------------*/
#define ID_MAX_AMOUNT          20                            /*�ɱ���ID���������*/
#define ID_LENGTH              6                             /* ���ݳ��� --------*/
#define ID_SEGMENT1            0x1000                        /* ���ݳ��� --------*/
#define ID_SEGMENT2            0x1040                        /* ���ݳ��� --------*/
#define DATA_SEGMENT           0x1080                        /* ���ݳ��� --------*/
/* �ڲ����� ---------------------------------------------------------------------*/
u8 Flash_Buffer[64];
/* �ڲ����� ---------------------------------------------------------------------*/
void FlashReadSegment (u8 *buf,u16 addr);
//void FlashWriteSegment  (u8 *buf,u16 addr);
void FlashWriteSegment (u16 addr,u8 num);
/**********************************************************************************
  ������:  CompID
  ��  ��:   
  ��  ��:  ��
  ��  ��:  ��  
  ��  ��:  ��
  ��  ����  
**********************************************************************************/
u8 CompID (u8 data[])
{
  u8 i;
  u8 flag;
  u8 place;
  
  place = 1;
  flag = NULL;
  
  FlashReadSegment (Flash_Buffer,ID_SEGMENT1);     // ������
  
  for(i = 0; i < 64;)
  {
    if (Flash_Buffer[i] == data[0])
    {
      if (Flash_Buffer[i+1] == data[1])
      {
        if (Flash_Buffer[i+2] == data[2])
        {
          if (Flash_Buffer[i+3] == data[3])
          {
            flag = place;                                    /*��ǰID���ڵ���� -*/
            /* ����ͨ������1���ֽ��е�1λͬʱΪ1ʱ˵���Ѿ��Ժ���*/
            if ((Flash_Buffer[i+4] & data[4]) || (Flash_Buffer[i+5] & data[5]))
            {
              flag |= 0x80;                                  /* ����ͬͨ����־ --*/
              break;
            }
          }
        }
      }
    }
    i += ID_LENGTH;
    place++;
  }
  
  if (flag == 0)
  {
    place = 1;
    FlashReadSegment (Flash_Buffer,ID_SEGMENT2);     // ������
    
    for(i = 0; i < 64;)
    {
     if (Flash_Buffer[i] == data[0])
     {
      if (Flash_Buffer[i+1] == data[1])
      {
        if (Flash_Buffer[i+2] == data[2])
        {
          if (Flash_Buffer[i+3] == data[3])
          {
            flag = place;                                    /*��ǰID���ڵ���� -*/
            flag |= 0x40;                                    /* ����ͬͨ����־ --*/
            /* ����ͨ������1���ֽ��е�1λͬʱΪ1ʱ˵���Ѿ��Ժ���*/
            if ((Flash_Buffer[i+4] & data[4]) || (Flash_Buffer[i+5] & data[5]))
            {
              flag |= 0x80;                                  /* ����ͬͨ����־ --*/
              break;
            }
          }
        }
      }
     }
     i += ID_LENGTH;
     place++;
    }
  }
  
  return (flag);
}
/**********************************************************************************
  ������:  	SEAVE_ID
  ��  ��:   
  ��  ��:   ��
  ��  ��:   ��
  ��  ��:	��
  ��  ����  
**********************************************************************************/
void SaveID (u8 data[])
{
  u8 i;
  u8 flag;
  u8 place;
  
  place = NULL;
  flag = NULL;
  
  FlashReadSegment (Flash_Buffer,ID_SEGMENT1);     // ������
  
  // �Ƚ��Ƿ��Ѵ�
  for(i = 0; i < 60;)                             /* ɨ������ID���� --*/
  {
    if (RF_DATA_HEAD != Flash_Buffer[i])
    {
      place = i;    //  ���浱ǰλ��
      flag = TRUE;  
      break;                                       /* û�б�����˳� --*/
    }
    i += ID_LENGTH;
  }
  if (flag == TRUE)
  {
    for(i = 0; i < 6; i++)
    {
      Flash_Buffer[place++] = data[i];
    }
    FlashWriteSegment (ID_SEGMENT1,1);     // д����
  }
  else
  {
    FlashReadSegment (Flash_Buffer,ID_SEGMENT2);     // ������
    // �Ƚ��Ƿ��Ѵ�
    for(i = 0; i < 60;)                             /* ɨ������ID���� --*/
    {
      if (RF_DATA_HEAD != Flash_Buffer[i])
      {
        place = i;    //  ���浱ǰλ��
        flag = TRUE;
        break;                                       /* û�б�����˳� --*/
      }
      i += ID_LENGTH;
    }
    
    if (flag == TRUE)
    {
      for(i = 0; i < 6; i++)
      {
        Flash_Buffer[place++] = data[i];
      }
      FlashWriteSegment (ID_SEGMENT2,2);     // д����
    }
    else if (i == 60)
    {
      place = 54;                                      // ���ǵ�19��
      for(i = 0; i < 6; i++)
      {
        Flash_Buffer[place++] = data[i];
      }
      FlashWriteSegment (ID_SEGMENT2,2);     // д����
    }
  }
}
/**********************************************************************************
  ������:  	SaveChannel
  ��  ��:   
  ��  ��:   ��
  ��  ��:   ��
  ��  ��:	  ��
  ��  ����  
**********************************************************************************/
void SaveChannel (u8 array[],u8 ch_addr)
{
  u8 i;
  u8 j;
  
  j = ch_addr & 0x0f;
  
  if (j > 10)
  {
    return;
  }
  
  if (ch_addr & 0x40)
  {
    FlashReadSegment (Flash_Buffer,ID_SEGMENT2);     // ������
  }
  else
  {
    FlashReadSegment (Flash_Buffer,ID_SEGMENT1);     // ������
  }
  
  i = (j * ID_LENGTH) - 2;            /* �õ�ͨ��λ�� ----*/
  Flash_Buffer[i] |= array[4];
  Flash_Buffer[i+1] |= array[5];            /* ���ͨ����ַ ----*/            
  
  if (ch_addr & 0x40)
  {
    FlashWriteSegment (ID_SEGMENT2,2);     // д����
  }
  else
  {
    FlashWriteSegment (ID_SEGMENT1,1);     // д����
  }
}
/**********************************************************************************
  ������:  	DeleteSingleChannel
  ��  ��:   
  ��  ��:   ��
  ��  ��:   ��
  ��  ��:	  ��
  ��  ����  
**********************************************************************************/
void DeleteChannel (u8 array[],u8 ch_addr)
{
  u8 i;
  u8 j;
  
  j = ch_addr & 0x0f;
  
  if (j > 10)
  {
    return;
  }
  
  if (ch_addr & 0x40)
  {
    FlashReadSegment (Flash_Buffer,ID_SEGMENT2);     // ������
  }
  else
  {
    FlashReadSegment (Flash_Buffer,ID_SEGMENT1);     // ������
  }
  
  i = (j * ID_LENGTH) - 2;            /* �õ�ͨ��λ�� ----*/
  Flash_Buffer[i] &= ~array[4];
  Flash_Buffer[i+1] &= ~array[5];            /* ɾ��ͨ����ַ ----*/   
  
  if ((Flash_Buffer[i] == 0) &&  (Flash_Buffer[i+1] == 0))   /* ͨ���Ƿ����0 ---*/
  {
    i -= 4;
    Flash_Buffer[i++] = 0;
    Flash_Buffer[i++] = 0;
    Flash_Buffer[i++] = 0;
    Flash_Buffer[i++] = 0;
    Flash_Buffer[i++] = 0;
    Flash_Buffer[i] = 0;
  }
  
  if (ch_addr & 0x40)
  {
    FlashWriteSegment (ID_SEGMENT2,2);     // д����
  }
  else
  {
    FlashWriteSegment (ID_SEGMENT1,1);     // д����
  }
}
/**********************************************************************************
  ������:  	DeleteAllID
  ��  ��:   
  ��  ��:   ��
  ��  ��:   ��
  ��  ��:	��
  ��  ����  
**********************************************************************************/
void DeleteAllID (u8 check)
{
  unsigned char *Flash_ptr;                            // Flash pointer
  u8 i;
  
  if ((check == 0xdd) && (Flag.Delete_Code))
  {
    Flag.Delete_Code = 0;
  
  _DINT();                                             // �����ж� 
  WDTCTL = WDTPW + WDTHOLD;                            // �ؿ��Ź�
  
  Hall_Pulse_Amount_Cnt = MIN_JOURNEY_VALUE; 
  MotorFlag.Direction = NULL;
  MotorFlag.Journey_Direction = NULL;
  MotorFlag.Motor_Jiggle_Status = NULL;
  Limit_Point_Amount = NULL;
  
  for (i = 0; i < (MID_LIMIT_POINT_AMOUNT + 2); i++)
  {
    Limit_Point[i].Limit_Place = NULL;
    Limit_Point[i].Flag.F1.Fine_Adjust = NULL;
    Limit_Point[i].Flag.F8 = NULL;
  }
  
  _DINT();                                             // �����ж� 
  WDTCTL = WDTPW + WDTHOLD;                            // �ؿ��Ź�
  
  Flash_ptr = (unsigned char *) (ID_SEGMENT1);         // Initialize Flash pointer D
  FCTL1 = FWKEY + ERASE;                               // Set Erase bit
  FCTL3 = FWKEY;	
  *Flash_ptr = 0;                                      // Dummy write to erase Flash segment
  
  Flash_ptr = (unsigned char *) (ID_SEGMENT2);         // Initialize Flash pointer D
  FCTL1 = FWKEY + ERASE;                               // Set Erase bit
  FCTL3 = FWKEY;	
  *Flash_ptr = 0;                                      // Dummy write to erase Flash segment
  
  Flash_ptr = (unsigned char *) (DATA_SEGMENT);        // Initialize Flash pointer D
  FCTL1 = FWKEY + ERASE;                               // Set Erase bit
  FCTL3 = FWKEY;	
  *Flash_ptr = 0;                                      // Dummy write to erase Flash segment
  
  FCTL1 = FWKEY;                                       // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                                // Set LOCK bit 
  
  Flag.Learn_Code_Statu = NULL;
  Opretion_Event = NULL;
  Motor_Status = MOTOR_STOP;
  Flag.Opretion_Finish = NULL;
  Flag.Del_Code_Finish = TRUE;
  
  _EINT();                          // �����ж� 
  //VariableInitialize();
  Joint_Time_Level       = 4; 
  //FlashWriteData(0xee);
  }
}
/**********************************************************************************
  ������:  FlashReadSegment
  ��  ��:   
  ��  ��:  ��
  ��  ��:  ��
  ��  ��:  ��
  ��  ����  
**********************************************************************************/
void FlashReadSegment (u8 *buf,u16 addr)
{
  unsigned char *Flash_ptr;                            // Flash pointer
  unsigned char i;
  
  Flash_ptr = (unsigned char *) (addr);               // ȡҳ���ַ
  for(i = 0; i < 64; i++)
  {
    *buf++ = *Flash_ptr++;
  }
}
/**********************************************************************************
  ������:  FlashWriteSegment
  ��  ��:   
  ��  ��:  ��
  ��  ��:  ��
  ��  ��:  ��
  ��  ����  
**********************************************************************************/
void FlashWriteSegment  (u16 addr,u8 num)
{
  unsigned char *W_buf;                                // Flash pointer
  unsigned char *Flash_ptr;                            // Flash pointer
  unsigned char i;
  unsigned char j = 0;
  unsigned char flag = 0;
  u8 R_buf[64];
  
  if ((num == 1) && (addr == ID_SEGMENT1))
  {
      flag = TRUE;
  }
  else if ((num == 2) && (addr == ID_SEGMENT2))
  {
      flag = TRUE;
  }
  else if ((num == 0xee) && (addr == DATA_SEGMENT))
  {
    flag = TRUE;
  }
  
  if (flag == 0)     // ��������Ƿ���� 
  {
    return; 
  }
  else
  {
    do
    {
      _DINT();                                             // �����ж� 
      WDTCTL = WDTPW + WDTHOLD;                            // �ؿ��Ź�
    
      W_buf = Flash_Buffer;                                // ȡ��Ҫд������
      Flash_ptr = (u8 *) (addr);                           // ȡд��ַ
    
      FCTL1 = FWKEY + ERASE;                               // ���ò���λ
      FCTL3 = FWKEY;
      *Flash_ptr = 0;                                      // Dummy write to erase Flash segment
      FCTL1 = FWKEY + WRT;                                 // Set WRT bit for write operation
    
      for (i = 0; i < 64; i++)  
      {
        *Flash_ptr++ = *W_buf++;                           // Write value to flash
      }
    
      FCTL1 = FWKEY;                                       // Clear WRT bit
      FCTL3 = FWKEY + LOCK;                                // Set LOCK bit 
      
      FlashReadSegment(R_buf,addr);
      
      j++;
      flag = 0;
      W_buf = Flash_Buffer;                                // ȡд���ݵ�ַ
      Flash_ptr = R_buf;                                   // ȡ�������ݵ�ַ
      for (i = 0; i < 64; i++)  
      {
        if (*Flash_ptr++ != *W_buf++)                      //�Ƿ��в���ȵ�
        {
          flag = TRUE;
          break;
        }
      }
    }
    while ((j < 2) && (flag));
    
    if (flag == 0)    // �Ƿ���ȷ����
    {
      Flag.Learn_Code_Statu = 0;
      Flag.Opretion_Finish = TRUE;
      Opretion_Event = 0;
      Motor_Status = MOTOR_STOP;
    }
    
    _EINT();                                             // �����ж� 
  }
}
/**********************************************************************************
  ������:  FlashWriteData
  ��  ��:   
  ��  ��:  ��
  ��  ��:  ��
  ��  ��:  ��
  ��  ����  
**********************************************************************************/
void FlashWriteData (u8 check)
{
  u8 i;
  u8 j = NULL;
  
  union
  {
    u8 EEprom_Flag;
    struct
    {
      u8 b0 :1;
      u8 b1:1;
      u8 b2 :1;
      u8 b3 :1;
      u8 b4 :1;
      u8 b5 :1;
      u8 b6 :1;
      u8 b7 :1;
    }E_Flag;
  }Save;
  
  if (check == 0xee)
  {
    // ������λ��
    for (i = 0; i < (MID_LIMIT_POINT_AMOUNT + 2); i++)
    {
      Flash_Buffer[j++] = (u8) ((Limit_Point[i].Limit_Place >> 24) & 0x000000ff);
      Flash_Buffer[j++] = (u8) ((Limit_Point[i].Limit_Place >> 16) & 0x000000ff);
      Flash_Buffer[j++] = (u8) ((Limit_Point[i].Limit_Place >> 8) & 0x000000ff);
      Flash_Buffer[j++] = (u8) (Limit_Point[i].Limit_Place & 0x000000ff);
      Flash_Buffer[j++] = Limit_Point[i].Flag.F8;
    }
    // �����־λ
    Save.EEprom_Flag = 0;
    Save.E_Flag.b0 = MotorFlag.Direction;
    Save.E_Flag.b1 = MotorFlag.Journey_Direction;
    Save.E_Flag.b2 = MotorFlag.Motor_Jiggle_Status;
    //Save.E_Flag.b3 = Hall_Array[0].vol_status;
    //Save.E_Flag.b4 = Hall_Array[1].vol_status;
    //Save.E_Flag.b5 = Hall_Array[0].edge;
    //Save.E_Flag.b6 = Hall_Array[1].edge;
    // Save.E_Flag.b7 = Hall_Array[0].edge_change;
  
    Flash_Buffer[j++] = Save.EEprom_Flag;
    
    Flash_Buffer[j++] = (u8) (Hall_Pulse_Amount_Cnt & 0x000000ff);
    Flash_Buffer[j++] = (u8) ((Hall_Pulse_Amount_Cnt >> 8) & 0x000000ff);
    Flash_Buffer[j++] = (u8) ((Hall_Pulse_Amount_Cnt >> 16) & 0x000000ff);
    Flash_Buffer[j++] = (u8) ((Hall_Pulse_Amount_Cnt >> 24) & 0x000000ff);
    
    Flash_Buffer[j++] = Limit_Point_Amount;
    Flash_Buffer[j++] = Hall_Status_Buffer;
    Flash_Buffer[j++] = Joint_Time_Level;
    Flash_Buffer[j++] = 0x5d;
 //   Flash_Buffer[j++] =Hall_Pulse_Amount;
    Flash_Buffer[j] = 0;
    
    for (i = 0; i < j; i++)
    {
      Flash_Buffer[j] ^= Flash_Buffer[i];
    }
    FlashWriteSegment(DATA_SEGMENT,check);     // д����
  }
}
/**********************************************************************************
  ������:  FlashReadData
  ��  ��:   
  ��  ��:  ��
  ��  ��:  ��
  ��  ��:  ��
  ��  ����  
**********************************************************************************/
void FlashReadData (void)
{
  u8 i;
  u8 j = NULL;
  
  union
  {
    u8 EEprom_Flag;
    struct
    {
      u8 b0 :1;
      u8 b1:1;
      u8 b2 :1;
      u8 b3 :1;
      u8 b4 :1;
      u8 b5 :1;
      u8 b6 :1;
      u8 b7 :1;
    }E_Flag;
  }Save;
  
  FlashReadSegment (Flash_Buffer,DATA_SEGMENT);     // ������
  
  for (i = 0; i < 40; i++)
  {
    j ^= Flash_Buffer[i];
  }
  
  if (NULL == j)
  {
    if (0x5d == Flash_Buffer[i - 2])
    {
      i -= 3;

      Joint_Time_Level = Flash_Buffer[i--];	  
      Hall_Status_Buffer = Flash_Buffer[i--];
      Limit_Point_Amount = Flash_Buffer[i--];
      
      Hall_Pulse_Amount_Cnt = Flash_Buffer[i--];
      Hall_Pulse_Amount_Cnt <<= 8;
      Hall_Pulse_Amount_Cnt |= Flash_Buffer[i--];
      Hall_Pulse_Amount_Cnt <<= 8;
      Hall_Pulse_Amount_Cnt |= Flash_Buffer[i--];
      Hall_Pulse_Amount_Cnt <<= 8;
      Hall_Pulse_Amount_Cnt |= Flash_Buffer[i--];
      
      Save.EEprom_Flag = Flash_Buffer[i--];
      MotorFlag.Direction            = Save.E_Flag.b0;
      MotorFlag.Journey_Direction    = Save.E_Flag.b1;
      MotorFlag.Motor_Jiggle_Status  = Save.E_Flag.b2;
     // Hall_Array[0].vol_status  = Save.E_Flag.b3;
     // Hall_Array[1].vol_status  = Save.E_Flag.b4;
     // Hall_Array[0].edge        = Save.E_Flag.b5;
     // Hall_Array[1].edge        = Save.E_Flag.b6;
     // Hall_Array[0].edge_change = Save.E_Flag.b7;
      
      for (i = 0; i < (MID_LIMIT_POINT_AMOUNT + 2); i++)
      {
        Limit_Point[i].Limit_Place = Flash_Buffer[j++];
        Limit_Point[i].Limit_Place <<= 8;
        Limit_Point[i].Limit_Place |= Flash_Buffer[j++];
        Limit_Point[i].Limit_Place <<= 8;
        Limit_Point[i].Limit_Place |= Flash_Buffer[j++];
        Limit_Point[i].Limit_Place <<= 8;
        Limit_Point[i].Limit_Place |= Flash_Buffer[j++];
        
        Limit_Point[i].Flag.F8 = Flash_Buffer[j++];
      }
    }
  }
}
