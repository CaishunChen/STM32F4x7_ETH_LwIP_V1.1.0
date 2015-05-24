#ifndef __RTC_H
#define __RTC_H

#include <stdint.h>

typedef struct _DATA_TIME_STRUCT
{
    uint8_t Hours;
    uint8_t Minutes;
    uint8_t Senconds; 
    uint8_t WeekDay;
    uint8_t Month;
    uint8_t Date;
    uint8_t Year;
}DATA_TIME_STRUCT;

extern uint8_t tmp_hour, tmp_minute, tmp_second;
extern uint8_t tmp_data, tmp_week, tmp_month, tmp_year; 

uint8_t AppRTC_Init(void);
void    RTC_Config(void);
void    RTC_TimeShow(DATA_TIME_STRUCT *pDataTime);
void    RTC_AlarmShow(void);
void    RTC_TimeRegulate(void);
void    RTC_GetTime_HM(uint16_t *);

#endif

