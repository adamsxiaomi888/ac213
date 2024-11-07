/**********************************************************************************
 *  ��Ŀ���:    AC213-04
 *  ��Ŀ����:    25�͹���
 *  �ͻ�����:    
 *  �����д:    ��С��
 *  ��������:    2012-09-15
 *  �������:    
 *  �汾����:    V2.4
 *
 *  KeyManage.c file
 *  ��������: ����ɨ�輰����
**********************************************************************************/
#include "Define.h"                                          /* ȫ�ֺ궨�� ------*/
#include "Extern.h"                                          /* ȫ�ֱ��������� --*/
/* �ڲ��� -----------------------------------------------------------------------*/
#define KEY_DIS_SHAKE_TIME     20
#define LEARN_TIME             (1000  / (SYSTEM_TIME * (KEY_DIS_SHAKE_TIME)))
#define CLR_TIME               (3000  / (SYSTEM_TIME * (KEY_DIS_SHAKE_TIME)))
#define CLR_OUT_TIME           (1500  / (SYSTEM_TIME * (KEY_DIS_SHAKE_TIME)))
#define POWER_OFF_TIME         (7000  / (SYSTEM_TIME * (KEY_DIS_SHAKE_TIME)))
#define LONG_KEY_MAX_TIME      (15000 / (SYSTEM_TIME * (KEY_DIS_SHAKE_TIME)))

#define SET_KEY                BIT7
#define NO_KEY                 BIT7
#define KEY_IN()               (P2IN & BIT7)
#define SET_KEY_IN(var)        (((var) & SET_KEY)?0:1)

/* �ڲ����� ---------------------------------------------------------------------*/


u8 Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
u8 Key_OFF_Cnt = KEY_DIS_SHAKE_TIME;
u8 Key_Old_Value;
u16 Set_Key_Cnt;

static u8 CLR_Key_Num;
static u8 Delete_Key_Num;
static u16 CLR_Key_Time_Out_Cnt;
/*********************************************************************************
  ������:   Send_Pair_WIFI_Order
  ��  ��:   ��������ָ��
  ��  ��:   ��
  ��  ��:   ��    
  ��  ��:   ��
  ��  ����
*********************************************************************************/
void Send_Pair_WIFI_Order (void)
{
	/*Uart_Tx_Array[0] = 0x00;
	Uart_Tx_Array[1] = 0xff;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	Uart_Tx_Array[2] = 0x00;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	Uart_Tx_Array[3] = 0x00;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	Uart_Tx_Array[4] = 0x9a;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	Uart_Tx_Array[5] = 0xe1;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	Uart_Tx_Array[6] = 0x01;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	Uart_Tx_Array[7] = 0x02;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	Uart_Tx_Array[8] = DataCRC (Uart_Tx_Array,8);; //У��
	Uart_TX_Byte_Total = 9;
        UART_TX_time = 3;
	UartStartupTX();
	//Flag_Uart.Time_200msTx = 0;
	//Time_200ms_TxCnt = TIME_200MS_TxVALUE;;*/
	
  u8 data[] = {0x02};
  UartUpload(0xE1, sizeof(data), data, 3, 0);
}

/*********************************************************************************
  ������:   Send_Clear_WIFI_Order
  ��  ��:   ��������ָ��
  ��  ��:   ��
  ��  ��:   ��    
  ��  ��:   ��
  ��  ����
*********************************************************************************/
void Send_Clear_WIFI_Order (void)
{
  /*
            Uart_Tx_Array[0] = 0x00;
            Uart_Tx_Array[1] = 0xff;
            Uart_Tx_Array[2] = 0x00;
            Uart_Tx_Array[3] = 0x00;
            Uart_Tx_Array[4] = 0x9a;
            Uart_Tx_Array[5] = 0xE1;
            Uart_Tx_Array[6] = 0x01;
            Uart_Tx_Array[7] = 0x01;
            Uart_Tx_Array[8] = DataCRC (Uart_Tx_Array,8);
            Uart_TX_Byte_Total = 9;
            UART_TX_time = 3;
            UartStartupTX();*/
	//Flag_Uart.Time_200msTx = 0;
	//Time_200ms_TxCnt = TIME_200MS_TxVALUE;
	
  u8 data[] = {0x01};
  UartUpload(0xE1, sizeof(data), data, 3, 0);
}

/**********************************************************************************
  ������:   KeyScan
  ��  ��:   ɨ�谴��
  ��  ��:   ��
  ��  ��:   ��
  ��  ��:   ��
  ��  ����  
 *********************************************************************************/
void KeyManage (void)
{
  u8 new_key_value;
  
  new_key_value = KEY_IN();                                  /* �������ֵ ------*/
  if (SET_KEY_IN(new_key_value))                             /* �Ƿ�ΪSET�� -----*/
  {
    if(--Key_ON_Cnt == NULL)                                 /* �Ƿ��������ʱ�� */
    {
      Key_OFF_Cnt = KEY_DIS_SHAKE_TIME;
      Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
      
      if (++Set_Key_Cnt > LONG_KEY_MAX_TIME)  // ����ʱ���Ƿ񳬳�
      {
        Set_Key_Cnt = 0;
        Flag.Set_Key_ON = 0;                  // ��Ӱ��������
        KeyFlag.SET_Dispose = TRUE;
      }
      if ((!KeyFlag.SET_Dispose) && (!Flag.Set_Limit_Point))//����ʱ������ĳһ����ʱKeyFlag.SET_Dispose��1����������������Ч��Set_Limit_Point������λ��ʱ��ִ�а���
      {
        if (!Flag.Set_Key_ON)//��������
        {
          Time_100ms_Cnt = TIME_100MS_VALUE;
          Flag.Time_100ms = NULL;
          Flag.Set_Key_ON = TRUE;
        }
        
        if ((Set_Key_Cnt == LEARN_TIME) && (!KeyFlag.CLR_Status) && (!KeyFlag.DELETE_ALL))  // �Ƿ�ѧ�룬��û�ж̰�4�κ�2��
        {
          if (NORMAL_MODE == Work_Status)
          {
            Flag.Learn_Code_Statu = TRUE;
            Flag.Opretion_Finish = TRUE;
            Opretion_Event = NULL;
            Flag.Time_100ms = TRUE;
            Motor_Status = MOTOR_STOP;
          }
          else
          {
            KeyFlag.SET_Dispose = TRUE; 
            WorkModeSetting(NORMAL_MODE);                         // ���빤��ģʽ
            Flag.Learn_Key_Hint = TRUE;
          }
        }
        else if (Set_Key_Cnt == CLR_TIME)
        {
          if (KeyFlag.CLR_Status)
          {
            CLR_Key_Num = 0;
            KeyFlag.CLR_Status = 0;
            KeyFlag.SET_Dispose = TRUE; 
            Send_Clear_WIFI_Order();
            Flag.Clear_status = 1;
            Flag.Clear_status_hint = 1;
            Clear_Status_Time_Cnt = 30;
            Flag.Opretion_Finish = TRUE;
            Opretion_Event = NULL;
            Motor_Status = MOTOR_STOP;
//            WorkModeSetting(NORMAL_MODE);                         // ��������ģʽ
          }		  
		  else if(KeyFlag.DELETE_ALL)//��ɾȫ�����Ϊ2��2��
		  {
		  	KeyFlag.SET_Dispose = TRUE; 
			Flag.Learn_Key_Hint = TRUE;//�����������
			if(++Delete_Key_Num==2)
			{
				Delete_Key_Num = 0;
                                CLR_Key_Num = NULL;
				KeyFlag.DELETE_ALL = 0;
				Flag.Delete_Code = TRUE;    // ɾȫ��
				Motor_Status = MOTOR_STOP;
		  	}
		  }  
        }
        else if (Set_Key_Cnt == POWER_OFF_TIME)
        {
          KeyFlag.SET_Dispose = TRUE;
          Flag.Learn_Code_Statu = NULL;
          Motor_Status = MOTOR_STOP;
          WorkModeSetting(POWER_OFF_MODE);                         // ����ػ�ģʽ
          Flag.Set_Key_ON = 0;
          //KeyFlag.CLR_Status = 0;
		  KeyFlag.DELETE_ALL = 0;
		  Delete_Key_Num =0;
        }
        else if (!KeyFlag.CLR_Disposal)//����һ��ֻ����һ��
        {
          KeyFlag.CLR_Disposal = TRUE;
          CLR_Key_Time_Out_Cnt = 0;
          Flag.Learn_Key_Hint = TRUE;		  
		  if(++CLR_Key_Num==3)
		  {
			 KeyFlag.DELETE_ALL = TRUE;
		  }
		  else if(CLR_Key_Num==5)//����ҲҪ�����̰������г���ʱ����λ
		  {
           	 KeyFlag.CLR_Status = TRUE;
			 KeyFlag.DELETE_ALL = NULL;
		  }
		  else if((CLR_Key_Num>=6))
		  {
			 KeyFlag.DELETE_ALL = NULL;
			 KeyFlag.CLR_Status = NULL;
		  }  

		  if((CLR_Key_Num==4)&&(!Delete_Key_Num))
		  {
			KeyFlag.DELETE_ALL = 0;			
		  }
        }
      }
    }
  }
  else
  {
    if(--Key_OFF_Cnt == NULL)                                //�Ƿ��������ʱ��
    {
      Key_OFF_Cnt = KEY_DIS_SHAKE_TIME;
      Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
      KeyFlag.SET_Dispose = NULL;
      Key_Old_Value = new_key_value;                         // ���µ�ǰ��ֵ
      Set_Key_Cnt = NULL;
      Flag.Set_Key_ON = NULL;
      
      KeyFlag.CLR_Disposal = NULL;
      if (++CLR_Key_Time_Out_Cnt >= CLR_OUT_TIME)
      {
        KeyFlag.CLR_Status = 0;
		KeyFlag.DELETE_ALL = 0;
        CLR_Key_Num = 0;
		Delete_Key_Num = 0;
        CLR_Key_Time_Out_Cnt = 0;
      }
    }
  }
           //LED_BLUE_ON();//������  YE
  if((!Flag.Clear_status_hint) && (!Flag.Clear_WIFI_Time_End))    //û��������������30�������û�������ɹ��������ɹ�3���ִ��
  {
    if(CHARGE_DETECT())//���ϳ����
    {       
      LED_RED_ON();//�����
      LED_BLUE_OFF();//������   
      if(CHARGE_STATUS())//������
      {
          LED_RED_OFF();//�����
          LED_BLUE_ON();//������
      }
    }
    else//�ε������
    {
      LED_BLUE_OFF();//������ 
      LED_RED_OFF();//�����
    }
  }
}

void Clear_WIFI_Hint(void)
{
  if (Flag.Clear_status_hint)
  {
    
    Flag.net_successful_count = TRUE;
//    Flag.Clear_WIFI_Time_End  = 0;
            
    if (--Clear_Status_Time_Cnt) {   //�����̵���˸
      
      LED_BLUE_BLINK();
    }
    else {
      LED_BLUE_OFF();
//      Flag.Clear_WIFI_Time_End = 1;
//      Clear_Status_End_Time_Cnt = 2;
      Flag.Clear_status_hint = NULL;
    }
  }
  
  if (Flag.Clear_WIFI_Time_End){                  //�����ɹ��������ʾ�͵͹���
      
      if (--Clear_Status_End_Time_Cnt)    //����������3��
          {
            LED_BLUE_ON();
          }      
            else
        {
            LED_BLUE_OFF();
            Flag.Clear_WIFI_Time_End = 0;
            Flag.net_successful_count = 0;    //���ʱ��־
            NET_Succe_Time_Cnt = 0;             //��ʱ������0            
            
            Clear_Status_Time_Cnt = 0;  //������3�볣�������͹���  
             

   // Clear_Status_Time_Cnt = 0;      //����͹���
      
        }           
  }
        
}