/**********************************************************************************
 *  项目编号:    AC235-01
 *  项目名称:    磁悬浮开合帘电机
 *  客户名称:                                    
 *  程序编写:    符小玲
 *  启动日期:    2013-01-31
 *  完成日期:    
 *  版本号码:    V1.1
 *
 *  LINUart.c file
 *  功能描述: 
**********************************************************************************/
#include "Define.h"                                          /* 全局宏定义 ------*/
#include "Extern.h"                                          /* 全局变量及函数 --*/
#include "string.h"
/* 内部宏 -----------------------------------------------------------------------*/
#define BAUD_RATE              2400                          // 波特率
#define BAUD_RATE_VALUE(var)        ((10000/((var)/100)) * SYSTEM_CLOCK)
#define a(var)        ((10/((var)/100)) * 32)
#define UART_RX_BYTE           10
#define UART_TX_BYTE           10 + 2
#define EUSART_NO_RX_TIME      4

#define EUSART_DATA_HEAD       0x9a                          /* 接收信号头 ------*/
#define EUSART_DATA_UP         0xdd                          /* 串口上行数据 ----*/
#define EUSART_DATA_STOP       0xcc                          /* 串口停止数据 ----*/
#define EUSART_DATA_DOWN       0xee                          /* 串口下行数据 ----*/
#define EUSART_DATA_LEARN      0xaa                          /* 串口学习数据 ----*/
#define EUSART_DATA_CLEAR      0xa6                          /* 串口清除数据 ----*/
#define EUSART_DATA_ORDER      0x0a                          /* 串口命令标志 ----*/
#define EUSART_DATA_INQUIRE    0xb3                          /* 串口查询标志 ----*/

#define INQUIRE_FUNCTION       0xCA                          /* 查询功能状态 ----*/
#define EUSART_DATA_GOTO_PCT   0x0D                          /* 运行到百分比 ----*/

//2018-12-17添加
#define DATA_TYPE_USER_OPT_0X11	 (0x11)	/*	用户可操作类设置*/
#define DATA_TYPE_SET_LIMIT_0X22 (0x22)	/*	设置可选功能、限位点*/

#define EUSART_DATA_SET_SPEED  0xD9                          /* 设定最高转速 ----*/
#define SET_DRY_CONTACT        0xD2                          /* 设定手控方式 ----*/
#define SET_HAND_DRAG          0xD5                          /* 设定手拉、方向等 */
#define SET_BAUD_RATE          0xDA                          /* 串口波特率 ------*/
#define PERCENT_INQUIRE        0xA1                          /* 百分比查詢 ------*/
#define ELECTRIC_INQUIRE       0xA2                          /* 电量查詢 ------*/

#define EUSART_JIGGLE_UP       0x0d                          /* 点动上行数据 ----*/
#define EUSART_JIGGLE_DOWN     0x0e                          /* 点动下行数据 ----*/
#define EUSART_DATA_LIMIT_1    0x01                          /* 中间限位点1 -----*/
#define EUSART_DATA_LIMIT_2    0x02                          /* 中间限位点2 -----*/
#define EUSART_DATA_LIMIT_3    0x03                          /* 中间限位点3 -----*/
#define EUSART_DATA_LIMIT_4    0x04                          /* 中间限位点4 -----*/
#define SEND_DATA_HEAD         0xd8                          /* 发送信号头 ------*/
/* 内部函数 ---------------------------------------------------------------------*/
void UartDataDisposal (void);
u8 JourneyPercentCount (void);
/* 内部变量 ---------------------------------------------------------------------*/
static u8 Uart_Byte_Cnt;                // 字节计数器
static u8 Uart_TX_Byte_Cnt;             // 字节计数器
static u8 Uart_Rx_Array[UART_RX_BYTE];
static u8 Uart_NO_Rx_Cnt;
//static u8 Uart_Resend_Cnt = 0;
#define UART_RESEND_TASK_TIME (200 / SYSTEM_TIME)
//static u8 Uart_Resend_Task_Cnt = 0;
//static u8 Inquire_Function_Flag;

#define PREFIX_ENABLE
#ifdef PREFIX_ENABLE
const static u8 prefix[] = {0x00, 0xff, 0x00, 0x00};
#endif

/**********************************************************************************
  函数名:   LinUartSetting
  功  能:   Uart 设置
**********************************************************************************/
void LinUartInit (void)
{

  UCA0BR0 = BAUD_RATE_VALUE(19200) & 0xFF;         // 19200
  UCA0BR1 = BAUD_RATE_VALUE(19200) >> 8;           // 19200
  UCA0MCTL = UCBRS1; 
  
  P1SEL |= BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD
  P1SEL2 |= BIT1 + BIT2 ;                   // P1.1 = RXD, P1.2=TXD
  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  UCA0CTL1 |=0x20;
  
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
 
}
/*********************************************************************************
  函数名:   DataCRC
  功  能:   循环异或效验
  输  入:   需效验的数组首址、效验数据的个数
  输  出:   空    
  返  回:   异或的结果
  描  述：
*********************************************************************************/
u8 DataCRC (u8 start_element,const u8 *array,u8 amount)
{
  u8 crc = NULL;
  u8 i;
  /*
  for (u8 j=0;j<20;j++)
  {
    if(array[j] == 0x9a)
    {
      i=j;
      break;
    }
  }*/
  for (i = start_element; i < amount; i++)    // 是否已经全部异或完
  {                                    // 取下一个数
    crc ^= array[i];                   // 循环异或 
  }
  return (crc);
}

/**********************************************************************************
  函数名:   DataVerify_ValidCom
  功  能:   协议内定的指令有效校验码???
**********************************************************************************/
/*u8 DataVerify_ValidCom(const u8 *array,u8 amount) {	
	
	return 0x31;
}*/

/**********************************************************************************
  函数名:	DataVerify_InvalidCom
  功  能: 	协议内定的指令无效校验码???
**********************************************************************************/
/*u8 DataVerify_InvalidCom(const u8 *array,u8 amount) {
	
	return 0xce;
}*/

/**********************************************************************************
  函数名:   UartUpload
  功  能:   按照协议打包数据通过串口发送到上位机
输入参数:	数据类型，数据长度，数据指针, 单次发送的帧数， 重发的次数
**********************************************************************************/
//void UartUpload(u8 dataType, u8 dataLength, const u8 *data, Verify dataVerify) {	
void UartUpload(u8 dataType, u8 dataLength, const u8 *data, u8 frameNum, u8 resendNum) {	

	u8 *pData = Uart_Tx_Array;								
	u8 idx = 0;
	//memset(Uart_Tx_Array, 1, sizeof(Uart_Tx_Array));		//测试用
#ifdef PREFIX_ENABLE
	memcpy(pData, prefix, sizeof(prefix));					//如果使能了发送前缀，则插入前缀到发送缓冲区
	idx += sizeof(prefix);
#endif
	pData[idx++] = EUSART_DATA_HEAD;			//插入头码
	pData[idx++] = dataType;					//插入数据类型
	pData[idx++] = dataLength;					//插入数据长度
	memcpy(&pData[idx], data, dataLength);		//插入数据
	idx += dataLength;
	//pData[idx] = dataVerify(pData, idx);		//插入校验码
	pData[idx] = DataCRC(4,pData, idx);
	Uart_TX_Byte_Total = idx + 1;				//设置发送参数
	UART_TX_time = frameNum;
	//Uart_Resend_Cnt = resendNum;
	UartStartupTX();							//开始发送
}

/**********************************************************************************
  函数名:   MotorControl
  功  能:   
 *********************************************************************************/
u32 CountPercentPlace (u8 percent)
{
  u32 temp;
  u32 place;
	
  temp = Hall_Pulse_Amount - MIN_JOURNEY_VALUE;
  place = MIN_JOURNEY_VALUE;
  
  if (percent > 100)
  {
    percent = 100;
  }
  
  if (!(MotorFlag.Direction ^ MotorFlag.Journey_Direction))
  {
    percent = 100 - percent;
  }
  
  place += temp * percent / 100;

  return place;
}

/**********************************************************************************
  函数名:   MotorControl
  功  能:   
 *********************************************************************************/
u8 JourneyPercentCount (void)
{
  u32 percent;
  u32 temp;
  
  if(Limit_Point_Amount<2) return 0xff;
  Hall_Pulse_Amount = (Limit_Point[Limit_Point_Amount-1].Limit_Place);
  temp = Hall_Pulse_Amount_Cnt - MIN_JOURNEY_VALUE;

  if (Hall_Pulse_Amount_Cnt < MIN_JOURNEY_VALUE+5) percent = 0;
  else if (Hall_Pulse_Amount_Cnt > Hall_Pulse_Amount-5) percent = 100;
  else percent = temp * 100 / (Hall_Pulse_Amount - MIN_JOURNEY_VALUE);

  if (percent > 100) percent = 100;
  
  if (!(MotorFlag.Direction ^ MotorFlag.Journey_Direction))
  {
    percent = 100 - percent;
  }
  
  return (percent);
}
/*********************************************************************************
  函数名:   UartStartupTX
  功  能:   准备数据，启动发送
  输  入:   空
  输  出:   空
  返  回:   空
  描  述：
*********************************************************************************/
void UartStartupTX (void)
{/*
  Bit_TypeDef Uart;
  
  RS485_TX();
  
  Uart.Flag_Byte = 0;
  
  Uart_Tx_Array[0] = SEND_DATA_HEAD;
  Uart_Tx_Array[1] = Uart_ID;
  Uart_Tx_Array[2] = Uart_CH_L;
  Uart_Tx_Array[3] = Uart_CH_H;
  
  if (Inquire_Function_Flag)
  {
    Uart_Tx_Array[4] = Uart_Baud_Rate;
    Inquire_Function_Flag = NULL;
    Uart_Tx_Array[5] = Dry_Contact_Control_Mode;
    Uart_Tx_Array[6] = Motor_Max_Speed;
    Uart_Tx_Array[7] = 0xca;     // 反馈功能状态
    
    Uart.Flag_Bit.b0 = Motor_Array[0].Motor_Flag.Hand_Drag_Status;
    Uart.Flag_Bit.b1 = Motor_Array[0].Motor_Flag.Direction;
    Uart.Flag_Bit.b2 = 0;
    Uart.Flag_Bit.b3 = Motor_Array[0].Motor_Flag.No_Slow_Start;
    Uart.Flag_Bit.b4 = Motor_Array[0].Motor_Flag.Serpentine_Curtain;
    Uart.Flag_Bit.b5 = Motor_Array[0].Motor_Flag.To_Journey_Meet_Plugb;
    Uart.Flag_Bit.b6 = !Motor_Array[0].Motor_Flag.PowerDown_Remember;
    Uart.Flag_Bit.b7 = Motor_Array[0].Motor_Flag.Manual_Control_Synchro;
  }
  else
  {
    Uart_Tx_Array[4] = Current_Current_mA / 10;
    Uart_Tx_Array[5] = Current_Voltage;
    Uart_Tx_Array[6] = Motor_Speed;
  
    if (Motor_Array[0].Motor_Flag.Journey_Set_OK)
    {
      Uart_Tx_Array[7] = JourneyPercentCount();
    }
    else
    {
      Uart_Tx_Array[7] = 0;
    }
    
    Uart.Flag_Bit.b0 = Motor_Array[0].Motor_Flag.Run_Status;
    Uart.Flag_Bit.b1 = Motor_Array[0].Motor_Flag.Journey_Set_OK;
    Uart.Flag_Bit.b6 = Flag.Goto_PCT;
  }
  
  Uart_Tx_Array[8] = Uart.Flag_Byte;
  Uart_Tx_Array[9] = DataCRC (Uart_Tx_Array,8);
  */
  Uart_TX_Byte_Cnt = 0;                   // 发送字节数清0
  IE2 |= UCA0TXIE;                        // Enable USCI_A0 TX interrupt
  IE2 &= ~UCA0RXIE;                       // Disable USCI_A0 RX interrupt
  IFG1 |= UCA0TXIFG;                      // 设置中断标志，进入发送中断程序
}
/*********************************************************************************
  函数名:   Uart_TX_Interrupt
  功  能:   串口发送中断
*********************************************************************************/
// USCI A0/B0 Transmit ISR
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{
  UCA0TXBUF = Uart_Tx_Array[Uart_TX_Byte_Cnt];	  // 没完继续发送
  if (++Uart_TX_Byte_Cnt >= Uart_TX_Byte_Total)         // 判断是否已发送完 
  {
    Uart_TX_Byte_Cnt = 0;                         // 发送次数清0 
    if(--UART_TX_time <=0)                //发送n包数据
    {
    	IE2 &= ~UCA0TXIE;                             // Disable USCI_A0 TX interrupt
    	IE2 |= UCA0RXIE;                              // Enable USCI_A0 RX interrupt
    }
  }
}
/*********************************************************************************
  函数名:   Uart_RX_Interrupt
  功  能:   串口接收中断
*********************************************************************************/
/*#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
  if (!Flag.Uart_Read_OK)
  {
    Uart_Rx_Array[Uart_Byte_Cnt] = UCA0RXBUF;
    if (++Uart_Byte_Cnt >= UART_RX_BYTE ) // 是否已接收完成
    {
      Uart_Byte_Cnt = NULL;				   // 清接收字节计数器
      Flag.Uart_Read_OK = TRUE;		   // 置接收完成标志
      Flag.Uart_rx_OK = TRUE;		     // 置接收完成标志
    }
    if (Uart_Rx_Array[0] != EUSART_DATA_HEAD)
    {
      Flag.Uart_Read_OK = NULL;     // 接收NO OK --------
      Uart_Byte_Cnt = NULL;
    }
  }
  else
  {
    UCA0RXBUF;
  }
}*/
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{

  if (!Flag.Uart_rx_OK)//串口接收处理完成
  {
        Uart_Rx_Array[Uart_Byte_Cnt++] = UCA0RXBUF; 
        if(Uart_Rx_Array[0] != 0x9a)
        {
            Uart_Byte_Cnt = 0;	 
        } 
        if (( Uart_Byte_Cnt >= Uart_Rx_Array[2] + 4))//接收是否完成  
        {
            Uart_Byte_Cnt = 0;				   // 清接收字节计数器
            Flag.Uart_rx_OK = TRUE;		   // 置接收完成标志
            if(Work_Status == SLEEP_MODE)
            {
                WorkModeSetting(NORMAL_MODE); //如果处于睡眠模式函数时，到正常模式去执行
                Flag.Key_Wakeup = TRUE;
                Key_Wakeup_time_cnt = 0;
                _BIC_SR_IRQ(LPM3_bits); 
            }
        }
        if(Uart_Byte_Cnt >= 23)
        {
            Uart_Byte_Cnt = 0;
            Flag.Uart_rx_OK = 0;
        }
  }

  else
  {
    UCA0RXBUF;
  }
}
/*********************************************************************************
  函数名:   UartControl
  功  能:   
*********************************************************************************/
void UartControl (u8 data)
{
  switch (data)
  {
    case EUSART_DATA_UP:
      Motor_Status = MOTOR_UP;
    break;
    case EUSART_DATA_STOP:
      Motor_Status = MOTOR_STOP;
      MotorFlag.Run_Status = NULL;
    break;
    case EUSART_DATA_DOWN:
      Motor_Status = MOTOR_DOWN;
    break;
  }
  
  
   if (Flag.Goto_PCT)
  {
    Flag.Goto_PCT = NULL;
    MotorFlag.StatusSwitch = TRUE;  //更新目标位置
  }
}
/*********************************************************************************
  函数名:   LearnUartID
  功  能:   
*********************************************************************************/
void LearnUartID (u8 array[])
{
  if (EUSART_DATA_LEARN == array[5])  
  {
    Uart_ID = array[1];
    Uart_CH_L = array[2];
    Uart_CH_H = array[3];
 /*  LedStatusUpdate(OPRATION_FINISH);*/
    FlashWriteData(0xee);
  }
  else if (EUSART_DATA_CLEAR == array[5]) 
  {
    Uart_ID = 0;
    Uart_CH_L = 0;
    Uart_CH_H = 0;
 /*   LedStatusUpdate(OPRATION_FINISH);*/
    FlashWriteData(0xee);
  }
}

/*********************************************************************************
  函数名:   InquirePercentCount
  功  能:   
*********************************************************************************/
//void InquirePercentCount(void)
//{
    /* Uart_Tx_Array[0] = 0x00;
     Uart_Tx_Array[1] = 0xff;
     Uart_Tx_Array[2] = 0x00;
     Uart_Tx_Array[3] = 0x00;
     Uart_Tx_Array[4] = 0x9a;
     Uart_Tx_Array[5] = 0xA1;
     Uart_Tx_Array[6] = 0x04;
     Uart_Tx_Array[7] = 0x00;
     Uart_Tx_Array[8] = JourneyPercentCount();
     Uart_Tx_Array[9] = 0x00;
     Uart_Tx_Array[10] = 0x00;
     Uart_Tx_Array[11] = DataCRC (Uart_Tx_Array,11); //校验
     Uart_TX_Byte_Total = 12;
     UART_TX_time = 3;
     UartStartupTX();*/  
	/* u8 data[] = {0x00, JourneyPercentCount(), 0x00, 0x00};
	 UartUpload(0xA1, sizeof(data), data, 3, 0);
}
*/
/*********************************************************************************
  函数名:   UartDataDisposal
  功  能:   串口接收的数据处理
  输  入:   串口接收的数据缓存数组首址
  输  出:   空    
  返  回:   空
  描  述：
**********************************************************************************/
void UartDataDisposal (void)
{
  
  if (Uart_Rx_Array[0] == EUSART_DATA_HEAD)    
  {
    if (DataCRC(0,Uart_Rx_Array,(Uart_Rx_Array[2]+4)) == NULL)
    {
		/*if(0x5a == Uart_Rx_Array[3]) 
		{
		  if(Uart_Resend_Cnt) 
		  {
			Uart_Resend_Cnt--;
			//Uart_Resend_Task_Cnt = 0;
		  }
		}
	  	else 
		  */
	    if (EUSART_DATA_ORDER == Uart_Rx_Array[1])         
        {
          UartControl(Uart_Rx_Array[3]);                     // 控制输出
            /*Uart_Tx_Array[0] = 0x00;
            Uart_Tx_Array[1] = 0xff;
            Uart_Tx_Array[2] = 0x00;
            Uart_Tx_Array[3] = 0x00;
            Uart_Tx_Array[4] = 0x9a;
            Uart_Tx_Array[5] = 0x0d;
            Uart_Tx_Array[6] = 0x01;
            Uart_Tx_Array[7] = 0x5a;
            Uart_Tx_Array[8] = DataCRC (Uart_Tx_Array,8); //校验
            Uart_TX_Byte_Total = 9;
            UART_TX_time = 3;
            UartStartupTX();*/
		   u8 data[] = {0x5a};
		   UartUpload(EUSART_DATA_ORDER, sizeof(data), data, 3, 0);
        }

      else if (EUSART_DATA_GOTO_PCT == Uart_Rx_Array[1])                        /*百分比控制*/
      {
          if (Limit_Point_Amount >= 2)       /*是否上下限位点 */
          {
          Flag.Goto_PCT = TRUE;
          MotorFlag.StatusSwitch = TRUE;     //更新目标位置
          
          Hall_Pulse_Amount_PCT = CountPercentPlace (Uart_Rx_Array[3]);
          
          Motor_Status = MOTOR_UP;
          if (!(MotorFlag.Journey_Direction ^ MotorFlag.Direction))
          {
            if (Hall_Pulse_Amount_Cnt > Hall_Pulse_Amount_PCT)
            {
              Motor_Status = MOTOR_DOWN;
            }
          }
          else
          {
            if (Hall_Pulse_Amount_Cnt < Hall_Pulse_Amount_PCT)
            {
              Motor_Status = MOTOR_DOWN;
            }
          }
          
          if ((Hall_Pulse_Amount_Cnt > (Hall_Pulse_Amount_PCT - 10))
          && (Hall_Pulse_Amount_Cnt < (Hall_Pulse_Amount_PCT + 10)))
          {
            MotorFlag.Run_Status = NULL;
            Motor_Status = MOTOR_STOP;
            Flag.Goto_PCT = NULL;
          }
            /*Uart_Tx_Array[0] = 0x00;
            Uart_Tx_Array[1] = 0xff;
            Uart_Tx_Array[2] = 0x00;
            Uart_Tx_Array[3] = 0x00;
            Uart_Tx_Array[4] = 0x9a;
            Uart_Tx_Array[5] = 0x0d;
            Uart_Tx_Array[6] = 0x01;
            Uart_Tx_Array[7] = 0x5a;
            Uart_Tx_Array[8] = DataCRC (Uart_Tx_Array,8); //校验
            Uart_TX_Byte_Total = 9;
            UART_TX_time = 3;
            UartStartupTX();*/
		   	u8 data[] = {0x5a};
		   	UartUpload(EUSART_DATA_GOTO_PCT, sizeof(data), data, 3, 0);
          }
          else
          {
			/*
            Uart_Tx_Array[0] = 0x00;
            Uart_Tx_Array[1] = 0xff;
            Uart_Tx_Array[2] = 0x00;
            Uart_Tx_Array[3] = 0x00;
            Uart_Tx_Array[4] = 0x9a;
            Uart_Tx_Array[5] = 0x0d;
            Uart_Tx_Array[6] = 0x01;
            Uart_Tx_Array[7] = 0xa5;
            Uart_Tx_Array[8] = DataCRC (Uart_Tx_Array,8); //校验
            Uart_TX_Byte_Total = 9;
            UART_TX_time = 3;
            UartStartupTX();*/
            u8 data[] = {0xa5};
            UartUpload(EUSART_DATA_GOTO_PCT, sizeof(data), data, 3, 0);
          }
      }
      else if (PERCENT_INQUIRE == Uart_Rx_Array[1])                             /*百分比查询*/
      {

         u8 data[] = {Motor_Status, JourneyPercentCount(), 0x00, 0x00};    //    
         UartUpload(0xA1, sizeof(data), data, 3, 0);
         
          //else
          {
            /*Uart_Tx_Array[0] = 0x00;
            Uart_Tx_Array[1] = 0xff;
            Uart_Tx_Array[2] = 0x00;
            Uart_Tx_Array[3] = 0x00;
            Uart_Tx_Array[4] = 0x9a;
            Uart_Tx_Array[5] = 0xA1;
            Uart_Tx_Array[6] = 0x04;
            Uart_Tx_Array[7] = 0x00;
            Uart_Tx_Array[8] = 0xff;
            Uart_Tx_Array[9] = 0x00;
            Uart_Tx_Array[10] = 0x00;
            Uart_Tx_Array[11] = DataCRC (Uart_Tx_Array,11); //校验
            Uart_TX_Byte_Total = 12;
            UART_TX_time = 3;
            UartStartupTX();*/
		   	//u8 data[] = {0x00, 0xff, 0x00, 0x00};
		   	//UartUpload(PERCENT_INQUIRE, sizeof(data), data, 3, 0);
          }
      }
      else if (ELECTRIC_INQUIRE == Uart_Rx_Array[1]) 
      {
	/*Uart_Tx_Array[0] = 0x00;
	Uart_Tx_Array[1] = 0xff;
	Uart_Tx_Array[2] = 0x00;
	Uart_Tx_Array[3] = 0x00;
	Uart_Tx_Array[4] = 0x9a;
	Uart_Tx_Array[5] = 0xA2;
	Uart_Tx_Array[6] = 0x05;
        Uart_Tx_Array[7] = 0x00;
        Uart_Tx_Array[8] = 0x00;
        Uart_Tx_Array[9] = 0x00;
        Uart_Tx_Array[10] = 0x00;
	Uart_Tx_Array[11] = Electric_Quantity;
	Uart_Tx_Array[12] = DataCRC (Uart_Tx_Array,12); //校验
	Uart_TX_Byte_Total = 13;
        UART_TX_time = 3;
	UartStartupTX();*/
	
		u8 data[] = {0x00, 0x00, 0x00, 0x00, Electric_Quantity};
		UartUpload(ELECTRIC_INQUIRE, sizeof(data), data, 3, 0);
      }
      else if (EUSART_DATA_INQUIRE == Uart_Rx_Array[1])
      {						
            /*Uart_Tx_Array[0] = 0x00;
            Uart_Tx_Array[1] = 0xff;
            Uart_Tx_Array[2] = 0x00;
            Uart_Tx_Array[3] = 0x00;
            Uart_Tx_Array[4] = 0x9a; 
            Uart_Tx_Array[5] = 0xb3;
            Uart_Tx_Array[6] = 0x01;
            Uart_Tx_Array[7] = 0x10;
            Uart_Tx_Array[8] = DataCRC (Uart_Tx_Array,8); //校验
            Uart_TX_Byte_Total = 9;
            UART_TX_time = 3;
            UartStartupTX();*/
		u8 data[] = {0x10};
		UartUpload(EUSART_DATA_INQUIRE, sizeof(data), data, 3, 0);
		//Flag.Opretion_Finish = TRUE;
      }
      else if (0xa3 == Uart_Rx_Array[1])
      {
            /*Uart_Tx_Array[0] = 0x00;
            Uart_Tx_Array[1] = 0xff;
            Uart_Tx_Array[2] = 0x00;
            Uart_Tx_Array[3] = 0x00;
            Uart_Tx_Array[4] = 0x9a;
            Uart_Tx_Array[5] = 0xa3;
            Uart_Tx_Array[6] = 0x03;
            Uart_Tx_Array[7] = 0x00;
            Uart_Tx_Array[8] = 0x00;
            Uart_Tx_Array[9] = 0x00;
            Uart_Tx_Array[10] = DataCRC (Uart_Tx_Array,8); //校验
            Uart_TX_Byte_Total = 11;
            UART_TX_time = 3;
            UartStartupTX();*/
          u8 data[] = {0x00, 0x00, 0x00};
          UartUpload(0xa3, sizeof(data), data, 3, 0);
          //Flag.Opretion_Finish = TRUE;
      }
      else if (0xE1 == Uart_Rx_Array[1])
      {
        Flag.Clear_status = 0;
        if(Uart_Rx_Array[3] == 0x02)
        {
                Flag.Clear_status_hint = 0;
                Flag.Clear_WIFI_Time_End = 1;//ye
                Clear_Status_End_Time_Cnt = 3;
                
                
                Clear_Status_Time_Cnt = 0;
                
                
        }
//        Flag.Clear_status_hint = 0;
//        Flag.Clear_WIFI_Time_End = 1;//ye
//        Clear_Status_End_Time_Cnt = 3;
      }
      else if(DATA_TYPE_USER_OPT_0X11 == Uart_Rx_Array[1]) {
	  	
	MotorFlag.Direction = !MotorFlag.Direction;		//设置方向
        FlashWriteData(0Xee);
		
        u8 data[] = {0x5a};
        UartUpload(DATA_TYPE_USER_OPT_0X11, sizeof(data), data, 3, 0);
        Flag.Opretion_Finish = TRUE;
        Opretion_Event = NULL;
      }
      else if(DATA_TYPE_SET_LIMIT_0X22 == Uart_Rx_Array[1]) 
      { 
	//if() 
        {
          u8 cmd = Uart_Rx_Array[4];
          u8 data[]= {0x5a};
          switch(cmd) 
          {
              case 0x01:
                if(!Limit_Point[0].Flag.F1.Limit_Activate) //没有设置过限位点
                {
                      MotorFlag.Journey_Direction = !MotorFlag.Direction;	//设置行程方向
                      Flag.Save_Limit_Point = TRUE;							//准备保存限位点
                      Motor_Status = MOTOR_STOP;							//停止电机
                }
                else if((MotorFlag.Journey_Direction == MotorFlag.Direction) &&	//设置过行程方向且设置过第一个限位点
                                (!Limit_Point[1].Flag.F1.Limit_Activate))
                {
                      Flag.Save_Limit_Point = TRUE;			 				//准备保存限位点 
                      Motor_Status = MOTOR_STOP;							//停止电机
                }	
                break;
              case 0x02:
                if(!Limit_Point[0].Flag.F1.Limit_Activate)	//从未设置过限位点
                {
                      MotorFlag.Journey_Direction = MotorFlag.Direction;	//设置行程方向
                      Flag.Save_Limit_Point = TRUE;							//准备保存限位点
                      Motor_Status = MOTOR_STOP;							//停止电机
                }
                else if((MotorFlag.Journey_Direction == !MotorFlag.Direction) &&	//设置过行程方向且设置过第一个限位点
                                (!Limit_Point[1].Flag.F1.Limit_Activate))
                {
                      Flag.Save_Limit_Point = TRUE;							//准备保存限位点
                      Motor_Status = MOTOR_STOP;							//停止电机
                }
              break;
            case 0x04:
            Motor_Status = MOTOR_UP;
            
                  break;
            case 0x08:
                  
                  Motor_Status = MOTOR_DOWN;
                  
                  break;
            case 0x10:
                    Motor_Status = MOTOR_UP;
                    Flag.Jiggle_Stop = TRUE;                
                    Hall_Pulse_Jiggle_Buff = Hall_Pulse_Amount_Cnt;        // 保存当前位置
                
                    break;
            case 0x20:  
                  Motor_Status = MOTOR_DOWN;
                  Flag.Jiggle_Stop = TRUE;                
                  Hall_Pulse_Jiggle_Buff = Hall_Pulse_Amount_Cnt;        // 保存当前位置
                  
                  break;
            case 0x40:  
                  Motor_Status = MOTOR_STOP;
                  
			break;
          }
          if(Uart_Rx_Array[5]==0x04)
          {
            memset(&Limit_Point,0,sizeof(Limit_Point));
            Flag.Opretion_Finish = TRUE;
            Opretion_Event = NULL;
          }
          
		  UartUpload(DATA_TYPE_SET_LIMIT_0X22, sizeof(data), data, 3, 0);
	}
      }
    }
  }
}
/*********************************************************************************
  函数名:   Uart_Status_Inspect
  功  能:   串口通讯状态检测
*********************************************************************************/
void Uart_Status_Inspect (void)
{
  if (!Flag.Uart_tx)
  {
    if (Flag.Uart_rx_OK)
    {
      Flag.Uart_rx_OK = NULL;
      Uart_NO_Rx_Cnt = NULL;
    }
    else if (Uart_Byte_Cnt != 0)
    {
      if (++Uart_NO_Rx_Cnt >= EUSART_NO_RX_TIME)
      {
        Uart_NO_Rx_Cnt = NULL;
        Uart_Byte_Cnt = NULL;
        Flag.Uart_Disposal = NULL;
      }
    }
  }
}