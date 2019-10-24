#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "lua_if.h"
#include "bsp.h"
#include "param.h"
#include "modbus_reg_addr.h"
#include "modbus_register.h"

/* Ϊ�˱����DAP�����еĺ�����������ģ�麯����ǰ׺�� h7swd */
static int lua_WriteReg16(lua_State* L);
static int lua_WriteReg32(lua_State* L);
static int lua_WriteRegFloat(lua_State* L);

static int lua_ReadReg16(lua_State* L);
static int lua_ReadReg32(lua_State* L);
static int lua_ReadRegFloat(lua_State* L);
static int lua_SaveParam(lua_State* L);

/*
*********************************************************************************************************
*	�� �� ��: lua_reg_RegisterFun
*	����˵��: ע��lua C���Խӿں���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void lua_reg_RegisterFun(void)
{
	//��ָ���ĺ���ע��ΪLua��ȫ�ֺ������������е�һ���ַ�������ΪLua����
    //�ڵ���C����ʱʹ�õ�ȫ�ֺ��������ڶ�������Ϊʵ��C������ָ�롣
    lua_register(g_Lua, "write_reg16", lua_WriteReg16);	
	lua_register(g_Lua, "write_reg32", lua_WriteReg32);
	lua_register(g_Lua, "write_regfloat", lua_WriteRegFloat);

    lua_register(g_Lua, "read_reg16", lua_ReadReg16);	
	lua_register(g_Lua, "read_reg32", lua_ReadReg32);
	lua_register(g_Lua, "read_regfloat", lua_ReadRegFloat);	
	
	lua_register(g_Lua, "save_param", lua_SaveParam);
}

/*
*********************************************************************************************************
*	�� �� ��: lua_SaveParam
*	����˵��: д������������ˢ�µ�eeprom
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int lua_SaveParam(lua_State* L)
{
	SaveParam();
	
	if (g_tVar.CalibEnable == 1)
	{
		SaveCalibParam();
	}
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: lua_WriteReg16
*	����˵��: д�Ĵ��� 16bit
*	��    ��: Addr��ַ �� �Ĵ���ֵ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int lua_WriteReg16(lua_State* L)
{
	uint16_t addr;
	int16_t value;


	if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
	{
		addr = luaL_checknumber(L, 1);	/* �Ĵ�����ַ */
	}

	if (lua_type(L, 2) == LUA_TNUMBER) /* �жϵ�1������ */
	{
		value = luaL_checknumber(L, 2);	/* �Ĵ���ֵ */
	}	
	
	WriteRegValue_06H(addr, (uint16_t)value);
	
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: lua_ReadReg16
*	����˵��: ���Ĵ��� 16bit
*	��    ��: Addr��ַ
*	�� �� ֵ: �Ĵ���ֵ
*********************************************************************************************************
*/
static int lua_ReadReg16(lua_State* L)
{
	uint16_t addr;
	uint16_t value;

	if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
	{
		addr = luaL_checknumber(L, 1);	/* �Ĵ�����ַ */
	}
	
	if (ReadRegValue_03H(addr, &value) == 1)
	{
		lua_pushnumber(L, value);	/* �ɹ�,�������� */
	}
	else
	{
		lua_pushnumber(L, 0);	/* �ɹ�,�������� */
	}
	
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: lua_WriteReg32
*	����˵��: д�Ĵ��� 32Bit����
*	��    ��: Addr��ַ �� �Ĵ���ֵ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int lua_WriteReg32(lua_State* L)
{
	uint16_t addr;
	int32_t value;

	if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
	{
		addr = luaL_checknumber(L, 1);	/* �Ĵ�����ַ */
	}

	if (lua_type(L, 2) == LUA_TNUMBER) /* �жϵ�2������ */
	{
		value = luaL_checknumber(L, 2);	/* �Ĵ�����ַ */
	}	
	
	WriteRegValue_06H(addr, (uint32_t)value >> 16);
	WriteRegValue_06H(addr + 1, (uint32_t)value);
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: lua_ReadReg32
*	����˵��: ���Ĵ��� 32bit �з���
*	��    ��: Addr��ַ
*	�� �� ֵ: �Ĵ���ֵ
*********************************************************************************************************
*/
static int lua_ReadReg32(lua_State* L)
{
	uint16_t addr;
	uint16_t value1;
	uint16_t value2;
	int32_t value32;
	uint8_t re = 0;

	if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
	{
		addr = luaL_checknumber(L, 1);	/* �Ĵ�����ַ */
	}
	
	re = ReadRegValue_03H(addr, &value1);
	re += ReadRegValue_03H(addr + 1, &value2);
	
	if (re == 2)
	{
		value32 =(value1 << 8) + value2;
		lua_pushnumber(L, value32);	/* �ɹ�,�������� */
	}
	else
	{
		lua_pushnumber(L, 0);	/* �ɹ�,�������� */
	}
	
	return 1;
}


/*
*********************************************************************************************************
*	�� �� ��: lua_WriteRegFloat
*	����˵��: д�Ĵ��� 32Bit����
*	��    ��: Addr��ַ �� �Ĵ���ֵ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int lua_WriteRegFloat(lua_State* L)
{
	uint16_t addr;
	float ff;

	if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
	{
		addr = luaL_checknumber(L, 1);	/* �Ĵ�����ַ */
	}

	if (lua_type(L, 2) == LUA_TNUMBER) /* �жϵ�2������ */
	{
		ff = luaL_checknumber(L, 2);	/* �Ĵ�����ַ */
	}	
	
	WriteRegValue_06H(addr, GetHigh16OfFloat(ff));
	WriteRegValue_06H(addr + 1, GetLow16OfFloat(ff));
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: lua_ReadRegFloat
*	����˵��: ���Ĵ��� ����
*	��    ��: Addr��ַ
*	�� �� ֵ: �Ĵ���ֵ
*********************************************************************************************************
*/
static int lua_ReadRegFloat(lua_State* L)
{
	uint16_t addr;
	uint16_t value1;
	uint16_t value2;
	float ff;
	uint8_t re = 0;
	uint8_t buf[4];

	if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
	{
		addr = luaL_checknumber(L, 1);	/* �Ĵ�����ַ */
	}
	
	re = ReadRegValue_03H(addr, &value1);
	re += ReadRegValue_03H(addr + 1, &value2);
	
	if (re == 2)
	{
		buf[0] = value1 >> 8;
		buf[1] = value1;
		buf[2] = value2 >> 8;
		buf[3] = value2;
		
		ff = BEBufToFloat(buf);
		
		lua_pushnumber(L, ff);	/* �ɹ�,�������� */
	}
	else
	{
		lua_pushnumber(L, 0);	/* �ɹ�,�������� */
	}
	
	return 1;
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
