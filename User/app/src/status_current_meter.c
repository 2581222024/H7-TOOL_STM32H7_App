/*
*********************************************************************************************************
*
*	ģ������ : �߲��ѹ������
*	�ļ����� : status_current_meter.c
*	��    �� : V1.0
*	˵    �� : 
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2019-10-19 armfly  ��ʽ����
*
*	Copyright (C), 2018-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"
#include "main.h"

/*
*********************************************************************************************************
*	�� �� ��: status_CurrentMeter
*	����˵��: �߲��ѹ������̬.
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void status_CurrentMeter(void)
{
	uint8_t ucKeyCode;		/* �������� */
	uint8_t fRefresh;
	FONT_T tFont;		/* ��������ṹ����� */

	DispHeader("�߲������");
	
	/* ����������� */
	{
		tFont.FontCode = FC_ST_16;	/* ������� 16���� */
		tFont.FrontColor = CL_BLACK;	/* ������ɫ */
		tFont.BackColor = FORM_BACK_COLOR;	/* ���ֱ�����ɫ */
		tFont.Space = 0;				/* ���ּ�࣬��λ = ���� */
	}
	
	fRefresh = 1;
	while (g_MainStatus == MS_CURRENT_METER)
	{
		bsp_Idle();

		if (fRefresh)	/* ˢ���������� */
		{
			fRefresh = 0;
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
					g_MainStatus = NextStatus(MS_CURRENT_METER);
					break;

				case  KEY_LONG_S:		/* S������ */
					break;				

				case  KEY_DOWN_C:		/* C������ */
					break;

				case  KEY_UP_C:			/* C���ͷ� */
					g_MainStatus = LastStatus(MS_CURRENT_METER);
					break;

				case  KEY_LONG_C:		/* C������ */
					break;	
				
				default:
					break;
			}
		}
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
