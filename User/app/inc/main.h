/*
*********************************************************************************************************
*
*	ģ������ : mainģ��
*	�ļ����� : main.h
*
*********************************************************************************************************
*/

#ifndef _MAIN_H_
#define _MAIN_H_

#include "ui_def.h"
#include "param.h"
#include "modbus_register.h"

#define SWITCH_BEEP_ENABLE	1		/* 1��ʾ�л���״̬ʱ������������ */

/* ������״̬�ֶ���, MS = Main Status */
enum
{
	MS_LINK_MODE = 0,	/* ����״̬ */
	
	MS_SYSTEM_SET,		/* ϵͳ���� */
	MS_HARD_INFO,		/* ����-Ӳ����Ϣ */
	MS_ESP32_TEST,		/* ESP32ģ������״̬ */
	MS_USB_UART1,		/* ���⴮��״̬��RS232 RS485 TTL-UART */

	MS_PROGRAMMER,		/* �ѻ������� */	
	MS_VOLTAGE_METER,	/* ��ѹ�� */
	MS_CURRENT_METER,	/* �߲������ */
	MS_TEMP_METER,		/* �¶ȱ� */
	MS_RESISTOR_METER,	/* ����� */
};

void DispHeader(char *_str);
uint16_t NextStatus(uint16_t _NowStatus);
uint16_t LastStatus(uint16_t _NowStatus);
void DSO_StartMode2(void);

extern uint16_t g_MainStatus;



#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

