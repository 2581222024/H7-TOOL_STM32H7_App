#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "lua_if.h"
#include "bsp.h"

static int lua_StartExtIO(lua_State* L);
static int lua_StopExtIO(lua_State* L);

static int lua_WriteDO(lua_State* L);
static int lua_ReadDI(lua_State* L);

static int lua_WriteDAC(lua_State* L);
static int lua_ReadADC(lua_State* L);

/*
*********************************************************************************************************
*	�� �� ��: lua_extio_RegisterFun
*	����˵��: ע��lua C���Խӿں���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void lua_extio_RegisterFun(void)
{
	//��ָ���ĺ���ע��ΪLua��ȫ�ֺ������������е�һ���ַ�������ΪLua����
    //�ڵ���C����ʱʹ�õ�ȫ�ֺ��������ڶ�������Ϊʵ��C������ָ�롣
	lua_register(g_Lua, "ex_start", lua_StartExtIO);
	lua_register(g_Lua, "ex_stop", lua_StopExtIO);
    lua_register(g_Lua, "ex_dout", lua_WriteDO);	
	lua_register(g_Lua, "ex_din", lua_ReadDI);
	lua_register(g_Lua, "ex_dac", lua_WriteDAC);
	lua_register(g_Lua, "ex_adc", lua_ReadADC);
}

			
/*
*********************************************************************************************************
*	�� �� ��: lua_StartExtIO
*	����˵��: ������չIO����
*	��    ��: 0-23
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int lua_StartExtIO(lua_State* L)
{
	EXIO_Start();
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: lua_StopExtIO
*	����˵��: ֹͣ��չIO����
*	��    ��: 0-23
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int lua_StopExtIO(lua_State* L)
{
	EXIO_Stop();
	return 1;	
}

/*
*********************************************************************************************************
*	�� �� ��: lua_WriteDO
*	����˵��: дDO
*	��    ��: 0-23
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int lua_WriteDO(lua_State* L)
{
	uint8_t pin;
	uint8_t value;

	if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
	{
		pin = luaL_checknumber(L, 1);	/* dac */
	}

	if (lua_type(L, 2) == LUA_TNUMBER) /* �жϵ�2������ */
	{
		value = luaL_checknumber(L, 2);	/* dac */
	}
	
	EX595_WritePin(pin, value);
	
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: lua_ReadDI
*	����˵��: ��DI
*	��    ��: 0-15
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int lua_ReadDI(lua_State* L)
{
	uint8_t pin;
	uint8_t value;

	if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
	{
		pin = luaL_checknumber(L, 1);	/* dac */
	}

	value = EX165_GetPin(pin);	
	
	lua_pushnumber(L, value);	/* �ɹ�,�������� */
	
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: lua_WriteDAC
*	����˵��: дDAC
*	��    ��: 0-1
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int lua_WriteDAC(lua_State* L)
{
	uint8_t ch;
	uint16_t dac;

	if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
	{
		ch = luaL_checknumber(L, 1);	/* dac */
	}

	if (lua_type(L, 2) == LUA_TNUMBER) /* �жϵ�2������ */
	{
		dac = luaL_checknumber(L, 2);	/* dac */
	}
	
	DAC8562_SetDacData(ch, dac);
	
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: lua_ReadADC
*	����˵��: ��ADC
*	��    ��: 0-7
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int lua_ReadADC(lua_State* L)
{
	uint8_t ch;
	int16_t adc;

	if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
	{
		ch = luaL_checknumber(L, 1);	/* dac */
	}

	adc = AD7606_ReadAdc(ch);	/* dac */

	lua_pushnumber(L, adc);	/* �ɹ�,�������� */
	
	return 1;
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
