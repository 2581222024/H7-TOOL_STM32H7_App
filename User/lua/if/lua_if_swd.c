#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "lua_if.h"
#include "bsp.h"

#include "target_reset.h"
#include "target_config.h"
#include "swd_host.h"
#include "Systick_Handler.h"

/* Ϊ�˱����DAP�����еĺ�����������ģ�麯����ǰ׺�� h7swd */

static int h7swd_Init(lua_State* L);
static int h7swd_ReadID(lua_State* L);
static int h7swd_WriteMemory(lua_State* L);
static int h7swd_ReadMemory(lua_State* L);

/*
*********************************************************************************************************
*    �� �� ��: lua_swd_RegisterFun
*    ����˵��: ע��lua C���Խӿں���
*    ��    ��: ��
*    �� �� ֵ: ��
*********************************************************************************************************
*/
void lua_swd_RegisterFun(void)
{
    //��ָ���ĺ���ע��ΪLua��ȫ�ֺ������������е�һ���ַ�������ΪLua����
    //�ڵ���C����ʱʹ�õ�ȫ�ֺ��������ڶ�������Ϊʵ��C������ָ�롣
    lua_register(g_Lua, "swd_init", h7swd_Init);    
    lua_register(g_Lua, "swd_getid", h7swd_ReadID);
    lua_register(g_Lua, "swd_write", h7swd_WriteMemory);    
    lua_register(g_Lua, "swd_read", h7swd_ReadMemory);
}
            
/*
*********************************************************************************************************
*    �� �� ��: h7swd_Init
*    ����˵��: ��оƬID
*    ��    ��: vcc : CPU�����ѹ
*    �� �� ֵ: ��
*********************************************************************************************************
*/
static int h7swd_Init(lua_State* L)
{
    float vcc;
        
    sysTickInit();    /* ����DAP�����еĳ�ʼ������,ȫ�ֱ�����ʼ�� */

    if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
    {
        vcc = luaL_checknumber(L, 1);    /* VCC��ѹ�����㣬��λV */
    }
    
    bsp_SetTVCC(vcc * 1000);    /* ���ýӿڵ�ƽ3.3V */
    bsp_DelayUS(100 * 100);        /* �ӳ�100ms */
        
    swd_init_debug();            /* ����swd debug״̬ */
        
    return 1;
}

/*
*********************************************************************************************************
*    �� �� ��: h7swd_ReadID
*    ����˵��: ��оƬID
*    ��    ��: ��
*    �� �� ֵ: ��
*********************************************************************************************************
*/
static int h7swd_ReadID(lua_State* L)
{
    uint32_t id;

    if (swd_read_idcode(&id) == 0)
    {
        lua_pushnumber(L, 0);    /* ���� */
    }
    else
    {
        lua_pushnumber(L, id);    /* �ɹ�,����ID */
    }    
    return 1;
}

/*
*********************************************************************************************************
*    �� �� ��: h7swd_WriteMemory
*    ����˵��: дCPU�ڴ棨��Ĵ�����
*    ��    ��: addr : Ŀ���ַ
*                data : ���ݻ�������������
*    �� �� ֵ: 0 ʧ��   1 �ɹ�
*********************************************************************************************************
*/
static int h7swd_WriteMemory(lua_State* L)
{
    uint32_t addr = 0;
    const char *data;
    size_t len = 0;
    
    sysTickInit();    /* ����DAP�����еĳ�ʼ������,ȫ�ֱ�����ʼ�� */

    if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
    {
        addr = luaL_checknumber(L, 1);    /* Ŀ���ڴ��ַ */
    }
    else
    {
        lua_pushnumber(L, 0);    /* ���� */
        return 1;        
    }

    if (lua_type(L, 2) == LUA_TSTRING)     /* �жϵ�2������ */
    {        
        data = luaL_checklstring(L, 2, &len); /* 1�ǲ�����λ�ã� len��stri        �ĳ��� */        
    }
    
    if (len > 128 * 1024)
    {
        lua_pushnumber(L, 0);    /* ���� */
        return 1;
    }
    
    if (swd_write_memory(addr, (uint8_t *)data, len) == 0)
    {
        lua_pushnumber(L, 0);    /* ���� */
    }
    else
    {
        lua_pushnumber(L, 1);    /* �ɹ� */
    }
        
    return 1;
}

/*
*********************************************************************************************************
*    �� �� ��: h7swd_ReadMemory
*    ����˵��: ��CPU�ڴ棨��Ĵ�����
*    ��    ��: addr : Ŀ���ַ
*                data : ���ݻ�������������
*    �� �� ֵ: 0 ʧ��   1 �ɹ�
*********************************************************************************************************
*/
static int h7swd_ReadMemory(lua_State* L)
{
    uint32_t addr;
    uint32_t num;

    if (lua_type(L, 1) == LUA_TNUMBER)     /* �жϵ�1������ */
    {        
        addr = luaL_checknumber(L, 1); /* 1�ǲ�����λ�ã� len��stri              �ĳ��� */        
    }
    else
    {
        lua_pushnumber(L, 0);    /* ���� */
        return 1;
    }
    
    if (lua_type(L, 2) == LUA_TNUMBER) /* �жϵ�2������ */
    {
        num = luaL_checknumber(L, 2);
        
        memset(s_lua_read_buf, 0, num);        
    }
    
    if (num > LUA_READ_LEN_MAX)
    {
        lua_pushnumber(L, 0);    /* ���� */
        return 1;
    }

    if (swd_read_memory(addr, s_lua_read_buf, num) == 0)
    {
        lua_pushnumber(L, 0);    /* ���� */
    }
    else
    {
        lua_pushnumber(L, 1);    /* �ɹ� */
    }

    lua_pushlstring(L, (char *)s_lua_read_buf, num); 
    return 1;
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
