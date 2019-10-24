/*
*********************************************************************************************************
*
*	ģ������ : �¶ȱ�
*	�ļ����� : status_temp_meter.c
*	��    �� : V1.0
*	˵    �� : �¶ȱ�
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

static void DispTemp(void);

/*
*********************************************************************************************************
*	�� �� ��: status_TempMeter
*	����˵��: �¶ȱ�״̬
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void status_TempMeter(void)
{
	uint8_t ucKeyCode;		/* �������� */
	uint8_t fRefresh;

	DispHeader("�¶Ȳ���");
	
	fRefresh = 1;
	bsp_StartAutoTimer(0, 200);	
	while (g_MainStatus == MS_TEMP_METER)
	{
		bsp_Idle();

		if (fRefresh)	/* ˢ���������� */
		{
			fRefresh = 0;
			DispTemp();
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
					g_MainStatus = NextStatus(MS_TEMP_METER);
					break;

				case  KEY_LONG_S:		/* S������ */
					break;				

				case  KEY_DOWN_C:		/* C������ */
					break;

				case  KEY_UP_C:			/* C���ͷ� */
					g_MainStatus = LastStatus(MS_TEMP_METER);					
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
*	�� �� ��: DispTemp
*	����˵��: ��ʾ������¶�ֵ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DispTemp(void)
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
	
	if (g_tVar.NTCRes > 1000)
	{
		sprintf(buf, "����: ---- K��");
		LCD_DispStrEx(10, 50, buf, &tFont, 220, ALIGN_CENTER);

		sprintf(buf, "�¶�: ---- ��");
		LCD_DispStrEx(10, 100, buf, &tFont, 220, ALIGN_CENTER);		
	}
	else
	{
		sprintf(buf, "����: %0.3fK��", g_tVar.NTCRes);
		LCD_DispStrEx(10, 50, buf, &tFont, 220, ALIGN_CENTER);

		sprintf(buf, "�¶�: %0.2f��", g_tVar.NTCTemp);
		LCD_DispStrEx(10, 100, buf, &tFont, 220, ALIGN_CENTER);		
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
