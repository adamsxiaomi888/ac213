/**********************************************************************************
 *  ��Ŀ���:    AC235-01
 *  ��Ŀ����:    ���������������
 *  �ͻ�����:                                    
 *  �����д:    ��С��
 *  ��������:    2013-01-31
 *  �������:    
 *  �汾����:    V1.1
 *
 *  LINUart.c file
 *  ��������: 
**********************************************************************************/
#include "Define.h"                                          /* ȫ�ֺ궨�� ------*/
#include "Extern.h"                                          /* ȫ�ֱ��������� --*/
#include "string.h"
/* �ڲ��� -----------------------------------------------------------------------*/
#define BAUD_RATE              2400                          // ������
#define BAUD_RATE_VALUE(var)        ((10000/((var)/100)) * SYSTEM_CLOCK)
#define a(var)        ((10/((var)/100)) * 32)
#define UART_RX_BYTE           10
#define UART_TX_BYTE           10 + 2
#define EUSART_NO_RX_TIME      4

#define EUSART_DATA_HEAD       0x9a                          /* �����ź�ͷ ------*/
#define EUSART_DATA_UP         0xdd                          /* ������������ ----*/
#define EUSART_DATA_STOP       0xcc                          /* ����ֹͣ���� ----*/
#define EUSART_DATA_DOWN       0xee                          /* ������������ ----*/
#define EUSART_DATA_LEARN      0xaa                          /* ����ѧϰ���� ----*/
#define EUSART_DATA_CLEAR      0xa6                          /* ����������� ----*/
#define EUSART_DATA_ORDER      0x0a                          /* ���������־ ----*/
#define EUSART_DATA_INQUIRE    0xb3                          /* ���ڲ�ѯ��־ ----*/

#define INQUIRE_FUNCTION       0xCA                          /* ��ѯ����״̬ ----*/
#define EUSART_DATA_GOTO_PCT   0x0D                          /* ���е��ٷֱ� ----*/

//2018-12-17���
#define DATA_TYPE_USER_OPT_0X11	 (0x11)	/*	�û��ɲ���������*/
#define DATA_TYPE_SET_LIMIT_0X22 (0x22)	/*	���ÿ�ѡ���ܡ���λ��*/

#define EUSART_DATA_SET_SPEED  0xD9                          /* �趨���ת�� ----*/
#define SET_DRY_CONTACT        0xD2                          /* �趨�ֿط�ʽ ----*/
#define SET_HAND_DRAG          0xD5                          /* �趨����������� */
#define SET_BAUD_RATE          0xDA                          /* ���ڲ����� ------*/
#define PERCENT_INQUIRE        0xA1                          /* �ٷֱȲ�ԃ ------*/
#define ELECTRIC_INQUIRE       0xA2                          /* ������ԃ ------*/

#define EUSART_JIGGLE_UP       0x0d                          /* �㶯�������� ----*/
#define EUSART_JIGGLE_DOWN     0x0e                          /* �㶯�������� ----*/
#define EUSART_DATA_LIMIT_1    0x01                          /* �м���λ��1 -----*/
#define EUSART_DATA_LIMIT_2    0x02                          /* �м���λ��2 -----*/
#define EUSART_DATA_LIMIT_3    0x03                          /* �м���λ��3 -----*/
#define EUSART_DATA_LIMIT_4    0x04                          /* �м���λ��4 -----*/
#define SEND_DATA_HEAD         0xd8                          /* �����ź�ͷ ------*/
/* �ڲ����� ---------------------------------------------------------------------*/
void UartDataDisposal (void);
u8 JourneyPercentCount (void);
/* �ڲ����� ---------------------------------------------------------------------*/
static u8 Uart_Byte_Cnt;                // �ֽڼ�����
static u8 Uart_TX_Byte_Cnt;             // �ֽڼ�����
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
  ������:   LinUartSetting
  ��  ��:   Uart ����
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
  ������:   DataCRC
  ��  ��:   ѭ�����Ч��
  ��  ��:   ��Ч���������ַ��Ч�����ݵĸ���
  ��  ��:   ��    
  ��  ��:   ���Ľ��
  ��  ����
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
  for (i = start_element; i < amount; i++)    // �Ƿ��Ѿ�ȫ�������
  {                                    // ȡ��һ����
    crc ^= array[i];                   // ѭ����� 
  }
  return (crc);
}

/**********************************************************************************
  ������:   DataVerify_ValidCom
  ��  ��:   Э���ڶ���ָ����ЧУ����???
**********************************************************************************/
/*u8 DataVerify_ValidCom(const u8 *array,u8 amount) {	
	
	return 0x31;
}*/

/**********************************************************************************
  ������:	DataVerify_InvalidCom
  ��  ��: 	Э���ڶ���ָ����ЧУ����???
**********************************************************************************/
/*u8 DataVerify_InvalidCom(const u8 *array,u8 amount) {
	
	return 0xce;
}*/

/**********************************************************************************
  ������:   UartUpload
  ��  ��:   ����Э��������ͨ�����ڷ��͵���λ��
�������:	�������ͣ����ݳ��ȣ�����ָ��, ���η��͵�֡���� �ط��Ĵ���
**********************************************************************************/
//void UartUpload(u8 dataType, u8 dataLength, const u8 *data, Verify dataVerify) {	
void UartUpload(u8 dataType, u8 dataLength, const u8 *data, u8 frameNum, u8 resendNum) {	

	u8 *pData = Uart_Tx_Array;								
	u8 idx = 0;
	//memset(Uart_Tx_Array, 1, sizeof(Uart_Tx_Array));		//������
#ifdef PREFIX_ENABLE
	memcpy(pData, prefix, sizeof(prefix));					//���ʹ���˷���ǰ׺�������ǰ׺�����ͻ�����
	idx += sizeof(prefix);
#endif
	pData[idx++] = EUSART_DATA_HEAD;			//����ͷ��
	pData[idx++] = dataType;					//������������
	pData[idx++] = dataLength;					//�������ݳ���
	memcpy(&pData[idx], data, dataLength);		//��������
	idx += dataLength;
	//pData[idx] = dataVerify(pData, idx);		//����У����
	pData[idx] = DataCRC(4,pData, idx);
	Uart_TX_Byte_Total = idx + 1;				//���÷��Ͳ���
	UART_TX_time = frameNum;
	//Uart_Resend_Cnt = resendNum;
	UartStartupTX();							//��ʼ����
}

/**********************************************************************************
  ������:   MotorControl
  ��  ��:   
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
  ������:   MotorControl
  ��  ��:   
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
  ������:   UartStartupTX
  ��  ��:   ׼�����ݣ���������
  ��  ��:   ��
  ��  ��:   ��
  ��  ��:   ��
  ��  ����
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
    Uart_Tx_Array[7] = 0xca;     // ��������״̬
    
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
  Uart_TX_Byte_Cnt = 0;                   // �����ֽ�����0
  IE2 |= UCA0TXIE;                        // Enable USCI_A0 TX interrupt
  IE2 &= ~UCA0RXIE;                       // Disable USCI_A0 RX interrupt
  IFG1 |= UCA0TXIFG;                      // �����жϱ�־�����뷢���жϳ���
}
/*********************************************************************************
  ������:   Uart_TX_Interrupt
  ��  ��:   ���ڷ����ж�
*********************************************************************************/
// USCI A0/B0 Transmit ISR
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{
  UCA0TXBUF = Uart_Tx_Array[Uart_TX_Byte_Cnt];	  // û���������
  if (++Uart_TX_Byte_Cnt >= Uart_TX_Byte_Total)         // �ж��Ƿ��ѷ����� 
  {
    Uart_TX_Byte_Cnt = 0;                         // ���ʹ�����0 
    if(--UART_TX_time <=0)                //����n������
    {
    	IE2 &= ~UCA0TXIE;                             // Disable USCI_A0 TX interrupt
    	IE2 |= UCA0RXIE;                              // Enable USCI_A0 RX interrupt
    }
  }
}
/*********************************************************************************
  ������:   Uart_RX_Interrupt
  ��  ��:   ���ڽ����ж�
*********************************************************************************/
/*#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
  if (!Flag.Uart_Read_OK)
  {
    Uart_Rx_Array[Uart_Byte_Cnt] = UCA0RXBUF;
    if (++Uart_Byte_Cnt >= UART_RX_BYTE ) // �Ƿ��ѽ������
    {
      Uart_Byte_Cnt = NULL;				   // ������ֽڼ�����
      Flag.Uart_Read_OK = TRUE;		   // �ý�����ɱ�־
      Flag.Uart_rx_OK = TRUE;		     // �ý�����ɱ�־
    }
    if (Uart_Rx_Array[0] != EUSART_DATA_HEAD)
    {
      Flag.Uart_Read_OK = NULL;     // ����NO OK --------
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

  if (!Flag.Uart_rx_OK)//���ڽ��մ������
  {
        Uart_Rx_Array[Uart_Byte_Cnt++] = UCA0RXBUF; 
        if(Uart_Rx_Array[0] != 0x9a)
        {
            Uart_Byte_Cnt = 0;	 
        } 
        if (( Uart_Byte_Cnt >= Uart_Rx_Array[2] + 4))//�����Ƿ����  
        {
            Uart_Byte_Cnt = 0;				   // ������ֽڼ�����
            Flag.Uart_rx_OK = TRUE;		   // �ý�����ɱ�־
            if(Work_Status == SLEEP_MODE)
            {
                WorkModeSetting(NORMAL_MODE); //�������˯��ģʽ����ʱ��������ģʽȥִ��
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
  ������:   UartControl
  ��  ��:   
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
    MotorFlag.StatusSwitch = TRUE;  //����Ŀ��λ��
  }
}
/*********************************************************************************
  ������:   LearnUartID
  ��  ��:   
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
  ������:   InquirePercentCount
  ��  ��:   
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
     Uart_Tx_Array[11] = DataCRC (Uart_Tx_Array,11); //У��
     Uart_TX_Byte_Total = 12;
     UART_TX_time = 3;
     UartStartupTX();*/  
	/* u8 data[] = {0x00, JourneyPercentCount(), 0x00, 0x00};
	 UartUpload(0xA1, sizeof(data), data, 3, 0);
}
*/
/*********************************************************************************
  ������:   UartDataDisposal
  ��  ��:   ���ڽ��յ����ݴ���
  ��  ��:   ���ڽ��յ����ݻ���������ַ
  ��  ��:   ��    
  ��  ��:   ��
  ��  ����
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
          UartControl(Uart_Rx_Array[3]);                     // �������
            /*Uart_Tx_Array[0] = 0x00;
            Uart_Tx_Array[1] = 0xff;
            Uart_Tx_Array[2] = 0x00;
            Uart_Tx_Array[3] = 0x00;
            Uart_Tx_Array[4] = 0x9a;
            Uart_Tx_Array[5] = 0x0d;
            Uart_Tx_Array[6] = 0x01;
            Uart_Tx_Array[7] = 0x5a;
            Uart_Tx_Array[8] = DataCRC (Uart_Tx_Array,8); //У��
            Uart_TX_Byte_Total = 9;
            UART_TX_time = 3;
            UartStartupTX();*/
		   u8 data[] = {0x5a};
		   UartUpload(EUSART_DATA_ORDER, sizeof(data), data, 3, 0);
        }

      else if (EUSART_DATA_GOTO_PCT == Uart_Rx_Array[1])                        /*�ٷֱȿ���*/
      {
          if (Limit_Point_Amount >= 2)       /*�Ƿ�������λ�� */
          {
          Flag.Goto_PCT = TRUE;
          MotorFlag.StatusSwitch = TRUE;     //����Ŀ��λ��
          
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
            Uart_Tx_Array[8] = DataCRC (Uart_Tx_Array,8); //У��
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
            Uart_Tx_Array[8] = DataCRC (Uart_Tx_Array,8); //У��
            Uart_TX_Byte_Total = 9;
            UART_TX_time = 3;
            UartStartupTX();*/
            u8 data[] = {0xa5};
            UartUpload(EUSART_DATA_GOTO_PCT, sizeof(data), data, 3, 0);
          }
      }
      else if (PERCENT_INQUIRE == Uart_Rx_Array[1])                             /*�ٷֱȲ�ѯ*/
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
            Uart_Tx_Array[11] = DataCRC (Uart_Tx_Array,11); //У��
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
	Uart_Tx_Array[12] = DataCRC (Uart_Tx_Array,12); //У��
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
            Uart_Tx_Array[8] = DataCRC (Uart_Tx_Array,8); //У��
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
            Uart_Tx_Array[10] = DataCRC (Uart_Tx_Array,8); //У��
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
	  	
	MotorFlag.Direction = !MotorFlag.Direction;		//���÷���
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
                if(!Limit_Point[0].Flag.F1.Limit_Activate) //û�����ù���λ��
                {
                      MotorFlag.Journey_Direction = !MotorFlag.Direction;	//�����г̷���
                      Flag.Save_Limit_Point = TRUE;							//׼��������λ��
                      Motor_Status = MOTOR_STOP;							//ֹͣ���
                }
                else if((MotorFlag.Journey_Direction == MotorFlag.Direction) &&	//���ù��г̷��������ù���һ����λ��
                                (!Limit_Point[1].Flag.F1.Limit_Activate))
                {
                      Flag.Save_Limit_Point = TRUE;			 				//׼��������λ�� 
                      Motor_Status = MOTOR_STOP;							//ֹͣ���
                }	
                break;
              case 0x02:
                if(!Limit_Point[0].Flag.F1.Limit_Activate)	//��δ���ù���λ��
                {
                      MotorFlag.Journey_Direction = MotorFlag.Direction;	//�����г̷���
                      Flag.Save_Limit_Point = TRUE;							//׼��������λ��
                      Motor_Status = MOTOR_STOP;							//ֹͣ���
                }
                else if((MotorFlag.Journey_Direction == !MotorFlag.Direction) &&	//���ù��г̷��������ù���һ����λ��
                                (!Limit_Point[1].Flag.F1.Limit_Activate))
                {
                      Flag.Save_Limit_Point = TRUE;							//׼��������λ��
                      Motor_Status = MOTOR_STOP;							//ֹͣ���
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
                    Hall_Pulse_Jiggle_Buff = Hall_Pulse_Amount_Cnt;        // ���浱ǰλ��
                
                    break;
            case 0x20:  
                  Motor_Status = MOTOR_DOWN;
                  Flag.Jiggle_Stop = TRUE;                
                  Hall_Pulse_Jiggle_Buff = Hall_Pulse_Amount_Cnt;        // ���浱ǰλ��
                  
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
  ������:   Uart_Status_Inspect
  ��  ��:   ����ͨѶ״̬���
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