/*
*********************************************************************************************************
*
*	ģ������ : ������ѡ��
*	�ļ����� : status_MenuMain.c
*	��    �� : V1.0
*	˵    �� : 
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2018-12-06 armfly  ��ʽ����
*
*	Copyright (C), 2018-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"
#include "param.h"
#include "main.h"
#include "images.h"		/* ͼ���ļ� */
#include "status_menu.h"
#include "modbus_reg_addr.h"

#define FORM_BACK_COLOR	CL_BLUE

#define ICON_HEIGHT	48				/* ����ͼ��߶� */
#define ICON_WIDTH	48				/* ����ͼ���� */

#define ICON_STEP_X 80		/* ͼ��֮��ļ�� */
#define ICON_STEP_Y	70		/* ͼ��֮��ļ�� */

#define MENU_COUNT	5
static const ICON_T s_tMainIcons[MENU_COUNT] =
{
	{ID_ICON, 40, 20, ICON_HEIGHT, ICON_WIDTH, (uint16_t *)acchujiao, "1.����ģʽ"},
	{ID_ICON, 40, 20, ICON_HEIGHT, ICON_WIDTH, (uint16_t *)acLCD, "2.��ѹ��"},
	{ID_ICON, 40, 20, ICON_HEIGHT, ICON_WIDTH, (uint16_t *)acRadio, "3.NTC����"},
	{ID_ICON, 40, 20, ICON_HEIGHT, ICON_WIDTH, (uint16_t *)acRecorder, "4.�����"},
	{ID_ICON, 40, 20, ICON_HEIGHT, ICON_WIDTH, (uint16_t *)acchujiao, "5.���ص���"},
};

/*
*********************************************************************************************************
*	�� �� ��: status_MenuMain
*	����˵��: ���ܲ˵�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void status_MenuMain(void)
{
	uint8_t ucKeyCode;		/* �������� */
	uint8_t fRefresh;
	FONT_T tFont;		/* ��������ṹ����� */
	FONT_T tIconFont;		/* ����һ������ṹ�����������ͼ���ı� */
	static uint8_t s_menu_idx = 0;
	uint8_t ucIgnoreKey = 0;	
	
	/* ����������� */
	{
		uint16_t x;
		uint16_t y;
		char buf[64];
		
		tFont.FontCode = FC_ST_16;		/* ������� 16���� */
		tFont.FrontColor = CL_YELLOW;	/* ������ɫ */
		tFont.BackColor = FORM_BACK_COLOR;	/* ���ֱ�����ɫ */
		tFont.Space = 0;				/* ���ּ�࣬��λ = ���� */

		LCD_ClrScr(FORM_BACK_COLOR);  	/* ������������ɫ */

		x = 5;
		
		y = 3;
		LCD_DispStr(x, y, "ѡ����", &tFont);
		
		y = y + 20;
		sprintf(buf, "MAC:%02X-%02X-%02X-%02X-%02X-%02X",
			g_tVar.MACaddr[0], g_tVar.MACaddr[1], g_tVar.MACaddr[2],
			g_tVar.MACaddr[3], g_tVar.MACaddr[4], g_tVar.MACaddr[5]);
		LCD_DispStr(x, y, buf, &tFont);		

		y = y + 20;
		sprintf(buf, "IP:%d.%d.%d.%d",
			g_tVar.MACaddr[0], g_tVar.MACaddr[1], g_tVar.MACaddr[2],
			g_tVar.MACaddr[3], g_tVar.MACaddr[4], g_tVar.MACaddr[5]);
		LCD_DispStr(x, y, buf, &tFont);		
		
		LCD_SetBackLight(BRIGHT_DEFAULT);	 /* �򿪱��⣬����Ϊȱʡ���� */
		
		/* ����������� */
		{
			tIconFont.FontCode = FC_ST_16;			/* ������� 16���� */
			tIconFont.FrontColor = CL_WHITE;	/* ������ɫ */
			tIconFont.BackColor = CL_BLUE;			/* ���ֱ�����ɫ */
			tIconFont.Space = 1;					/* ���ּ�࣬��λ = ���� */
		}		
	}
	
	bsp_ClearKey();		/* �建��İ��� */

	/* ���������淵�غ���Ҫ���Ե�1��C�������¼� */
	if (bsp_GetKeyState(KID_S) || bsp_GetKeyState(KID_C))
	{
		ucIgnoreKey = 1;
	}
	
	fRefresh = 1;
	while (g_MainStatus == MS_MAIN_MENU)
	{
		bsp_Idle();

		if (fRefresh)	/* ˢ���������� */
		{
			fRefresh = 0;
			LCD_DrawIcon32(&s_tMainIcons[s_menu_idx], &tIconFont, 0);			
		}

		ucKeyCode = bsp_GetKey();	/* ��ȡ��ֵ, �޼�����ʱ���� KEY_NONE = 0 */
		if (ucKeyCode != KEY_NONE)
		{			
			/* �м����� */
			switch (ucKeyCode)
			{
				case  KEY_UP_S:		/* S�� */
					BEEP_KeyTone();	
					LCD_ClrScr(CL_BLUE);
					if (++s_menu_idx >= MENU_COUNT)
					{
						s_menu_idx = 0;		
					}
					fRefresh = 1;
					break;

				case  KEY_UP_C:		/* C�� */
					if (ucIgnoreKey == 1)
					{
						ucIgnoreKey = 0;	/* ������1�����������¼� */
						break;
					}					
					BEEP_KeyTone();	
					LCD_ClrScr(CL_BLUE);
					if (s_menu_idx > 0)
					{
						s_menu_idx--;
					}
					else
					{
						s_menu_idx = MENU_COUNT - 1;
					}						
					fRefresh = 1;
					break;					

				case  KEY_LONG_S:		/* S�� */
					BEEP_KeyTone();	
					g_MainStatus = MS_SYSTEM_SET;
					break;

				case  KEY_LONG_C:		/* C�� */
					BEEP_KeyTone();	
					if (++g_tParam.DispDir > 3)
					{
						g_tParam.DispDir = 0;
					}
					LCD_SetDirection(g_tParam.DispDir);
					SaveParam();
					LCD_ClrScr(CL_BLUE);
					fRefresh = 1;
					ucIgnoreKey = 1;					
					break;				
				
				default:
					break;
			}
		}
	}
}

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
	FONT_T tFont;		/* ��������ṹ����� */
	FONT_T tIconFont;		/* ����һ������ṹ�����������ͼ���ı� */
	static uint8_t s_menu_idx = 0;
	uint8_t ucIgnoreKey = 0;	
	
	/* ����������� */
	{
		tFont.FontCode = FC_ST_24;	/* ������� 16���� */
		tFont.FrontColor = CL_WHITE;	/* ������ɫ */
		tFont.BackColor = FORM_BACK_COLOR;	/* ���ֱ�����ɫ */
		tFont.Space = 0;				/* ���ּ�࣬��λ = ���� */

		LCD_ClrScr(FORM_BACK_COLOR);  	/* ������������ɫ */

		LCD_DispStr(5, 3, "ֱ����ѹ��", &tFont);	
	}
	
	bsp_ClearKey();		/* �建��İ��� */

	/* ���������淵�غ���Ҫ���Ե�1��C�������¼� */
	if (bsp_GetKeyState(KID_S) || bsp_GetKeyState(KID_C))
	{
		ucIgnoreKey = 1;
	}
	
	/* ��ʼ��DSOӲ����· */
	{
		DSO_InitHard();
		DSO_SetDC(1,1);		/* CH1 ѡDC��� */
		DSO_SetDC(2,1);		/* CH2 ѡDC��� */
		DSO_SetGain(1, 0);	/* CH1 ����ѡ����С��������� */
		DSO_SetGain(2, 0);	/* CH2 ����ѡ����С��������� */
	}
	
	fRefresh = 1;
	bsp_StartAutoTimer(0, 100);		/* 100ms ˢ�� */
	while (g_MainStatus == MS_VOLT_METER)
	{
		bsp_Idle();

		if (fRefresh)	/* ˢ���������� */
		{
			fRefresh = 0;
			;		
		}

		ucKeyCode = bsp_GetKey();	/* ��ȡ��ֵ, �޼�����ʱ���� KEY_NONE = 0 */
		if (ucKeyCode != KEY_NONE)
		{			
			/* �м����� */
			switch (ucKeyCode)
			{
				case  KEY_UP_S:		/* S�� */
					BEEP_KeyTone();	
					LCD_ClrScr(CL_BLUE);
					fRefresh = 1;
					break;

				case  KEY_UP_C:		/* C�� */
					if (ucIgnoreKey == 1)
					{
						ucIgnoreKey = 0;	/* ������1�����������¼� */
						break;
					}					
					BEEP_KeyTone();	
					LCD_ClrScr(CL_BLUE);						
					fRefresh = 1;
					break;					

				case  KEY_LONG_S:		/* S�� */
					BEEP_KeyTone();	
					g_MainStatus = MS_SYSTEM_SET;
					break;

				case  KEY_LONG_C:		/* C�� */
					BEEP_KeyTone();	
					if (++g_tParam.DispDir > 3)
					{
						g_tParam.DispDir = 0;
					}
					LCD_SetDirection(g_tParam.DispDir);
					SaveParam();
					LCD_ClrScr(CL_BLUE);
					fRefresh = 1;
					ucIgnoreKey = 1;					
					break;				
				
				default:
					break;
			}
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: status_VoltMeter
*	����˵��: ��ѹ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void status_VoltMeter(void)
{
	uint8_t ucKeyCode;		/* �������� */
	uint8_t fRefresh;
	FONT_T tFont;		/* ��������ṹ����� */
	FONT_T tIconFont;		/* ����һ������ṹ�����������ͼ���ı� */
	static uint8_t s_menu_idx = 0;
	uint8_t ucIgnoreKey = 0;	
	
	/* ����������� */
	{
		tFont.FontCode = FC_ST_24;	/* ������� 16���� */
		tFont.FrontColor = CL_WHITE;	/* ������ɫ */
		tFont.BackColor = FORM_BACK_COLOR;	/* ���ֱ�����ɫ */
		tFont.Space = 0;				/* ���ּ�࣬��λ = ���� */

		LCD_ClrScr(FORM_BACK_COLOR);  	/* ������������ɫ */

		LCD_DispStr(5, 3, "ֱ����ѹ��", &tFont);	
	}
	
	bsp_ClearKey();		/* �建��İ��� */

	/* ���������淵�غ���Ҫ���Ե�1��C�������¼� */
	if (bsp_GetKeyState(KID_S) || bsp_GetKeyState(KID_C))
	{
		ucIgnoreKey = 1;
	}
	
	/* ��ʼ��DSOӲ����· */
	{
		DSO_InitHard();
		DSO_SetDC(1,1);		/* CH1 ѡDC��� */
		DSO_SetDC(2,1);		/* CH2 ѡDC��� */
		DSO_SetGain(1, 0);	/* CH1 ����ѡ����С��������� */
		DSO_SetGain(2, 0);	/* CH2 ����ѡ����С��������� */
	}
	
	fRefresh = 1;
	bsp_StartAutoTimer(0, 100);		/* 100ms ˢ�� */
	while (g_MainStatus == MS_VOLT_METER)
	{
		bsp_Idle();

		if (fRefresh)	/* ˢ���������� */
		{
			fRefresh = 0;
			;		
		}

		ucKeyCode = bsp_GetKey();	/* ��ȡ��ֵ, �޼�����ʱ���� KEY_NONE = 0 */
		if (ucKeyCode != KEY_NONE)
		{			
			/* �м����� */
			switch (ucKeyCode)
			{
				case  KEY_UP_S:		/* S�� */
					BEEP_KeyTone();	
					LCD_ClrScr(CL_BLUE);
					fRefresh = 1;
					break;

				case  KEY_UP_C:		/* C�� */
					if (ucIgnoreKey == 1)
					{
						ucIgnoreKey = 0;	/* ������1�����������¼� */
						break;
					}					
					BEEP_KeyTone();	
					LCD_ClrScr(CL_BLUE);						
					fRefresh = 1;
					break;					

				case  KEY_LONG_S:		/* S�� */
					BEEP_KeyTone();	
					g_MainStatus = MS_SYSTEM_SET;
					break;

				case  KEY_LONG_C:		/* C�� */
					BEEP_KeyTone();	
					if (++g_tParam.DispDir > 3)
					{
						g_tParam.DispDir = 0;
					}
					LCD_SetDirection(g_tParam.DispDir);
					SaveParam();
					LCD_ClrScr(CL_BLUE);
					fRefresh = 1;
					ucIgnoreKey = 1;					
					break;				
				
				default:
					break;
			}
		}
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
