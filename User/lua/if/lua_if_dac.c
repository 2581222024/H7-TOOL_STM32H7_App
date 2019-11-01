#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "lua_if.h"
#include "bsp.h"
#include "param.h"
#include "modbus_reg_addr.h"
#include "modbus_register.h"

/* Ϊ�˱����DAP�����еĺ�����������ģ�麯����ǰ׺�� h7swd */

static int lua_WriteDac(lua_State* L);
static int lua_WriteVolt(lua_State* L);
static int lua_WriteCurr(lua_State* L);
static int lua_WriteTvccDac(lua_State* L);
static int lua_WriteTvccVolt(lua_State* L);
static int lua_PowerOnDac(lua_State* L);
static int lua_PowerOffDac(lua_State* L);

/*
*********************************************************************************************************
*    �� �� ��: lua_adc_RegisterFun
*    ����˵��: ע��lua C���Խӿں���
*    ��    ��: ��
*    �� �� ֵ: ��
*********************************************************************************************************
*/
void lua_dac_RegisterFun(void)
{
    //��ָ���ĺ���ע��ΪLua��ȫ�ֺ������������е�һ���ַ�������ΪLua����
    //�ڵ���C����ʱʹ�õ�ȫ�ֺ��������ڶ�������Ϊʵ��C������ָ�롣
    lua_register(g_Lua, "dac_on", lua_PowerOnDac);    
    lua_register(g_Lua, "dac_off", lua_PowerOffDac);
    lua_register(g_Lua, "dac_write", lua_WriteDac);    
    lua_register(g_Lua, "dac_volt", lua_WriteVolt);
    lua_register(g_Lua, "dac_curr", lua_WriteCurr);
    lua_register(g_Lua, "write_tvcc_dac", lua_WriteTvccDac);
    lua_register(g_Lua, "write_tvcc_volt", lua_WriteTvccVolt);
}
        
/*
*********************************************************************************************************
*    �� �� ��: lua_PowerOnDac
*    ����˵��: ��DAC��Դ
*    ��    ��: ��
*    �� �� ֵ: ��
*********************************************************************************************************
*/
static int lua_PowerOnDac(lua_State* L)
{    
    WriteRegValue_06H(REG03_DAC_WAVE_TYPE, 0);    /* ֱ�� */
    WriteRegValue_06H(REG03_DAC_WAVE_START, 1);
    return 1;
}

/*
*********************************************************************************************************
*    �� �� ��: lua_PowerOffDac
*    ����˵��: �ر�DAC��Դ
*    ��    ��: ��
*    �� �� ֵ: ��
*********************************************************************************************************
*/
static int lua_PowerOffDac(lua_State* L)
{    
    WriteRegValue_06H(REG03_DAC_WAVE_TYPE, 0);    /* ֱ�� */
    WriteRegValue_06H(REG03_DAC_WAVE_START, 0);
    return 1;
}

/*
*********************************************************************************************************
*    �� �� ��: lua_WriteDac
*    ����˵��: дDAC
*    ��    ��: dacֵ 0-4095
*    �� �� ֵ: ��
*********************************************************************************************************
*/
static int lua_WriteDac(lua_State* L)
{
    uint16_t value;

    if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
    {
        value = luaL_checknumber(L, 1);    /* dac */
    }
    
    WriteRegValue_06H(REG03_OUT_VOLT_DAC, value);
    
    return 1;
}

/*
*********************************************************************************************************
*    �� �� ��: lua_WriteVolt
*    ����˵��: д��ѹֵ
*    ��    ��: ��ѹmv
*    �� �� ֵ: ��
*********************************************************************************************************
*/
static int lua_WriteVolt(lua_State* L)
{
    int16_t value;

    if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
    {
        value = luaL_checknumber(L, 1);    /* ��ѹ */
    }
    
    WriteRegValue_06H(REG03_OUT_VOLT_MV, value);
    
    return 1;
}

/*
*********************************************************************************************************
*    �� �� ��: lua_WriteCurr
*    ����˵��: д����ֵ
*    ��    ��: ����ֵuA
*    �� �� ֵ: ��
*********************************************************************************************************
*/
static int lua_WriteCurr(lua_State* L)
{
    uint16_t value;


    if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
    {
        value = luaL_checknumber(L, 1);    /* dac */
    }
    
    WriteRegValue_06H(REG03_OUT_CURR_UA, value);
    
    return 1;
}

/*
*********************************************************************************************************
*    �� �� ��: lua_WriteTvccDac
*    ����˵��: дTVCC DAC
*    ��    ��: dac 0-127
*    �� �� ֵ: ��
*********************************************************************************************************
*/
static int lua_WriteTvccDac(lua_State* L)
{
    uint16_t value;

    if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
    {
        value = luaL_checknumber(L, 1);    /* dac */
    }
    
    WriteRegValue_06H(REG03_OUT_TVCC_DAC, value);
    
    return 1;
}

/*
*********************************************************************************************************
*    �� �� ��: lua_WriteTvccVolt
*    ����˵��: дTVCC ��ѹ
*    ��    ��: dac 1200-5000
*    �� �� ֵ: ��
*********************************************************************************************************
*/
static int lua_WriteTvccVolt(lua_State* L)
{
    uint16_t value;

    if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
    {
        value = luaL_checknumber(L, 1);    /* dac */
    }
    
    WriteRegValue_06H(REG03_OUT_TVCC_MV, value);
    
    return 1;
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
