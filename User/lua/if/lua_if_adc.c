#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "lua_if.h"
#include "bsp.h"
#include "param.h"

/* Ϊ�˱����DAP�����еĺ�����������ģ�麯����ǰ׺�� h7swd */

static int lua_ReadAdc(lua_State* L);
static int lua_ReadAnalog(lua_State* L);

/*
*********************************************************************************************************
*	�� �� ��: lua_adc_RegisterFun
*	����˵��: ע��lua C���Խӿں���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void lua_adc_RegisterFun(void)
{
	//��ָ���ĺ���ע��ΪLua��ȫ�ֺ������������е�һ���ַ�������ΪLua����
    //�ڵ���C����ʱʹ�õ�ȫ�ֺ��������ڶ�������Ϊʵ��C������ָ�롣
    lua_register(g_Lua, "read_adc", lua_ReadAdc);	
	lua_register(g_Lua, "read_analog", lua_ReadAnalog);
}
			
/*
*********************************************************************************************************
*	�� �� ��: lua_ReadAdc
*	����˵��: ��ADCֵ
*	��    ��: ͨ���� 0-8
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int lua_ReadAdc(lua_State* L)
{
	uint8_t ch;
	float re;

	if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
	{
		ch = luaL_checknumber(L, 1);	/* ģ��ͨ�� */
	}

	if (ch == AN_CH1)      				re = g_tVar.ADC_CH1Volt;		/* CH1��ѹ */
	else if (ch == AN_CH2) 				re = g_tVar.ADC_CH2Volt;		/* CH2��ѹ */
	else if (ch == AN_HIGH_SIDE_VOLT) 	re = g_tVar.ADC_HighSideVolt;
	else if (ch == AN_HIGH_SIDE_CURR) 	re = g_tVar.ADC_HighSideCurr;
	else if (ch == AN_TVCC_VOLT) 		re = g_tVar.ADC_TVCCVolt;		/* TVCCʵ���ѹ */
	else if (ch == AN_TVCC_CURR) 		re = g_tVar.ADC_TVCCCurr;		/* TVCCʵ���ѹ */
	else if (ch == AN_NTC_RES) 			re = g_tVar.ADC_NTCRes;			/* NTC���� */
	else if (ch == AN_12V_VOLT) 		re = g_tVar.ADC_ExtPowerVolt;
	else if (ch == AN_USB_VOLT) 		re = g_tVar.ADC_USBPowerVolt;	
	else re = 0;
	
	lua_pushnumber(L, re);	/* �ɹ�,�������� */
	
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: lua_ReadAdc
*	����˵��: ��У׼���ģ����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int lua_ReadAnalog(lua_State* L)
{
	uint8_t ch;
	float re;

	if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
	{
		ch = luaL_checknumber(L, 1);	/* ģ��ͨ�� */
	}

	if (ch == AN_CH1)      				re = g_tVar.CH1Volt;		/* CH1��ѹ */
	else if (ch == AN_CH2) 				re = g_tVar.CH2Volt;		/* CH2��ѹ */
	else if (ch == AN_HIGH_SIDE_VOLT) 	re = g_tVar.HighSideVolt;
	else if (ch == AN_HIGH_SIDE_CURR) 	re = g_tVar.HighSideCurr;
	else if (ch == AN_TVCC_VOLT) 		re = g_tVar.TVCCVolt;		/* TVCCʵ���ѹ */
	else if (ch == AN_TVCC_CURR) 		re = g_tVar.TVCCCurr;		/* TVCCʵ���ѹ */
	else if (ch == AN_NTC_RES) 			re = g_tVar.NTCRes;			/* NTC���� */
	else if (ch == AN_12V_VOLT) 		re = g_tVar.ExtPowerVolt;
	else if (ch == AN_USB_VOLT) 		re = g_tVar.USBPowerVolt;	
	else re = 0;
	
	lua_pushnumber(L, re);	/* �ɹ�,�������� */
	
	return 1;
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
