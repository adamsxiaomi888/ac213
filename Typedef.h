

typedef signed long  s32;
typedef signed short s16;
typedef signed char  s8;

typedef signed long  const 	sc32;            /* Read Only */
typedef signed short const 	sc16;            /* Read Only */
typedef signed char  const  sc8;             /* Read Only */

typedef volatile signed long  vs32;
typedef volatile signed short vs16;
typedef volatile signed char  vs8;

//typedef volatile signed long  const  vsc32;  /* Read Only */
//ypedef volatile signed short const  vsc16;  /* Read Only */
//typedef volatile signed char  const  vsc8;   /* Read Only */

typedef unsigned long  u32;
typedef unsigned short u16;
typedef unsigned char  u8;

typedef unsigned long  const  uc32;           /* Read Only */
typedef unsigned short const  uc16;           /* Read Only */
typedef unsigned char  const  c8;  	          /* Read Only */

typedef volatile unsigned long   vu32;
typedef volatile unsigned short  vu16;
typedef volatile unsigned char   vu8;


//typedef volatile unsigned long  const  vuc32; /* Read Only */
//typedef volatile unsigned short  const vuc16; /* Read Only */
//typedef volatile unsigned char  const  vuc8;  /* Read Only */

typedef struct
{
  union
  {
    struct
    {
      u8 Limit_Activate            : 1;
      u8 Fine_Adjust               : 1;                      /* 微调限位点标志 --*/
    }F1;
    u8 F8;
  }Flag;
  
  u32 Limit_Place;
}Limit;

//#define __no_init 

typedef struct
{
  u8 vol_status                : 1;
  u8 edge                      : 1;
  u8 edge_change               : 1;
  u8 const HALL;
  u8 sample_value;
  u8 sample_cnt;
  u16 time_cnt;
}Hall;

#define WORK_STATUS_NUM           4
typedef void (*WorkStatusManage) (void);
typedef enum {
  NORMAL_MODE                       = (u8)0x00,                   /* 正常模式 --------*/
  SLEEP_MODE                        = (u8)0x01,                   /* 睡眠模式 --------*/
  POWER_OFF_MODE                    = (u8)0x02,                   /* 关机模式 --------*/
  POWER_DOWN_MODE                   = (u8)0x03,                   /* 掉电模式 --------*/
} Work_Status_TypeDef;

typedef u8 (*Verify)(const u8 *array,u8 amount);
