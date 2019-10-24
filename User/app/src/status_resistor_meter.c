/*
*********************************************************************************************************
*
*	ģ������ : �����
*	�ļ����� : status_resitor_meter.c
*	��    �� : V1.0
*	˵    �� : �����
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2018-12-06 armfly  ��ʽ����
*
*	Copyright (C), 2018-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"
#include "main.h"

static void DispResistor(void);

/*
*********************************************************************************************************
*	�� �� ��: status_ResistorMeter
*	����˵��: �����״̬
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void status_ResistorMeter(void)
{
	uint8_t ucKeyCode;		/* �������� */
	uint8_t fRefresh;

	DispHeader("�������");
	
	fRefresh = 1;
	bsp_StartAutoTimer(0, 300);
	while (g_MainStatus == MS_RESISTOR_METER)
	{
		bsp_Idle();

		if (fRefresh)	/* ˢ���������� */
		{
			fRefresh = 0;					
			DispResistor();
		}

		/* ��·���� 20ŷ */
		if (g_tVar.NTCRes < (float)0.02)
		{
			BEEP_Start(10,10,0);
		}
		else
		{
			BEEP_Stop();
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
					g_MainStatus = NextStatus(MS_RESISTOR_METER);
					break;

				case  KEY_LONG_S:		/* S������ */
					break;				

				case  KEY_DOWN_C:		/* C������ */
					break;

				case  KEY_UP_C:			/* C���ͷ� */
					g_MainStatus = LastStatus(MS_RESISTOR_METER);
					break;

				case  KEY_LONG_C:		/* C������ */
					break;	
				
				default:
					break;
			}
		}
	}
	bsp_StopTimer(0);
	BEEP_Stop();
}

/*
*********************************************************************************************************
*	�� �� ��: DispResistor
*	����˵��: ��ʾ����ֵ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DispResistor(void)
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
	
	if (g_tVar.NTCRes < 1.0)
	{
		sprintf(buf, "����: %0.1f��", g_tVar.NTCRes * 1000);
	}
	else if (g_tVar.NTCRes < 1000)
	{
		sprintf(buf, "����: %0.3fK��", g_tVar.NTCRes);
	}
	else
	{
		sprintf(buf, "����: > 1M��" );
	}

	LCD_DispStrEx(10, 50, buf, &tFont, 220, ALIGN_CENTER);
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
