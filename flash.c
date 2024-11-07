/**********************************************************************************
 *  项目编号:    AC213-04
 *  项目名称:    25低功耗
 *  客户名称:                                    
 *  程序编写:    符小玲
 *  启动日期:    2012-09-15
 *  完成日期:    
 *  版本号码:    V2.4
 
 *  Initialize.C file
 *  功能描述: 系统初始化
 *********************************************************************************/
#include "Define.h"                                          /* 全局宏定义 ------*/
#include "Extern.h"                                          /* 全局变量及函数 --*/
/* 内部宏 -----------------------------------------------------------------------*/
#define ID_MAX_AMOUNT          20                            /*可保存ID的最大数量*/
#define ID_LENGTH              6                             /* 数据长度 --------*/
#define ID_SEGMENT1            0x1000                        /* 数据长度 --------*/
#define ID_SEGMENT2            0x1040                        /* 数据长度 --------*/
#define DATA_SEGMENT           0x1080                        /* 数据长度 --------*/
/* 内部变量 ---------------------------------------------------------------------*/
u8 Flash_Buffer[64];
/* 内部函数 ---------------------------------------------------------------------*/
void FlashReadSegment (u8 *buf,u16 addr);
//void FlashWriteSegment  (u8 *buf,u16 addr);
void FlashWriteSegment (u16 addr,u8 num);
/**********************************************************************************
  函数名:  CompID
  功  能:   
  输  入:  空
  输  出:  空  
  返  回:  空
  描  述：  
**********************************************************************************/
u8 CompID (u8 data[])
{
  u8 i;
  u8 flag;
  u8 place;
  
  place = 1;
  flag = NULL;
  
  FlashReadSegment (Flash_Buffer,ID_SEGMENT1);     // 读数据
  
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
            flag = place;                                    /*当前ID所在的序号 -*/
            /* 两个通道中有1个字节中的1位同时为1时说明已经对号码*/
            if ((Flash_Buffer[i+4] & data[4]) || (Flash_Buffer[i+5] & data[5]))
            {
              flag |= 0x80;                                  /* 有相同通道标志 --*/
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
    FlashReadSegment (Flash_Buffer,ID_SEGMENT2);     // 读数据
    
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
            flag = place;                                    /*当前ID所在的序号 -*/
            flag |= 0x40;                                    /* 有相同通道标志 --*/
            /* 两个通道中有1个字节中的1位同时为1时说明已经对号码*/
            if ((Flash_Buffer[i+4] & data[4]) || (Flash_Buffer[i+5] & data[5]))
            {
              flag |= 0x80;                                  /* 有相同通道标志 --*/
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
  函数名:  	SEAVE_ID
  功  能:   
  输  入:   空
  输  出:   空
  返  回:	空
  描  述：  
**********************************************************************************/
void SaveID (u8 data[])
{
  u8 i;
  u8 flag;
  u8 place;
  
  place = NULL;
  flag = NULL;
  
  FlashReadSegment (Flash_Buffer,ID_SEGMENT1);     // 读数据
  
  // 比较是否已存
  for(i = 0; i < 60;)                             /* 扫描整个ID区域 --*/
  {
    if (RF_DATA_HEAD != Flash_Buffer[i])
    {
      place = i;    //  保存当前位置
      flag = TRUE;  
      break;                                       /* 没有保存过退出 --*/
    }
    i += ID_LENGTH;
  }
  if (flag == TRUE)
  {
    for(i = 0; i < 6; i++)
    {
      Flash_Buffer[place++] = data[i];
    }
    FlashWriteSegment (ID_SEGMENT1,1);     // 写数据
  }
  else
  {
    FlashReadSegment (Flash_Buffer,ID_SEGMENT2);     // 读数据
    // 比较是否已存
    for(i = 0; i < 60;)                             /* 扫描整个ID区域 --*/
    {
      if (RF_DATA_HEAD != Flash_Buffer[i])
      {
        place = i;    //  保存当前位置
        flag = TRUE;
        break;                                       /* 没有保存过退出 --*/
      }
      i += ID_LENGTH;
    }
    
    if (flag == TRUE)
    {
      for(i = 0; i < 6; i++)
      {
        Flash_Buffer[place++] = data[i];
      }
      FlashWriteSegment (ID_SEGMENT2,2);     // 写数据
    }
    else if (i == 60)
    {
      place = 54;                                      // 覆盖第19个
      for(i = 0; i < 6; i++)
      {
        Flash_Buffer[place++] = data[i];
      }
      FlashWriteSegment (ID_SEGMENT2,2);     // 写数据
    }
  }
}
/**********************************************************************************
  函数名:  	SaveChannel
  功  能:   
  输  入:   空
  输  出:   空
  返  回:	  空
  描  述：  
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
    FlashReadSegment (Flash_Buffer,ID_SEGMENT2);     // 读数据
  }
  else
  {
    FlashReadSegment (Flash_Buffer,ID_SEGMENT1);     // 读数据
  }
  
  i = (j * ID_LENGTH) - 2;            /* 得到通道位置 ----*/
  Flash_Buffer[i] |= array[4];
  Flash_Buffer[i+1] |= array[5];            /* 添加通道地址 ----*/            
  
  if (ch_addr & 0x40)
  {
    FlashWriteSegment (ID_SEGMENT2,2);     // 写数据
  }
  else
  {
    FlashWriteSegment (ID_SEGMENT1,1);     // 写数据
  }
}
/**********************************************************************************
  函数名:  	DeleteSingleChannel
  功  能:   
  输  入:   空
  输  出:   空
  返  回:	  空
  描  述：  
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
    FlashReadSegment (Flash_Buffer,ID_SEGMENT2);     // 读数据
  }
  else
  {
    FlashReadSegment (Flash_Buffer,ID_SEGMENT1);     // 读数据
  }
  
  i = (j * ID_LENGTH) - 2;            /* 得到通道位置 ----*/
  Flash_Buffer[i] &= ~array[4];
  Flash_Buffer[i+1] &= ~array[5];            /* 删除通道地址 ----*/   
  
  if ((Flash_Buffer[i] == 0) &&  (Flash_Buffer[i+1] == 0))   /* 通道是否等于0 ---*/
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
    FlashWriteSegment (ID_SEGMENT2,2);     // 写数据
  }
  else
  {
    FlashWriteSegment (ID_SEGMENT1,1);     // 写数据
  }
}
/**********************************************************************************
  函数名:  	DeleteAllID
  功  能:   
  输  入:   空
  输  出:   空
  返  回:	空
  描  述：  
**********************************************************************************/
void DeleteAllID (u8 check)
{
  unsigned char *Flash_ptr;                            // Flash pointer
  u8 i;
  
  if ((check == 0xdd) && (Flag.Delete_Code))
  {
    Flag.Delete_Code = 0;
  
  _DINT();                                             // 关总中断 
  WDTCTL = WDTPW + WDTHOLD;                            // 关看门狗
  
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
  
  _DINT();                                             // 关总中断 
  WDTCTL = WDTPW + WDTHOLD;                            // 关看门狗
  
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
  
  _EINT();                          // 开总中断 
  //VariableInitialize();
  Joint_Time_Level       = 4; 
  //FlashWriteData(0xee);
  }
}
/**********************************************************************************
  函数名:  FlashReadSegment
  功  能:   
  输  入:  空
  输  出:  空
  返  回:  空
  描  述：  
**********************************************************************************/
void FlashReadSegment (u8 *buf,u16 addr)
{
  unsigned char *Flash_ptr;                            // Flash pointer
  unsigned char i;
  
  Flash_ptr = (unsigned char *) (addr);               // 取页面地址
  for(i = 0; i < 64; i++)
  {
    *buf++ = *Flash_ptr++;
  }
}
/**********************************************************************************
  函数名:  FlashWriteSegment
  功  能:   
  输  入:  空
  输  出:  空
  返  回:  空
  描  述：  
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
  
  if (flag == 0)     // 传入参数是否出错 
  {
    return; 
  }
  else
  {
    do
    {
      _DINT();                                             // 关总中断 
      WDTCTL = WDTPW + WDTHOLD;                            // 关看门狗
    
      W_buf = Flash_Buffer;                                // 取需要写的数据
      Flash_ptr = (u8 *) (addr);                           // 取写地址
    
      FCTL1 = FWKEY + ERASE;                               // 设置擦除位
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
      W_buf = Flash_Buffer;                                // 取写数据地址
      Flash_ptr = R_buf;                                   // 取读出数据地址
      for (i = 0; i < 64; i++)  
      {
        if (*Flash_ptr++ != *W_buf++)                      //是否有不相等的
        {
          flag = TRUE;
          break;
        }
      }
    }
    while ((j < 2) && (flag));
    
    if (flag == 0)    // 是否正确保存
    {
      Flag.Learn_Code_Statu = 0;
      Flag.Opretion_Finish = TRUE;
      Opretion_Event = 0;
      Motor_Status = MOTOR_STOP;
    }
    
    _EINT();                                             // 开总中断 
  }
}
/**********************************************************************************
  函数名:  FlashWriteData
  功  能:   
  输  入:  空
  输  出:  空
  返  回:  空
  描  述：  
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
    // 保存限位点
    for (i = 0; i < (MID_LIMIT_POINT_AMOUNT + 2); i++)
    {
      Flash_Buffer[j++] = (u8) ((Limit_Point[i].Limit_Place >> 24) & 0x000000ff);
      Flash_Buffer[j++] = (u8) ((Limit_Point[i].Limit_Place >> 16) & 0x000000ff);
      Flash_Buffer[j++] = (u8) ((Limit_Point[i].Limit_Place >> 8) & 0x000000ff);
      Flash_Buffer[j++] = (u8) (Limit_Point[i].Limit_Place & 0x000000ff);
      Flash_Buffer[j++] = Limit_Point[i].Flag.F8;
    }
    // 保存标志位
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
    FlashWriteSegment(DATA_SEGMENT,check);     // 写数据
  }
}
/**********************************************************************************
  函数名:  FlashReadData
  功  能:   
  输  入:  空
  输  出:  空
  返  回:  空
  描  述：  
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
  
  FlashReadSegment (Flash_Buffer,DATA_SEGMENT);     // 读数据
  
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
