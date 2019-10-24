/*
*********************************************************************************************************
*
*	ģ������ : ��������
*	�ļ����� : status_link_mode.c
*	��    �� : V1.0
*	˵    �� : 
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
*	�� �� ��: status_LinkMode
*	����˵��: ����ģʽ����������λ�����ƣ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void status_LinkMode(void)
{
	uint8_t ucKeyCode;		/* �������� */
	uint8_t fRefresh;
	uint8_t fIgnoreKey = 0;

	DispHeader("����ģʽ");
	
	usbd_CloseCDC();
	usbd_OpenCDC(8);		/* ����USB���⴮��8�� ���ں�PC���USBͨ�� */
	
	fRefresh = 1;
	while (g_MainStatus == MS_LINK_MODE)
	{
		bsp_Idle();

		if (fRefresh)	/* ˢ���������� */
		{
			fRefresh = 0;

			{
				FONT_T tFont;		/* ��������ṹ����� */
				uint16_t x;
				uint16_t y;
				char buf[64];
				uint8_t line_cap = 20;
				
				tFont.FontCode = FC_ST_16;		/* ������� 16���� */
				tFont.FrontColor = CL_YELLOW;	/* ������ɫ */
				tFont.BackColor = FORM_BACK_COLOR;	/* ���ֱ�����ɫ */
				tFont.Space = 0;				/* ���ּ�࣬��λ = ���� */

				x = 5;
				y = 2 * line_cap;
				sprintf(buf, "��̫��MAC:%02X-%02X-%02X-%02X-%02X-%02X",
					g_tVar.MACaddr[0], g_tVar.MACaddr[1], g_tVar.MACaddr[2],
					g_tVar.MACaddr[3], g_tVar.MACaddr[4], g_tVar.MACaddr[5]);
				LCD_DispStr(x, y, buf, &tFont);

				y = y + line_cap;
				sprintf(buf, "IP:%d.%d.%d.%d", g_tParam.LocalIPAddr[0], g_tParam.LocalIPAddr[1],
					g_tParam.LocalIPAddr[2], g_tParam.LocalIPAddr[3]);					
				LCD_DispStr(x, y, buf, &tFont);	
				
				y = y + line_cap;
				sprintf(buf, "TCP�˿ں�: %d", g_tParam.LocalTCPPort);					
				LCD_DispStr(x, y, buf, &tFont);	
				
				y = y + line_cap;
				sprintf(buf, "UDP�˿ں�: %d", g_tParam.LocalTCPPort);					
				LCD_DispStr(x, y, buf, &tFont);						
					
				tFont.FrontColor = CL_BLACK;	/* ���� */
				y = 10 * line_cap;
				sprintf(buf, "����S����ϵͳ����", &tFont);
				
				LCD_DispStr(x, y, buf, &tFont);	
				y = 11 * line_cap;
				sprintf(buf, "����C�л�����", &tFont);
				LCD_DispStr(x, y, buf, &tFont);					
			}
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
					g_MainStatus = NextStatus(MS_LINK_MODE);
					break;

				case  KEY_LONG_S:		/* S������ */
					BEEP_KeyTone();	
					g_MainStatus = MS_SYSTEM_SET;
					break;								

				case  KEY_DOWN_C:		/* C������ */
					break;

				case  KEY_UP_C:			/* C���ͷ� */
					if (fIgnoreKey == 1)
					{
						fIgnoreKey = 0;
						break;
					}
					g_MainStatus = LastStatus(MS_LINK_MODE);
					break;

				case  KEY_LONG_C:		/* C�� */
					BEEP_KeyTone();	
					if (++g_tParam.DispDir > 3)
					{
						g_tParam.DispDir = 0;
					}
					LCD_SetDirection(g_tParam.DispDir);
					SaveParam();
					DispHeader("����ģʽ");
					fRefresh = 1;
					fIgnoreKey = 1;	/* ��Ҫ����J���������ĵ��𰴼� */
					break;	
					
				default:
					break;
			}
		}
	}

	usbd_CloseCDC();	
	usbd_OpenCDC(1);		/* ����USB���⴮��1�� �������⴮�ڣ�RS232 RS485 TTL-UART */
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
