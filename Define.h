/**********************************************************************************
 *  ��Ŀ���:    AC213-04
 *  ��Ŀ����:    25�͹���
 *  �ͻ�����:                                    
 *  �����д:    ��С��
 *  ��������:    2012-09-15
 *  �������:    
 *  �汾����:    V2.4
 *
 *  Define.h file
 *  ��������: ȫ�ֺ궨��
**********************************************************************************/
#ifndef Define_h
    #define Define_h
    
    /* ͷ�ļ� -------------------------------------------------------------------*/
    #include "Typedef.h"                                     /* ���Ͷ���ͷ�ļ� --*/
    #include "msp430g2553.h"
    
    /* ȫ�ֺ궨�� ---------------------------------------------------------------*/
    #define NULL                   0
    #define TRUE                   1
    #define SYSTEM_CLOCK           16
    #define RF_DECODE_TIME         50
    #define SYSTEM_TIME            2
    #define SYSTEM_TIME_VALUE      SYSTEM_TIME * 1000
    #define BAUD_RATE_VALUE1(var)        ((10000/((var)/100)) * 4)
    //#define AM28                    // AM28���
    //#define EXTERNAL_POWER_SUPPLY   // ���õ�Դ
    //#define AM213_04_V45_PCB          // AM213-04���ý��յ��
    #define INCHING_FIXED_ANGLE       // �㶯�̶��Ƕ�
    
    #ifdef EXTERNAL_POWER_SUPPLY
      #define POWER_DOWN_VAULE       750  //770
      #define LOW_VOL_HINT_VALUE     830  //820
      #define VOL_NORMAL_VALUE       850  //840

      #define VOL_RECOVERY_VALUE     880  //630
    #else
      #define POWER_DOWN_VAULE       520  //550
      #define LOW_VOL_HINT_VALUE     620  //610
      #define VOL_NORMAL_VALUE       640  //630

      #define VOL_RECOVERY_VALUE     620  //630
    #endif


    #ifdef AM28
      #define STARTUP_CURRENT        1500                      /* ������������ֵ --*/
      #define MAX_OUT_CURRENT        2500
      #define HALL_REVERSE
    #else
      #define STARTUP_CURRENT        900                      /* ������������ֵ --*/
      #define MAX_OUT_CURRENT        1500
    #endif
    
    
    // AM213-04���ý��յ��
    #ifdef AM213_04_V45_PCB
      #define BUZER                  4		             /* P3.4-------------*/
      #define RF_EN                  4		             /* P2.4-------------*/
      #define RF_POWER               6		             /* P3.6-------------*/

      #define BUZZER_ON()           (P3OUT |=  (1 << BUZER))
      #define BUZZER_OFF()          (P3OUT &= ~(1 << BUZER))
      #define RF_WORK()             (P2OUT |=  (1 << RF_EN))
      #define RF_SLEEP()            (P2OUT &= ~(1 << RF_EN))
      #define RF_POWER_ON()         (P3OUT |=  (1 << RF_POWER))
      #define RF_POWER_OFF()        (P3OUT &= ~(1 << RF_POWER))
    #else // AM258-01
      #define BUZER                  5		             /* P3.5-------------*/
      #define RF_EN                  4		             /* P3.4-------------*/
      #define RF_POWER               4		             /* P2.4-------------*/

      #define BUZZER_ON()           (P3OUT |=  (1 << BUZER))
      #define BUZZER_OFF()          (P3OUT &= ~(1 << BUZER))
      #define RF_WORK()             (P3OUT |=  (1 << RF_EN))
      #define RF_SLEEP()            (P3OUT &= ~(1 << RF_EN))
      #define RF_POWER_OFF()        (P2OUT |=  (1 << RF_POWER))
      #define RF_POWER_ON()         (P2OUT &= ~(1 << RF_POWER))
    #endif

    #define NO_VOL_SAMPLING_TIME    (500 / (TIME_10MS_VALUE * SYSTEM_TIME))

    /* Ϊ�˴�������һ��ѭ����ͬʱ����50��100MS�����飬���Խ�ʱ����΢�� --*/
    #define TIME_10MS_VALUE        (16  / SYSTEM_TIME)
    #define TIME_50MS_VALUE        (45  / SYSTEM_TIME)
    #define TIME_100MS_VALUE       (103 / SYSTEM_TIME)
    #define TIME_500MS_VALUE       (500 / SYSTEM_TIME)
    #define TIME_1S_VALUE          (1000 / 500)
    #define BUZZER_HINT_TIME       (1000 / SYSTEM_TIME)

    #define HALL_PULSE_MAX         16777210
    #define MIN_JOURNEY_VALUE      8000000
    #define HALL_PULSE_MIN         1000
    #define MID_LIMIT_POINT_AMOUNT 4

    #define LIMIT_ACTIVATE         1
    #define LIMIT_ADJUST           2
    #define LIMIT_PLACE            3
    #define LIMIT_SET_RANGE        4
    
    #define SET_UP_DIRECTION()     (MotorFlag.Motor_Direction = !MotorFlag.Direction)
    #define SET_DOWN_DIRECTION()   (MotorFlag.Motor_Direction = MotorFlag.Direction)

    #define MAX_SPEED              25
    #define MIN_SPEED              8
    
    #define PWM_CYC                16
    #define MAX_DUTY               ((u16) (((SYSTEM_CLOCK * 1000) / PWM_CYC) / 2))       // up down mode ��Ҫ�ٳ�2
    #define MIN_DUTY               100

    /* RF�������� ------*/
    #define RF_DATA_HEAD           0xA3                      /* RFͷ�� ----------*/
    #define RF_DATA_UP             0x0b                      /* RF�������� ------*/
    #define RF_DATA_STOP           0x23                      /* RFֹͣ���� ------*/
    #define RF_STOP_2S             0x5a                      /* RFֹͣ����2S ----*/
    #define RF_JIGGLE_STOP         0x24                      /* RF�㶯ͣ���� ----*/
    #define RF_DATA_DOWN           0x43                      /* RF�������� ------*/
    #define RF_DATA_LEARN          0x53                      /* RFѧϰ���� ------*/
    #define RF_DATA_CLEAR          0x2b                      /* RF������� ------*/
    #define RF_DATA_AWOKEN         0x50                      /* RF�������� ------*/
    #define RF_DATA_UP_DOWN_1S     0x4a                      /*  ----------------*/

    #define RF_UP_DOWN_5S          0x3a                      /*  ----------------*/
    #define RF_UP_STOP             0x1a                      /*  ----------------*/
    #define RF_DOWN_STOP           0x2a                      /*  ----------------*/
    #define RF_ACCESS_LEARN        0x55                      /*  ----------------*/

    #define RF_CONSECUTIVE_UP      0x8B                     /* RF�������� ------*/
    #define RF_CONSECUTIVE_DOWN    0xC3                     /* RF�������� ------*/
    
    /* �������״̬ ----*/
    #define MOTOR_STOP             0                         /* ֹͣ ------------*/
    #define MOTOR_UP               1                         /* ���� ------------*/
    #define MOTOR_DOWN             2                         /* ���� ------------*/
    
    /* �������ʱ�� ----*/
    #define MOTOR_RUN_TIME         5                         /* ����� 5���� --*/
    #define MOTOR_RUN_CNT_VALUE    (MOTOR_RUN_TIME * (60000 / (TIME_10MS_VALUE * SYSTEM_TIME)))
    #define MOTOR_SWITCH_LATIME    (500 / (TIME_10MS_VALUE * SYSTEM_TIME))
    #define JOINT_ACTION_TIME      (1500 / (TIME_10MS_VALUE * SYSTEM_TIME))

    /* IO �˿ڶ��� --------------------------------------------------------------*/
    #define POWER_DOWN_DETECT()   (P2IN & BIT6)              /* P2.6 ------------*/
    #define RF_SIGNAL_SAMPLE()    (P2IN & BIT3)              /* P2.3 ------------*/
    
    #define HALL_POWEER            0		             /* P1.0-------------*/
    #define POWER_IN               7		             /* P1.7-------------*/

    #define BRAKE                  1		             /* P3.1-------------*/
    
    #define IN1_H                  2		             /* P2.2-------------*/
    #define IN1_L                  5		             /* P2.5-------------*/
    #define IN2_H                  2		             /* P3.2-------------*/
    #define IN2_L                  3		             /* P3.3-------------*/
   
    #define IN1_H_1()             (P2OUT |=  (1 << IN1_H))
    #define IN1_H_0()             (P2OUT &= ~(1 << IN1_H))
    #define IN1_L_1()             (P2OUT |=  (1 << IN1_L))
    #define IN1_L_0()             (P2OUT &= ~(1 << IN1_L))

    #define IN2_H_1()             (P3OUT |=  (1 << IN2_H))
    #define IN2_H_0()             (P3OUT &= ~(1 << IN2_H))
    #define IN2_L_1()             (P3OUT |=  (1 << IN2_L))
    #define IN2_L_0()             (P3OUT &= ~(1 << IN2_L))


	
	#define RED 	 7
	#define BLUE     6   
	
    #define HALL_POWEER_ON()      (P1OUT |=  (1 << HALL_POWEER))
    #define HALL_POWEER_OFF()     (P1OUT &= ~(1 << HALL_POWEER))
    #define POWEER_DETECT_ON()    (P1OUT |=  (1 << POWER_IN))
    #define POWEER_DETECT_OFF()   (P1OUT &= ~(1 << POWER_IN))
    #define BRAKE_OFF()           (P3OUT |=  (1 << BRAKE))
    #define BRAKE_ON()            (P3OUT &= ~(1 << BRAKE))

    #define LED_RED_ON()		  (P3OUT |=  (1 << RED))
    #define LED_RED_OFF()		  (P3OUT &= ~(1 << RED))
    #define LED_BLUE_ON()		  (P3OUT |=  (1 << BLUE))
    #define LED_BLUE_OFF()		  (P3OUT &= ~(1 << BLUE))
    #define LED_RED_BLINK()		  (P3OUT ^=  (1 << RED))
    #define LED_BLUE_BLINK()		  (P3OUT ^=  (1 << BLUE))

    
   // #define TEST_SIGNAL
    #ifdef TEST_SIGNAL
      #define test_1()            (P1OUT |= (1 << 1))
      #define test_0()            (P1OUT &= ~(1 << 1))
      #define test()              (P1OUT ^= (1 << 1))
	#else
      #define test_1()            
      #define test_0()            
      #define test()              
    #endif

    #define CHARGE_DETECT()    (P1IN & BIT3)              /* P1.3 -������Ƿ����-----------*/
	#define CHARGE_STATUS()    (P1IN & BIT5)              /* P1.5 -����Ƿ����-----------*/

	#define CHARGE_DET  5
	#define INT_OPEN	0x28
	#define INT_CLOSE	0x00
	#define CHARGE_ON	0x00//����������ж�
	#define CHARGE_OFF	0x08//������ε��ж�
	


#endif

