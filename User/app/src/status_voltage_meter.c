/*
*********************************************************************************************************
*
*	ģ������ : ��ѹ��
*	�ļ����� : status_voltage_meter.c
*	��    �� : V1.0
*	˵    �� : ��ѹ��
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2019-10-06 armfly  ��ʽ����
*
*	Copyright (C), 2019-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"
#include "main.h"

static void DispCH1CH2(void);
static void AutoVoltRange(void);

/*
*********************************************************************************************************
*	�� �� ��: status_VoltageMeter
*	����˵��: ��ѹ��״̬.
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void status_VoltageMeter(void)
{
	uint8_t ucKeyCode;		/* �������� */
	uint8_t fRefresh;

	DispHeader("��ѹ��");
	
	DSO_StartMode2();	/* ʾ��������ģʽ2-���ٶ�ͨ��ɨ�� */
	
	fRefresh = 1;
	
	bsp_StartAutoTimer(0, 300);
	while (g_MainStatus == MS_VOLTAGE_METER)
	{
		bsp_Idle();

		if (fRefresh)	/* ˢ���������� */
		{
			fRefresh = 0;
		
			DispCH1CH2();
			AutoVoltRange();			
		}
		
		if (bsp_CheckTimer(0))
		{
			fRefresh = 1;
		}

		ucKeyCode = bsp_GetKey();	/* ��ȡ��ֵ, �޼�����ʱ���� KEY_NONE = 0 */
		if (ucKeyCode != KEY_NONE)
		{
			/* �м����� */
			switch (ucKeyCode)
			{
				case  KEY_DOWN_S:		/* S������ */
					break;

				case  KEY_UP_S:			/* S���ͷ� */
					g_MainStatus = NextStatus(MS_VOLTAGE_METER);
					break;

				case  KEY_LONG_S:		/* S������ */
					break;				

				case  KEY_DOWN_C:		/* C������ */
					break;

				case  KEY_UP_C:			/* C���ͷ� */
					g_MainStatus = LastStatus(MS_VOLTAGE_METER);
					break;

				case  KEY_LONG_C:		/* C������ */
					break;	
				
				default:
					break;
			}
		}
	}
	bsp_StopTimer(0);
}

/*
*********************************************************************************************************
*	�� �� ��: AutoVoltRange
*	����˵��: �Զ���������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AutoVoltRange(void)
{
	float range[] = {13.8, 6.4, 3.2, 1.6, 0.8, 0.4, 0.2, 0.1};
	uint8_t i;
	
	for (i = 0; i < 8; i++)
	{
		if (g_tVar.CH1Volt >= -range[7 - i] && g_tVar.CH1Volt <= range[7 - i])
		{
			WriteRegValue_06H(0x0202, 7 - i);		/* CH1ͨ������0-7 */
			break;
		}
	}
	
	for (i = 0; i < 8; i++)
	{
		if (g_tVar.CH2Volt >= -range[7 - i] && g_tVar.CH2Volt <= range[7 - i])
		{
			WriteRegValue_06H(0x0203, 7 - i);		/* CH2ͨ������0-7 */
			break;
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: DispCH1CH2
*	����˵��: ��ʾCH1 CH2��ѹ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DispCH1CH2(void)
{
	FONT_T tFont;
	char buf[64];
	
	/* ����������� */
	{
		tFont.FontCode = FC_ST_24;			/* ������� 16���� */
		tFont.FrontColor = CL_WHITE;		/* ������ɫ */
		tFont.BackColor = HEAD_BAR_COLOR;	/* ���ֱ�����ɫ */
		tFont.Space = 0;					/* ���ּ�࣬��λ = ���� */
	}
	
	sprintf(buf, "CH1: %8.3fV", g_tVar.CH1Volt);
	LCD_DispStrEx(10, 50, buf, &tFont, 220, ALIGN_CENTER);

	sprintf(buf, "CH2: %8.3fV", g_tVar.CH2Volt);
	LCD_DispStrEx(10, 100, buf, &tFont, 220, ALIGN_CENTER);	
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
