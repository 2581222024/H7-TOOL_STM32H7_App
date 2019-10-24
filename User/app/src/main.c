/*
*********************************************************************************************************
*
*	ģ������ : H7-TOOL App������
*	�ļ����� : main.c
*	��    �� : V1.0
*	˵    �� : 
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2019-10-01 armfly  ��ʽ����
*
*	Copyright (C), 2019-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"		/* printf����������������ڣ����Ա����������ļ� */
#include "main.h"

#include "status_link_mode.h"
#include "status_voltage_meter.h"
#include "status_current_meter.h"
#include "status_resistor_meter.h"
#include "status_temp_meter.h"
#include "status_programmer.h"
#include "status_system_set.h"

#include "wifi_if.h"
#include "ff.h"
#include "ff_gen_drv.h"
#include "emmc_diskio_dma.h"

#include "lwip_user.h"
#include "lua_if.h"

#include "target_reset.h"
#include "target_config.h"
#include "swd_host.h"
#include "usbd_user.h"

static void DispLogo(void);

uint16_t g_MainStatus;

/* ��״̬�л�˳�� */
static const uint16_t StatusOrder[] = 
{
	MS_LINK_MODE,		/* ����״̬ */
	MS_VOLTAGE_METER,	/* ��ѹ�� */	
	MS_RESISTOR_METER,	/* ����� */	
	MS_CURRENT_METER,	/* �߲������ */
	MS_TEMP_METER,		/* �¶ȱ� */
	MS_PROGRAMMER,		/* �ѻ������� */	
};


/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: c�������
*	��    �Σ���
*	�� �� ֵ: �������(���账��)
*********************************************************************************************************
*/
int main(void)
{	
	bsp_Init();	
	LoadParam();	/* ��ȡӦ�ó������, �ú�����param.c */
	
	PERIOD_Start(&g_tRunLed, 50, 50, 0);		/* LEDһֱ��˸, ������ */
	
	DispLogo();
		
	bsp_InitESP32();
	
	DSO_InitHard();
	DSO_SetDC(1,1);
	DSO_SetDC(2,1);
	DSO_SetGain(1, 3);
	DSO_SetGain(2, 3);
	
	//usbd_OpenMassStorage();
	bsp_SetTVCC(3300);
	
	/* LwIP ��ʼ�� */
	{
		/* ����������ߣ��˺���ִ��ʱ����� */
		/* �������������ȫ�ֱ��� g_tParam.lwip_ip, g_tParam.lwip_net_mask, g_tParam.lwip_gateway */
		lwip_start();
		
		lwip_pro();
	}	
	
	PERIOD_Stop(&g_tRunLed);	/* ֹͣLED��˸ */
	
	//wifi_state = WIFI_INIT;
	
	/* ���������״̬��ʵ�ֳ������л� */
	g_MainStatus = MS_LINK_MODE;	/* ��ʼ״̬ = �������� */
	while (1)
	{
		switch (g_MainStatus)
		{	
			case MS_LINK_MODE:		/* ����״̬ */
				status_LinkMode();
				break;
			
			case MS_SYSTEM_SET:		/* ϵͳ���� */
				status_SystemSetMain();
				break;

			case MS_HARD_INFO:		/* Ӳ����Ϣ */
				status_HardInfo();
				break;	
			
			case MS_ESP32_TEST:		/* ESP32ģ��̼����� */
				status_ESP32Test();
				break;	
			
			case MS_USB_UART1:		/* USB���⴮�ڣ�ӳ�䵽Ӳ��UART1�� RS485 RS232 */
				status_UsbUart1();
				break;
			
			case MS_PROGRAMMER:		/* �ѻ������� */	
				status_Programmer();
				break;

			case MS_VOLTAGE_METER:	/* ��ѹ�� */
				status_VoltageMeter();
				break;
				
			case MS_CURRENT_METER:	/* �߲������ */
				status_CurrentMeter();
				break;
			
			case MS_TEMP_METER:		/* �¶ȱ� */
				status_TempMeter();
				break;
			
			case MS_RESISTOR_METER:	/* ����� */
				status_ResistorMeter();
				break;

			default:
				g_MainStatus = MS_LINK_MODE;
				break;
		}
	}
}	

/*
*********************************************************************************************************
*	�� �� ��: NextStatus
*	����˵��: ״̬�л�, ���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint16_t NextStatus(uint16_t _NowStatus)
{
	uint16_t next;
	uint16_t i;
	uint16_t count;
	
	count = sizeof(StatusOrder) / 2;

	for (i = 0; i < count; i++)
	{
		if (_NowStatus == StatusOrder[i])
		{
			next = i;
			break;
		}
	}
	
	if (++next >= count)
	{
		next = 0;
	}
	
	#if SWITCH_BEEP_ENABLE == 1
		BEEP_KeyTone();
	#endif
	return StatusOrder[next];
}

/*
*********************************************************************************************************
*	�� �� ��: LastStatus
*	����˵��: ״̬�л�, ��ǰ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint16_t LastStatus(uint16_t _NowStatus)
{
	uint16_t next;
	uint16_t i;
	uint16_t count;
	
	count = sizeof(StatusOrder) / 2;

	for (i = 0; i < count; i++)
	{
		if (_NowStatus == StatusOrder[i])
		{
			next = i;
			break;
		}
	}
	
	if (next > 0)
	{
		next--;
	}
	else
	{
		next = count - 1;
	}
	#if SWITCH_BEEP_ENABLE == 1
		BEEP_KeyTone();
	#endif	
	return StatusOrder[next];
}

/*
*********************************************************************************************************
*	�� �� ��: DispLogo
*	����˵��: ������ʾ�汾��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DispLogo(void)
{
	FONT_T tFont;		/* ��������ṹ����� */
	
	tFont.FontCode = FC_ST_16;		/* ������� 16���� */
	tFont.FrontColor = CL_YELLOW;	/* ������ɫ */
	tFont.BackColor = FORM_BACK_COLOR;	/* ���ֱ�����ɫ */
	tFont.Space = 0;				/* ���ּ�࣬��λ = ���� */

	ST7789_SetDirection(g_tParam.DispDir);
	
	LCD_ClrScr(FORM_BACK_COLOR);  	/* ������������ɫ */
	
	/* ��ʾAPP�̼��汾���汾�ŷ����ж������� */
	{
		char buf[64];
		uint16_t x = 5;
		uint16_t y = 3;
		uint16_t line_cap = 20;

		LCD_DispStr(x , y, "H7-TOOL�๦�ܿ�������", &tFont);
		y += line_cap;
		
		sprintf(buf, "App Ver:%d.%02X",
			APP_VERSION >> 8, APP_VERSION & 0xFF);
		LCD_DispStr(x , y, buf, &tFont);

		y += line_cap;
		sprintf(buf, "Boot Ver:%d.%02X",
			BOOT_VERSION >> 8, BOOT_VERSION & 0xFF);
		LCD_DispStr(x , y, buf, &tFont);			
		
		y += line_cap;
		LCD_DispStr(x , y, "������������...", &tFont);	
	}
		
	LCD_SetBackLight(BRIGHT_DEFAULT);	 /* �򿪱��⣬����Ϊȱʡ���� */
}

/*
*********************************************************************************************************
*	�� �� ��: DispHeader
*	����˵��: ��ʾ������⣨̧ͷ��1�У�
*	��    ��: _str : ��������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DispHeader(char * _str)
{
	FONT_T tFont;
	
	/* ����������� */
	{
		tFont.FontCode = FC_ST_24;			/* ������� 16���� */
		tFont.FrontColor = CL_WHITE;		/* ������ɫ */
		tFont.BackColor = HEAD_BAR_COLOR;	/* ���ֱ�����ɫ */
		tFont.Space = 0;					/* ���ּ�࣬��λ = ���� */
	}
	
	LCD_DispStrEx(0, 0, _str, &tFont, 240, ALIGN_CENTER);
	
	LCD_Fill_Rect(0, 24, 240 - 24, 240, FORM_BACK_COLOR);	/* ����  */
}


/*
*********************************************************************************************************
*	�� �� ��: DSO_StartMode2
*	����˵��: ����ʾ����ŷʽ2����ͨ������ɨ��ģʽ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DSO_StartMode2(void)
{
	WriteRegValue_06H(0x01FF, 2);		/* ��ͨ�����ٲ��� */
	WriteRegValue_06H(0x0200, 1);		/* CH1ѡDC��� */
	WriteRegValue_06H(0x0201, 1);		/* CH2ѡDC��� */
	WriteRegValue_06H(0x0202, 0);		/* CH1ͨ������0�������Ŵ� */
	WriteRegValue_06H(0x0203, 0);		/* CH2ͨ������0�������Ŵ� */
	WriteRegValue_06H(0x0204, 0);		/* CH1ͨ��ֱ��ƫֵ��δ�� */
	WriteRegValue_06H(0x0205, 0);		/* CH2ͨ��ֱ��ƫֵ��δ�� */
	WriteRegValue_06H(0x0206, 12);		/* ����Ƶ��1M */
	WriteRegValue_06H(0x0207, 0);		/* �������1K */
	WriteRegValue_06H(0x0208, 0);		/* ������ƽ */
	WriteRegValue_06H(0x0209, 50);		/* ����λ�� */
	WriteRegValue_06H(0x020A, 0);		/* ����ģʽ 0=�Զ� */
	WriteRegValue_06H(0x020B, 0);		/* ����ͨ��CH1 */
	WriteRegValue_06H(0x020C, 0);		/* �������� */
	WriteRegValue_06H(0x020D, 2);		/* ͨ��ʹ�� */
	WriteRegValue_06H(0x020E, 1);		/* ��ʼ�ɼ� */
}

/*
*********************************************************************************************************
*	�� �� ��: TestFunc
*	����˵��: ��ʱ���Ժ�����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if 0
static void TestFunc(void)
{
	#if 0	/* ����eeprom���� */
	
	ee_WriteBytes("12345678",0,  8);
	ee_WriteBytes("87981234",2048 - 8,  8);
	
	#endif
	
	#if 0	/* ����SWD�ӿ� */
	{
		static uint8_t read_buf[2048];
				
		sysTickInit();
		
		bsp_SetTVCC(3.3 * 1000);	/* ���ýӿڵ�ƽ3.3V */
		bsp_DelayUS(100 * 100);		/* �ӳ�100ms */
		
		swd_init_debug();	/* ����swd debug״̬ */
		

		while (1)
		{
			/******** �˶δ���ִ��ʱ�� 5.2ms *************/
				/* д2048�ֽڵ�STM32F030�ڴ� */
				memset(read_buf, 0x55, 2048);
				swd_write_memory(0x20000000, read_buf, 2048);
				
				/* ��STM32F030�ڴ�2048�ֽ� */
				memset(read_buf, 0, 2048);
				swd_read_memory(0x20000000, read_buf, 2048);   

				swd_read_memory(0x58020000, read_buf, 2048);   
			
			bsp_DelayUS(50000);	/* �ӳ�1ms */		
		}
	}
	#endif
	
	#if 0
	{
		uint8_t ucWaveIdx = 0;
		int16_t volt_min = -10000;
		int16_t volt_max = 10000;
		uint32_t freq = 1000;
		uint16_t duty = 50;
			bsp_InitDAC1();	
		g_tDacWave.VoltRange = 1;
		g_tDacWave.CycleSetting = 0;			
		g_tDacWave.VoltMin = volt_min;
		g_tDacWave.VoltMax = volt_max;
		g_tDacWave.Freq = freq;
		g_tDacWave.Duty = duty;

		g_tDacWave.Type = DAC_WAVE_SIN;			

	
		dac1_StartDacWave();
		
		while(1);
	}
	#endif	
}
#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
