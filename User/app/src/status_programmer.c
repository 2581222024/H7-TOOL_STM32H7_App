/*
*********************************************************************************************************
*
*	ģ������ : �ѻ������
*	�ļ����� : status_programmer.c
*	��    �� : V1.0
*	˵    �� : �ѻ������
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

/*
*********************************************************************************************************
*	�� �� ��: status_Programmmer
*	����˵��: �ѻ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void status_Programmer(void)
{
	uint8_t ucKeyCode;		/* �������� */
	uint8_t fRefresh;

	DispHeader("��¼��");
	
	fRefresh = 1;
	while (g_MainStatus == MS_PROGRAMMER)
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
					g_MainStatus = NextStatus(MS_PROGRAMMER);
					break;

				case  KEY_LONG_S:		/* S������ */
					break;				

				case  KEY_DOWN_C:		/* C������ */
					break;

				case  KEY_UP_C:			/* C���ͷ� */
					g_MainStatus = LastStatus(MS_PROGRAMMER);
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
