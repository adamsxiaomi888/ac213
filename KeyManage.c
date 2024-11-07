/**********************************************************************************
 *  项目编号:    AC213-04
 *  项目名称:    25低功耗
 *  客户名称:    
 *  程序编写:    符小玲
 *  启动日期:    2012-09-15
 *  完成日期:    
 *  版本号码:    V2.4
 *
 *  KeyManage.c file
 *  功能描述: 按键扫描及处理
**********************************************************************************/
#include "Define.h"                                          /* 全局宏定义 ------*/
#include "Extern.h"                                          /* 全局变量及函数 --*/
/* 内部宏 -----------------------------------------------------------------------*/
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

/* 内部变量 ---------------------------------------------------------------------*/


u8 Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
u8 Key_OFF_Cnt = KEY_DIS_SHAKE_TIME;
u8 Key_Old_Value;
u16 Set_Key_Cnt;

static u8 CLR_Key_Num;
static u8 Delete_Key_Num;
static u16 CLR_Key_Time_Out_Cnt;
/*********************************************************************************
  函数名:   Send_Pair_WIFI_Order
  功  能:   发送配网指令
  输  入:   空
  输  出:   空    
  返  回:   空
  描  述：
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
	Uart_Tx_Array[8] = DataCRC (Uart_Tx_Array,8);; //校验
	Uart_TX_Byte_Total = 9;
        UART_TX_time = 3;
	UartStartupTX();
	//Flag_Uart.Time_200msTx = 0;
	//Time_200ms_TxCnt = TIME_200MS_TxVALUE;;*/
	
  u8 data[] = {0x02};
  UartUpload(0xE1, sizeof(data), data, 3, 0);
}

/*********************************************************************************
  函数名:   Send_Clear_WIFI_Order
  功  能:   发送清网指令
  输  入:   空
  输  出:   空    
  返  回:   空
  描  述：
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
  函数名:   KeyScan
  功  能:   扫描按键
  输  入:   空
  输  出:   空
  返  回:   空
  描  述：  
 *********************************************************************************/
void KeyManage (void)
{
  u8 new_key_value;
  
  new_key_value = KEY_IN();                                  /* 整体读键值 ------*/
  if (SET_KEY_IN(new_key_value))                             /* 是否为SET键 -----*/
  {
    if(--Key_ON_Cnt == NULL)                                 /* 是否过了消抖时间 */
    {
      Key_OFF_Cnt = KEY_DIS_SHAKE_TIME;
      Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
      
      if (++Set_Key_Cnt > LONG_KEY_MAX_TIME)  // 按键时间是否超长
      {
        Set_Key_Cnt = 0;
        Flag.Set_Key_ON = 0;                  // 不影响电机工作
        KeyFlag.SET_Dispose = TRUE;
      }
      if ((!KeyFlag.SET_Dispose) && (!Flag.Set_Limit_Point))//按键时在设置某一内容时KeyFlag.SET_Dispose置1，继续按键不再生效；Set_Limit_Point设置限位点时不执行按键
      {
        if (!Flag.Set_Key_ON)//按键按下
        {
          Time_100ms_Cnt = TIME_100MS_VALUE;
          Flag.Time_100ms = NULL;
          Flag.Set_Key_ON = TRUE;
        }
        
        if ((Set_Key_Cnt == LEARN_TIME) && (!KeyFlag.CLR_Status) && (!KeyFlag.DELETE_ALL))  // 是否学码，且没有短按4次和2次
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
            WorkModeSetting(NORMAL_MODE);                         // 进入工作模式
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
//            WorkModeSetting(NORMAL_MODE);                         // 进入正常模式
          }		  
		  else if(KeyFlag.DELETE_ALL)//将删全码更改为2短2长
		  {
		  	KeyFlag.SET_Dispose = TRUE; 
			Flag.Learn_Key_Hint = TRUE;//响蜂鸣器提醒
			if(++Delete_Key_Num==2)
			{
				Delete_Key_Num = 0;
                                CLR_Key_Num = NULL;
				KeyFlag.DELETE_ALL = 0;
				Flag.Delete_Code = TRUE;    // 删全码
				Motor_Status = MOTOR_STOP;
		  	}
		  }  
        }
        else if (Set_Key_Cnt == POWER_OFF_TIME)
        {
          KeyFlag.SET_Dispose = TRUE;
          Flag.Learn_Code_Statu = NULL;
          Motor_Status = MOTOR_STOP;
          WorkModeSetting(POWER_OFF_MODE);                         // 进入关机模式
          Flag.Set_Key_ON = 0;
          //KeyFlag.CLR_Status = 0;
		  KeyFlag.DELETE_ALL = 0;
		  Delete_Key_Num =0;
        }
        else if (!KeyFlag.CLR_Disposal)//按键一次只进来一次
        {
          KeyFlag.CLR_Disposal = TRUE;
          CLR_Key_Time_Out_Cnt = 0;
          Flag.Learn_Key_Hint = TRUE;		  
		  if(++CLR_Key_Num==3)
		  {
			 KeyFlag.DELETE_ALL = TRUE;
		  }
		  else if(CLR_Key_Num==5)//长按也要经过短按，所有长按时才置位
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
    if(--Key_OFF_Cnt == NULL)                                //是否过了消抖时间
    {
      Key_OFF_Cnt = KEY_DIS_SHAKE_TIME;
      Key_ON_Cnt = KEY_DIS_SHAKE_TIME;
      KeyFlag.SET_Dispose = NULL;
      Key_Old_Value = new_key_value;                         // 更新当前键值
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
           //LED_BLUE_ON();//蓝灯亮  YE
  if((!Flag.Clear_status_hint) && (!Flag.Clear_WIFI_Time_End))    //没有配网或配网后30秒结束，没有联网成功或联网成功3秒后执行
  {
    if(CHARGE_DETECT())//插上充电器
    {       
      LED_RED_ON();//红灯亮
      LED_BLUE_OFF();//蓝灯灭   
      if(CHARGE_STATUS())//充满电
      {
          LED_RED_OFF();//红灯灭
          LED_BLUE_ON();//蓝灯亮
      }
    }
    else//拔掉充电器
    {
      LED_BLUE_OFF();//蓝灯灭 
      LED_RED_OFF();//红灯灭
    }
  }
}

void Clear_WIFI_Hint(void)
{
  if (Flag.Clear_status_hint)
  {
    
    Flag.net_successful_count = TRUE;
//    Flag.Clear_WIFI_Time_End  = 0;
            
    if (--Clear_Status_Time_Cnt) {   //配网绿灯闪烁
      
      LED_BLUE_BLINK();
    }
    else {
      LED_BLUE_OFF();
//      Flag.Clear_WIFI_Time_End = 1;
//      Clear_Status_End_Time_Cnt = 2;
      Flag.Clear_status_hint = NULL;
    }
  }
  
  if (Flag.Clear_WIFI_Time_End){                  //联网成功后进入显示和低功耗
      
      if (--Clear_Status_End_Time_Cnt)    //连上网后常亮3秒
          {
            LED_BLUE_ON();
          }      
            else
        {
            LED_BLUE_OFF();
            Flag.Clear_WIFI_Time_End = 0;
            Flag.net_successful_count = 0;    //清计时标志
            NET_Succe_Time_Cnt = 0;             //计时器等于0            
            
            Clear_Status_Time_Cnt = 0;  //连上网3秒常亮后进入低功耗  
             

   // Clear_Status_Time_Cnt = 0;      //进入低功耗
      
        }           
  }
        
}