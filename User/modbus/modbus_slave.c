/*
*********************************************************************************************************
*
*	ģ������ : MODBUS�ӻ�ģ��
*	�ļ����� : modbus_slave.c
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2014-2015, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "main.h"
#include "param.h"
#include "modbus_slave.h"
#include "modbus_reg_addr.h"
#include "modbus_register.h"
#include "modbus_iap.h"
#include "lua_if.h"

static void MODS_AnalyzeApp(void);

//static void MODS_RxTimeOut(void);

static void MODS_01H(void);
static void MODS_02H(void);
static void MODS_03H(void);
static void MODS_04H(void);
static void MODS_05H(void);
static void MODS_06H(void);
static void MODS_10H(void);
static void MODS_0FH(void);
static void MODS_64H(void);
static void MODS_65H(void);
static void MODS_66H(void);
static void MODS_67H(void);
static void MODS_68H(void);

static void MODS_60H(void);		/* PC�����ָ���ȡ�������� */

void MODS_ReciveNew(uint8_t _byte);

MODS_T g_tModS;
MOD_WAVE_T g_tModWave;

/*
*********************************************************************************************************
*	�� �� ��: MODS_Poll
*	����˵��: �������ݰ�. �����������������á�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t MODS_Poll(uint8_t *_buf, uint16_t _len)
{
	uint16_t addr;
	uint16_t crc1;

	g_tModS.RxBuf = _buf;
	g_tModS.RxCount = _len;
	
	g_tModS.TxCount = 0;
	//*_AckBuf = g_tModS.TxBuf;

	if (g_tModS.RxCount < 4)
	{
		goto err_ret;
	}

	if (g_tModS.TCP_Flag == 0)
	{
		/* ����CRCУ��� */
		crc1 = CRC16_Modbus(g_tModS.RxBuf, g_tModS.RxCount);
		if (crc1 != 0)
		{
			MODS_SendAckErr(ERR_PACKAGE);		/* ��������Ӧ�� */
			goto err_ret;
		}
	}
	else
	{
		g_tModS.RxCount += 2;
	}

	/* վ��ַ (1�ֽڣ� */
	addr = g_tModS.RxBuf[0];	/* ��1�ֽ� վ�� */
	if (addr != g_tParam.Addr485 && addr != 0xF4)
	{
		goto err_ret;
	}

	/* ����Ӧ�ò�Э�� */
	MODS_AnalyzeApp();
	g_tModS.RxCount = 0;	/* ��������������������´�֡ͬ�� */
	return 1;
	
err_ret:
	g_tModS.RxCount = 0;	/* ��������������������´�֡ͬ�� */
	return 0;
}

#if 0
/*
*********************************************************************************************************
*	�� �� ��: MODS_ReciveNew
*	����˵��: ���ڽ����жϷ���������ñ����������յ�һ���ֽ�ʱ��ִ��һ�α�������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
//void MODS_ReciveNew(uint8_t _byte)
//{
//	/*
//		3.5���ַ���ʱ������ֻ������RTUģʽ���棬��ΪRTUģʽû�п�ʼ���ͽ�������
//		�������ݰ�֮��ֻ�ܿ�ʱ���������֣�Modbus�����ڲ�ͬ�Ĳ������£����ʱ���ǲ�һ���ģ�
//		���Ծ���3.5���ַ���ʱ�䣬�����ʸߣ����ʱ������С�������ʵͣ����ʱ������Ӧ�ʹ�

//		4800  = 7.297ms
//		9600  = 3.646ms
//		19200  = 1.771ms
//		38400  = 0.885ms
//	*/
//	uint32_t timeout;
//	
//	g_rtu_timeout = 0;
//	
//	timeout = 35000000 / g_tParam.Baud;		/* ���㳬ʱʱ�䣬��λus */
//	
//	/* Ӳ����ʱ�жϣ���ʱ����us Ӳ����ʱ��1����ADC, ��ʱ��2����Modbus */
//	bsp_StartHardTimer(2, timeout, (void *)MODS_RxTimeOut);

//	if (g_tModS.RxCount < RX_BUF_SIZE)
//	{
//		g_tModS.RxBuf[g_tModS.RxCount++] = _byte;
//	}
//}

///*
//*********************************************************************************************************
//*	�� �� ��: MODS_RxTimeOut
//*	����˵��: ����3.5���ַ�ʱ���ִ�б������� ����ȫ�ֱ��� g_rtu_timeout = 1; ֪ͨ������ʼ���롣
//*	��    ��: ��
//*	�� �� ֵ: ��
//*********************************************************************************************************
//*/
//static void MODS_RxTimeOut(void)
//{
//	g_rtu_timeout = 1;
//}
#endif


/*
*********************************************************************************************************
*	�� �� ��: MODS_SendWithCRC
*	����˵��: ����һ������, �Զ�׷��2�ֽ�CRC��������ȫ�ֱ���: g_tModS.TxBuf, g_tModS.TxCount
*	��    ��: ��
*			  _ucLen ���ݳ��ȣ�����CRC��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MODS_SendWithCRC(void)
{
	uint16_t crc;

	crc = CRC16_Modbus(g_tModS.TxBuf, g_tModS.TxCount);
	g_tModS.TxBuf[g_tModS.TxCount++] = crc >> 8;
	g_tModS.TxBuf[g_tModS.TxCount++] = crc;
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_SendAckErr
*	����˵��: ���ʹ���Ӧ��
*	��    ��: _ucErrCode : �������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MODS_SendAckErr(uint8_t _ucErrCode)
{
	g_tModS.TxCount = 0;
	g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[0];		/* 485��ַ */
	g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[1] | 0x80;	/* �쳣�Ĺ����� */
	g_tModS.TxBuf[g_tModS.TxCount++] = _ucErrCode;				/* �������(01,02,03,04) */

	MODS_SendWithCRC();
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_SendAckOk
*	����˵��: ������ȷ��Ӧ��.
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MODS_SendAckOk(void)
{
	uint8_t i;

	g_tModS.TxCount = 0;
	for (i = 0; i < 6; i++)
	{
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[i];
	}
	MODS_SendWithCRC();
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_AnalyzeApp
*	����˵��: ����Ӧ�ò�Э��
*	��    ��:
*		     _DispBuf  �洢����������ʾ����ASCII�ַ�����0����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODS_AnalyzeApp(void)
{
	switch (g_tModS.RxBuf[1])			/* ��2���ֽ� ������ */
	{
		case 0x01:	/* ��ȡ��Ȧ״̬*/
			MODS_01H();
			break;

		case 0x02:	/* ��ȡ����״̬ */
			MODS_02H();
			break;
		
		case 0x03:	/* ��ȡ1�������������ּĴ��� ��һ���������ּĴ�����ȡ�õ�ǰ�Ķ�����ֵ*/
			MODS_03H();
			break;
		
		case 0x04:	/* ��ȡ1������ģ��������Ĵ��� */
			MODS_04H();
			break;
		
		case 0x05:	/* ǿ�Ƶ���Ȧ���� */
			MODS_05H();
			break;
		
		case 0x06:	/* д�����������ּĴ��� (�洢��CPU��FLASH�У���EEPROM�еĲ���)*/
			MODS_06H();			
			break;
			
		case 0x10:	/* д����������ּĴ��� (�洢��CPU��FLASH�У���EEPROM�еĲ���)*/
			MODS_10H();
			break;
		
		case 0x0F:
			MODS_0FH();	/* ǿ�ƶ����Ȧ����ӦD01/D02/D03�� */
			break;
		
//		case 0x15:	/* д�ļ��Ĵ��� */
//			MODS_15H();
//			break;
		
		case 0x60:		/* ��ȡ��������ר�ù����� */
			MODS_60H();
			break;
		
		case 0x64:	/* С�������ؽӿ� */
			MODS_64H();
			break;

		case 0x65:	/* ��ʱִ��С���� */
			MODS_65H();
			break;		
		
		default:
			g_tModS.RspCode = RSP_ERR_CMD;
			MODS_SendAckErr(g_tModS.RspCode);	/* ��������������� */
			break;
	}
}


/*
*********************************************************************************************************
*	�� �� ��: MODS_03H
*	����˵��: ��ȡ���ּĴ��� ��һ���������ּĴ�����ȡ�õ�ǰ�Ķ�����ֵ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODS_03H(void)
{
	uint16_t regaddr;
	uint16_t num;
	uint16_t value;
	uint16_t i;
	
	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ����ֵ����� */
		goto err_ret;
	}

	regaddr = BEBufToUint16(&g_tModS.RxBuf[2]); 
	num = BEBufToUint16(&g_tModS.RxBuf[4]);
	if (num > (TX_BUF_SIZE - 5) / 2)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ����ֵ����� */
		goto err_ret;		
	}

err_ret:
	if (g_tModS.RxBuf[0] != 0x00)	/* 00�㲥��ַ��Ӧ��, FF��ַӦ��g_tParam.Addr485 */
	{	
		if (g_tModS.RspCode == RSP_OK)			/* ��ȷӦ�� */
		{
			g_tModS.TxCount = 0;
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tParam.Addr485;
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[1];
			g_tModS.TxBuf[g_tModS.TxCount++] = num * 2;
			
			/* ���⴦�����ζ�ȡ ADC_BUFFER_SIZE */
			if (regaddr >= REG03_CH1_WAVE_0 && regaddr <= REG03_CH1_WAVE_END) 
			{
				uint16_t m;
				uint32_t idx;
				uint8_t *p;
				
				m = regaddr - REG03_CH1_WAVE_0;
				idx = m * DSO_PACKAGE_SIZE / 4;
				for (i = 0; i < num / 2; i++)
				{
					
					p = (uint8_t *)&g_Ch1WaveBuf[idx++];
					g_tModS.TxBuf[g_tModS.TxCount++] = p[3];
					g_tModS.TxBuf[g_tModS.TxCount++] = p[2];
					g_tModS.TxBuf[g_tModS.TxCount++] = p[1];
					g_tModS.TxBuf[g_tModS.TxCount++] = p[0];
				}
			}
			else if (regaddr >= REG03_CH2_WAVE_0 && regaddr <= REG03_CH2_WAVE_END)
			{
				uint16_t m;
				uint32_t idx;
				uint8_t *p;
				
				m = regaddr - REG03_CH2_WAVE_0;
				idx = m * DSO_PACKAGE_SIZE / 4;;
				for (i = 0; i < num / 2; i++)
				{
					p = (uint8_t *)&g_Ch2WaveBuf[idx++];
					g_tModS.TxBuf[g_tModS.TxCount++] = p[3];
					g_tModS.TxBuf[g_tModS.TxCount++] = p[2];
					g_tModS.TxBuf[g_tModS.TxCount++] = p[1];
					g_tModS.TxBuf[g_tModS.TxCount++] = p[0];
				}
			}			
			else
			{
				for (i = 0; i < num; i++)
				{
					if (ReadRegValue_03H(regaddr++, &value) == 0)
					{
						g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* �Ĵ�����ַ���� */
						goto err_ret;
					}
					g_tModS.TxBuf[g_tModS.TxCount++] = value >> 8;
					g_tModS.TxBuf[g_tModS.TxCount++] = value;
				}
			}
			
			MODS_SendWithCRC();
		
		}
		else
		{
			MODS_SendAckErr(g_tModS.RspCode);	/* ��������������� */
		}
	}
}


/*
*********************************************************************************************************
*	�� �� ��: MODS_04H
*	����˵��: ��һ������ģ�����Ĵ����� �ṹ��03Hָ����ͬ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODS_04H(void)
{
	uint16_t regaddr;
	uint16_t num;
	uint16_t value;
	uint8_t i;
	
	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ����ֵ����� */
		goto err_ret;
	}

	regaddr = BEBufToUint16(&g_tModS.RxBuf[2]); 
	num = BEBufToUint16(&g_tModS.RxBuf[4]);
	if (num > (TX_BUF_SIZE - 5) / 2)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ����ֵ����� */
		goto err_ret;		
	}

err_ret:
	if (g_tModS.RxBuf[0] != 0x00)	/* 00�㲥��ַ��Ӧ��, FF��ַӦ��g_tParam.Addr485 */
	{	
		if (g_tModS.RspCode == RSP_OK)			/* ��ȷӦ�� */
		{
			g_tModS.TxCount = 0;
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tParam.Addr485;
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[1];
			g_tModS.TxBuf[g_tModS.TxCount++] = num * 2;
			
			for (i = 0; i < num; i++)
			{
				if (ReadRegValue_04H(regaddr++, &value) == 0)
				{
					g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* �Ĵ�����ַ���� */
					goto err_ret;
				}
				g_tModS.TxBuf[g_tModS.TxCount++] = value >> 8;
				g_tModS.TxBuf[g_tModS.TxCount++] = value;
			}
			
			MODS_SendWithCRC();
		
		}
		else
		{
			MODS_SendAckErr(g_tModS.RspCode);	/* ��������������� */
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_06H
*	����˵��: д�����Ĵ���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODS_06H(void)
{

	/*
		д���ּĴ�����ע��06ָ��ֻ�ܲ����������ּĴ�����16ָ��������õ����������ּĴ���

		��������:
			11 �ӻ���ַ
			06 ������
			00 �Ĵ�����ַ���ֽ�
			01 �Ĵ�����ַ���ֽ�
			00 ����1���ֽ�
			01 ����1���ֽ�
			9A CRCУ����ֽ�
			9B CRCУ����ֽ�

		�ӻ���Ӧ:
			11 �ӻ���ַ
			06 ������
			00 �Ĵ�����ַ���ֽ�
			01 �Ĵ�����ַ���ֽ�
			00 ����1���ֽ�
			01 ����1���ֽ�
			1B CRCУ����ֽ�
			5A	CRCУ����ֽ�

		����:
			01 06 30 06 00 25  A710    ---- ������������Ϊ 2.5
			01 06 30 06 00 10  6707    ---- ������������Ϊ 1.0


			01 06 30 1B 00 00  F6CD    ---- SMA �˲�ϵ�� = 0 �ر��˲�
			01 06 30 1B 00 01  370D    ---- SMA �˲�ϵ�� = 1
			01 06 30 1B 00 02  770C    ---- SMA �˲�ϵ�� = 2
			01 06 30 1B 00 05  36CE    ---- SMA �˲�ϵ�� = 5

			01 06 30 07 00 01  F6CB    ---- ����ģʽ�޸�Ϊ T1
			01 06 30 07 00 02  B6CA    ---- ����ģʽ�޸�Ϊ T2

			01 06 31 00 00 00  8736    ---- ������ӿ��¼��
			01 06 31 01 00 00  D6F6    ---- �����澯��¼��

*/

	uint16_t reg;
	uint16_t value;
//	uint8_t i;
	
	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ����ֵ����� */
		goto err_ret;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* �Ĵ����� */
	value = BEBufToUint16(&g_tModS.RxBuf[4]);	/* �Ĵ���ֵ */

	fResetReq_06H = 0;	
	fSaveReq_06H = 0;		/* ��Ҫ������� */
	fSaveCalibParam = 0;
	if (WriteRegValue_06H(reg, value) == 0)
	{
		g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* �Ĵ�����ַ���� */
	}
	else
	{
		if (fSaveReq_06H == 1)		/* ��Ҫ�ȷ� */
		{
			fSaveReq_06H = 1;
			SaveParam();
		}
		
		if (fSaveCalibParam == 1)
		{
			fSaveCalibParam = 0;
			SaveCalibParam();		/* ����У׼���� */
		}

		if (fResetReq_06H == 1)
		{
			fResetReq_06H = 0;
			{
				/* ��λ����APP */
				*(uint32_t *)0x20000000 = 0;
				NVIC_SystemReset();	/* ��λCPU */	
			}
		}	
	}
	
err_ret:
	if (g_tModS.RspCode == RSP_OK)			/* ��ȷӦ�� */
	{
		MODS_SendAckOk();
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);	/* ��������������� */
	}
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_10H
*	����˵��: ����д����Ĵ���.  �����ڸ�дʱ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODS_10H(void)
{
	/*
		�ӻ���ַΪ11H�����ּĴ�������ʵ��ַΪ0001H���Ĵ����Ľ�����ַΪ0002H���ܹ�����2���Ĵ�����
		���ּĴ���0001H������Ϊ000AH�����ּĴ���0002H������Ϊ0102H��

		��������:
			11 �ӻ���ַ
			10 ������
			00 �Ĵ�����ʼ��ַ���ֽ�
			01 �Ĵ�����ʼ��ַ���ֽ�
			00 �Ĵ����������ֽ�
			02 �Ĵ����������ֽ�
			04 �ֽ���
			00 ����1���ֽ�
			0A ����1���ֽ�
			01 ����2���ֽ�
			02 ����2���ֽ�
			C6 CRCУ����ֽ�
			F0 CRCУ����ֽ�

		�ӻ���Ӧ:
			11 �ӻ���ַ
			06 ������
			00 �Ĵ�����ַ���ֽ�
			01 �Ĵ�����ַ���ֽ�
			00 ����1���ֽ�
			01 ����1���ֽ�
			1B CRCУ����ֽ�
			5A	CRCУ����ֽ�

		����:
			01 10 30 00 00 06 0C  07 DE  00 0A  00 01  00 08  00 0C  00 00     389A    ---- дʱ�� 2014-10-01 08:12:00
			01 10 30 00 00 06 0C  07 DF  00 01  00 1F  00 17  00 3B  00 39     5549    ---- дʱ�� 2015-01-31 23:59:57

	*/
	uint16_t reg_addr;
	uint16_t reg_num;
//	uint8_t byte_num;
	uint16_t value;
	uint8_t i;
	uint8_t *_pBuf;	
	
	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount < 11)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ����ֵ����� */
		goto err_ret;
	}

	fSaveReq_06H = 0;
	fSaveCalibParam = 0;
	fResetReq_06H = 0;
	
	reg_addr = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* �Ĵ����� */
	reg_num = BEBufToUint16(&g_tModS.RxBuf[4]);	/* �Ĵ������� */
//	byte_num = g_tModS.RxBuf[6];	/* ������������ֽ��� */
	_pBuf = &g_tModS.RxBuf[7];

	for (i = 0; i < reg_num; i++)
	{
		value = BEBufToUint16(_pBuf);
		
		if (WriteRegValue_06H(reg_addr + i, value) == 0)
		{
			g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* �Ĵ�����ַ���� */
			break;
		}
		
		_pBuf += 2;
	}
		
	if (fSaveReq_06H == 1)		/* ��Ҫ�ȷ� */
	{
		fSaveReq_06H = 1;
		SaveParam();
	}
	
	if (fSaveCalibParam == 1)
	{
		fSaveCalibParam = 0;
		SaveCalibParam();		/* ����У׼���� */
	}

	if (fResetReq_06H == 1)
	{
		fResetReq_06H = 0;
		{
			/* ��λ����APP */
			*(uint32_t *)0x20000000 = 0;
			NVIC_SystemReset();	/* ��λCPU */	
		}
	}	
		
err_ret:
	if (g_tModS.RspCode == RSP_OK)			/* ��ȷӦ�� */
	{
		MODS_SendAckOk();	
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);	/* ��������������� */
	}
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_01H
*	����˵��: ��ȡ��Ȧ״̬����ӦԶ�̿���D01/D02/D03��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODS_01H(void)
{
	/*
		��������:
			11 �ӻ���ַ
			01 ������
			00 �Ĵ�����ʼ��ַ���ֽ�
			13 �Ĵ�����ʼ��ַ���ֽ�
			00 �Ĵ����������ֽ�
			25 �Ĵ����������ֽ�
			0E CRCУ����ֽ�
			84 CRCУ����ֽ�

		�ӻ�Ӧ��: 	1����ON��0����OFF�������ص���Ȧ����Ϊ8�ı�����������������ֽ�δβʹ��0����. BIT0��Ӧ��1��
			11 �ӻ���ַ
			01 ������
			05 �����ֽ���
			CD ����1(��Ȧ0013H-��Ȧ001AH)
			6B ����2(��Ȧ001BH-��Ȧ0022H)
			B2 ����3(��Ȧ0023H-��Ȧ002AH)
			0E ����4(��Ȧ0032H-��Ȧ002BH)
			1B ����5(��Ȧ0037H-��Ȧ0033H)
			45 CRCУ����ֽ�
			E6 CRCУ����ֽ�

		����:
			01 01 10 01 00 03   29 0B	--- ��ѯD01��ʼ��3���̵���״̬
			01 01 10 03 00 01   09 0A   --- ��ѯD03�̵�����״̬
	*/
	uint16_t reg;
	uint16_t num;
	uint16_t i;
	uint16_t m;
	uint8_t status[MODS_DO_NUM / 8 + 1];

	g_tModS.RspCode = RSP_OK;

	/* û���ⲿ�̵�����ֱ��Ӧ����� */
	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;	/* ����ֵ����� */
		goto err_ret;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* �Ĵ����� */
	num = BEBufToUint16(&g_tModS.RxBuf[4]);	/* �Ĵ������� */

	if (num > MODS_DO_NUM)
	{
		g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* �Ĵ�����ַ���� */
		goto err_ret;
	}
	
	m = (num + 7) / 8;	
	if (m < sizeof(status))
	{	
		for (i = 0; i < m; i++)
		{
			status[i] = 0;
		}
		for (i = 0; i < num; i++)
		{					
			uint8_t value;
			
			if (MODS_GetDIState(i + reg, &value))
			{
				if (value == 1)
				{
					status[i / 8] |= (1 << (i % 8));
				}
			}	
			else
			{
				g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* �Ĵ�����ַ���� */
			}
		}
	}
	else
	{
		g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* �Ĵ�����ַ���� */
	}

err_ret:
	if (g_tModS.RxBuf[0] != 0x00)	/* 00�㲥��ַ��Ӧ��, FF��ַӦ��g_tParam.Addr485 */
	{
		if (g_tModS.RspCode == RSP_OK)		/* ��ȷӦ�� */
		{
			g_tModS.TxCount = 0;
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tParam.Addr485;
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[1];
			g_tModS.TxBuf[g_tModS.TxCount++] = m;			/* �����ֽ��� */

			for (i = 0; i < m; i++)
			{
				g_tModS.TxBuf[g_tModS.TxCount++] = status[i];	/* �̵���״̬ */
			}
			MODS_SendWithCRC();
		}
		else
		{
			MODS_SendAckErr(g_tModS.RspCode);	/* ��������������� */
		}
	}
}

static void MODS_02H(void)
{
	/*
		��������:
			11 �ӻ���ַ
			02 ������
			00 �Ĵ�����ַ���ֽ�
			C4 �Ĵ�����ַ���ֽ�
			00 �Ĵ����������ֽ�
			16 �Ĵ����������ֽ�
			BA CRCУ����ֽ�
			A9 CRCУ����ֽ�

		�ӻ�Ӧ��:  ��Ӧ����ɢ����Ĵ���״̬���ֱ��Ӧ�������е�ÿλֵ��1 ����ON��0 ����OFF��
		           ��һ�������ֽڵ�LSB(����ֽ�)Ϊ��ѯ��Ѱַ��ַ����������ڰ�˳���ڸ��ֽ����ɵ��ֽ�
		           ����ֽ����У�ֱ�������8λ����һ���ֽ��е�8������λҲ�Ǵӵ��ֽڵ����ֽ����С�
		           �����ص�����λ������8�ı������������������ֽ��е�ʣ��λ�����ֽڵ����λʹ��0��䡣
			11 �ӻ���ַ
			02 ������
			03 �����ֽ���
			AC ����1(00C4H-00CBH)
			DB ����2(00CCH-00D3H)
			35 ����3(00D4H-00D9H)
			20 CRCУ����ֽ�
			18 CRCУ����ֽ�

		����:
		01 02 20 01 00 08  23CC  ---- ��ȡT01-08��״̬
		01 02 20 04 00 02  B3CA  ---- ��ȡT04-05��״̬
		01 02 20 01 00 12  A207   ---- �� T01-18
	*/

	uint16_t reg;
	uint16_t num;
	uint16_t i;
	uint16_t m;
	uint8_t status[MODS_DI_NUM / 8 + 1];

	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;	/* ����ֵ����� */
		return;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* �Ĵ����� */
	num = BEBufToUint16(&g_tModS.RxBuf[4]);		/* �Ĵ������� */

	m = (num + 7) / 8;
	if (m > 0 && m < sizeof(status))
	{
		for (i = 0; i < m; i++)
		{
			status[i] = 0;
		}
		for (i = 0; i < num; i++)
		{
			uint8_t state;
			
			if (MODS_GetDIState(reg + i, &state))
			{
				if (state == 1)
				{
					status[i / 8] |= (1 << (i % 8));
				}
			}
			else
			{
				g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* �Ĵ�����ַ���� */
				break;
			}
		}
	}
	else
	{
		g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* �Ĵ�����ַ���� */
	}

	if (g_tModS.RspCode == RSP_OK)		/* ��ȷӦ�� */
	{
		g_tModS.TxCount = 0;
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[0];
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[1];
		g_tModS.TxBuf[g_tModS.TxCount++] = m;			/* �����ֽ��� */

		for (i = 0; i < m; i++)
		{
			g_tModS.TxBuf[g_tModS.TxCount++] = status[i];	/* T01-02״̬ */
		}
		MODS_SendWithCRC();
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);	/* ��������������� */
	}
}


/*
*********************************************************************************************************
*	�� �� ��: MODS_05H
*	����˵��: 
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODS_05H(void)
{
	/*
		��������: д������Ȧ�Ĵ�����FF00Hֵ������Ȧ����ON״̬��0000Hֵ������Ȧ����OFF״̬
		��05Hָ�����õ�����Ȧ��״̬��15Hָ��������ö����Ȧ��״̬��
			11 �ӻ���ַ
			05 ������
			00 �Ĵ�����ַ���ֽ�
			AC �Ĵ�����ַ���ֽ�
			FF ����1���ֽ�
			00 ����2���ֽ�
			4E CRCУ����ֽ�
			8B CRCУ����ֽ�

		�ӻ�Ӧ��:
			11 �ӻ���ַ
			05 ������
			00 �Ĵ�����ַ���ֽ�
			AC �Ĵ�����ַ���ֽ�
			FF �Ĵ���1���ֽ�
			00 �Ĵ���1���ֽ�
			4E CRCУ����ֽ�
			8B CRCУ����ֽ�

		����:
		01 05 10 01 FF 00   D93A   -- D01��
		01 05 10 01 00 00   98CA   -- D01�ر�

		01 05 10 02 FF 00   293A   -- D02��
		01 05 10 02 00 00   68CA   -- D02�ر�

		01 05 10 03 FF 00   78FA   -- D03��
		01 05 10 03 00 00   390A   -- D03�ر�
	*/
	uint16_t reg;
	uint16_t value;

	g_tModS.RspCode = RSP_OK;
	
	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ����ֵ����� */
		goto err_ret;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* �Ĵ����� */
	value = BEBufToUint16(&g_tModS.RxBuf[4]);	/* ���� */

	if (value == 0xff00)
	{
		if (MODS_WriteRelay(reg, 1) == 0)
		{
			g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* �Ĵ�����ַ���� */
		}
	}
	else if (value == 0x0000)
	{
		if (MODS_WriteRelay(reg, 0) == 0)
		{
			g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* �Ĵ�����ַ���� */
		}
	}
	else
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* �Ĵ���ֵ����� */
	}
	
err_ret:
	if (g_tModS.RxBuf[0] != 0x00)	/* 00�㲥��ַ��Ӧ��, FF��ַӦ��g_tParam.Addr485 */
	{	
		if (g_tModS.RspCode == RSP_OK)			/* ��ȷӦ�� */
		{
			MODS_SendAckOk();
		}
		else
		{
			MODS_SendAckErr(g_tModS.RspCode);	/* ��������������� */
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_0FH
*	����˵��: дһ����������̵������ı�����̵���״̬��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void  MODS_0FH(void)
{
	uint16_t regaddr;
	uint16_t num;
	uint8_t m;
	uint8_t i;
	uint8_t *_pBuf;
	uint8_t status[MODS_DO_NUM / 8 + 1];

	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount < 10)
	{
		goto fail;
	}

	regaddr = BEBufToUint16(&g_tModS.RxBuf[2]); 
	num = BEBufToUint16(&g_tModS.RxBuf[4]);
	m = g_tModS.RxBuf[6];
	if ((m + 9) != g_tModS.RxCount)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;
		goto fail;
	}
	if ((num + 7) / 8 != m )
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* �Ĵ�����ַ���� */
		goto fail;
	}
	_pBuf = &g_tModS.RxBuf[7];
	
	if (m > 0 && m < sizeof(status))
	{
		for (i = 0; i < m; i++)
		{ 	
			status[i] = *(_pBuf++);
		}
		for(i = 0; i < num; i++)
		{
			if (status[i / 8] & (1 << (i % 8)))
			{
				if (MODS_WriteRelay(regaddr, 1) == 0)
				{
					g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* �Ĵ�����ַ���� */
					break;
				}
			}
			else
			{
				if (MODS_WriteRelay(regaddr, 0) == 0)
				{
					g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* �Ĵ�����ַ���� */
					break;
				}
			}
			regaddr++;
		}
	}
	else
	{
		g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* �Ĵ�����ַ���� */
	}

fail:	
	if (g_tModS.RxBuf[0] != 0x00)	/* 00�㲥��ַ��Ӧ��, FF��ַӦ��g_tParam.Addr485 */
	{	
		if (g_tModS.RspCode == RSP_OK)			/* ��ȷӦ�� */
		{
			g_tModS.TxCount = 0;
			
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tParam.Addr485;
		
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[1];
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[2];
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[3];
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[4];
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[5];
			
			MODS_SendWithCRC();
			return;
		}
		else
		{
			MODS_SendAckErr(g_tModS.RspCode);	/* ��������������� */
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_64H
*	����˵��: С�������ؽӿ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODS_64H(void)
{
	/*
		��������: С��������
			01  ; վ��
			64  ; ������
			0100 0000 ; �ܳ��� 4�ֽ�
			0000 0000 : ƫ�Ƶ�ַ 4�ֽ�
			0020 0000 : �������ݳ��� 4�ֽ�
			xx ... xx : �������ݣ�n��
			CCCC      : CRC16
	
		�ӻ�Ӧ��:
			01  ; �ӻ���ַ
			64  ; ������			
			00  ; ִ�н����0��ʾOK  1��ʾ����
			CCCC : CRC16
	*/
	uint32_t total_len;		/* ���򳤶� */
	uint32_t offset_addr;
	uint32_t package_len;	/* �������ݳ��� */
	
	g_tModS.RspCode = RSP_OK;
	
	if (g_tModS.RxCount < 11)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ����ֵ����� */
		goto err_ret;
	}
	total_len = BEBufToUint32(&g_tModS.RxBuf[2]);
	offset_addr = BEBufToUint32(&g_tModS.RxBuf[6]);
	package_len = BEBufToUint32(&g_tModS.RxBuf[10]);
	
	lua_DownLoad(offset_addr, &g_tModS.RxBuf[14], package_len, total_len);	/* ��lua���򱣴浽�ڴ� */
	
	if (offset_addr + package_len >= total_len)
	{
		luaL_dostring(g_Lua, s_lua_prog_buf);
	}
	
err_ret:
	if (g_tModS.RxBuf[0] != 0x00)	/* 00�㲥��ַ��Ӧ��, FF��ַӦ��g_tParam.Addr485 */
	{	
		if (g_tModS.RspCode == RSP_OK)			/* ��ȷӦ�� */
		{
			g_tModS.TxCount = 0;		
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tParam.Addr485;	/* ������ַ */
			g_tModS.TxBuf[g_tModS.TxCount++] = 0x64;	/* ������ */
			g_tModS.TxBuf[g_tModS.TxCount++] = 0x00;	/* ִ�н�� 00 */
			MODS_SendWithCRC();
		}
		else
		{
			MODS_SendAckErr(g_tModS.RspCode);	/* ��������������� */
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_65H
*	����˵��: ִ����ʱ�ű�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODS_65H(void)
{
	/*
		65H - ִ����ʱ��LUA��������֡������
			01  ; �ӻ���ַ
			65  ; ������
			0100 : �������ݳ��� 2�ֽ�
			xxxx : �ű����ݣ�0����
			CCCC : CRC16
	
		�ӻ�Ӧ��:
			01  ; �ӻ���ַ
			65  ; ������
			00  ; ִ�н����0��ʾOK  1��ʾ����
			CCCC : CRC16	
			
	*/
//	uint16_t lual_len;		/* ���򳤶� */
	
	g_tModS.RspCode = RSP_OK;
	
	if (g_tModS.RxCount < 11)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ����ֵ����� */
		goto err_ret;
	}
//	lual_len = BEBufToUint16(&g_tModS.RxBuf[2]);

	if (g_Lua > 0)
	{
		luaL_dostring(g_Lua, (char *)&g_tModS.RxBuf[4]);
	}
		
err_ret:
	if (g_tModS.RxBuf[0] != 0x00)	/* 00�㲥��ַ��Ӧ��, FF��ַӦ��g_tParam.Addr485 */
	{	
		if (g_tModS.RspCode == RSP_OK)			/* ��ȷӦ�� */
		{
			g_tModS.TxCount = 0;		
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tParam.Addr485;	/* ������ַ */
			g_tModS.TxBuf[g_tModS.TxCount++] = 0x65;	/* ������ */
			g_tModS.TxBuf[g_tModS.TxCount++] = 0x00;	/* ִ�н�� 00 */
			
			MODS_SendWithCRC();
		}
		else
		{
			MODS_SendAckErr(g_tModS.RspCode);	/* ��������������� */
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_66H
*	����˵��: wirte�ļ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODS_66H(void)
{
	/*
		66H - write file
		��������:
			01  ; վ��
			66  ; ������
			0100 0000 ; �ܳ��� 4�ֽ�
			0000 0000 : ƫ�Ƶ�ַ 4�ֽ�
			0020 0000 : �������ݳ��� 4�ֽ�
			xx ... xx : ���ݣ�n��
			CCCC      : CRC16	
			
		�ӻ�Ӧ��:
			01  ; �ӻ���ַ
			66  ; ������
			00  ; ִ�н����0��ʾOK  1��ʾ����
			CCCC : CRC16	
	*/
	uint32_t total_len;		/* ���򳤶� */
	uint32_t offset_addr;
	uint32_t package_len;	/* �������ݳ��� */
	
	g_tModS.RspCode = RSP_OK;
	
	if (g_tModS.RxCount < 11)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ����ֵ����� */
		goto err_ret;
	}
	total_len = BEBufToUint32(&g_tModS.RxBuf[2]);
	offset_addr = BEBufToUint32(&g_tModS.RxBuf[6]);
	package_len = BEBufToUint32(&g_tModS.RxBuf[10]);
	
	
	lua_DownLoad(offset_addr, &g_tModS.RxBuf[14], package_len, total_len);	/* �����ݱ��浽�ڴ� */
		
err_ret:
	if (g_tModS.RxBuf[0] != 0x00)	/* 00�㲥��ַ��Ӧ��, FF��ַӦ��g_tParam.Addr485 */
	{	
		if (g_tModS.RspCode == RSP_OK)			/* ��ȷӦ�� */
		{
			g_tModS.TxCount = 0;		
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tParam.Addr485;	/* ������ַ */
			g_tModS.TxBuf[g_tModS.TxCount++] = 0x64;	/* ������ */
			g_tModS.TxBuf[g_tModS.TxCount++] = 0x00;	/* ִ�н�� 00 */
			MODS_SendWithCRC();
		}
		else
		{
			MODS_SendAckErr(g_tModS.RspCode);	/* ��������������� */
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_67H
*	����˵��: read file
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODS_67H(void)
{
	/*
		67H -  read file
		��������:
			01  ; վ��
			67  ; ������
			0100 0000 ; �ܳ��� 4�ֽ� -- �ȱ����������ò���
			0000 0000 : ƫ�Ƶ�ַ 4�ֽ�
			0020 0000 : ���ݳ��� 4�ֽ�
			CCCC      : CRC16	
			
		�ӻ�Ӧ��:
			01  ; �ӻ���ַ
			67  ; ������
			00  ; ִ�н����0��ʾOK  1��ʾ����
			0000 0000 : ƫ�Ƶ�ַ 4�ֽ�
			0000 0000 : �������ݳ���
			xxx  : ������
			CCCC : CRC16	

	*/
//	uint32_t total_len;		/* ���򳤶� */
	uint32_t offset_addr;
	uint32_t package_len;	/* �������ݳ��� */
	
	g_tModS.RspCode = RSP_OK;
	
	if (g_tModS.RxCount < 11)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ����ֵ����� */
		goto err_ret;
	}
//	total_len = BEBufToUint32(&g_tModS.RxBuf[2]);
	offset_addr = BEBufToUint32(&g_tModS.RxBuf[6]);
	package_len = BEBufToUint32(&g_tModS.RxBuf[10]);

	lua_67H_Read(offset_addr, &g_tModS.TxBuf[11], package_len);
	
err_ret:
	if (g_tModS.RxBuf[0] != 0x00)	/* 00�㲥��ַ��Ӧ��, FF��ַӦ��g_tParam.Addr485 */
	{	
		if (g_tModS.RspCode == RSP_OK)			/* ��ȷӦ�� */
		{
			g_tModS.TxCount = 0;		
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tParam.Addr485;	/* ������ַ */
			g_tModS.TxBuf[g_tModS.TxCount++] = 0x67;	/* ������ */
			g_tModS.TxBuf[g_tModS.TxCount++] = 0x00;	/* ִ�н�� 00 */
			
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[6];
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[7];
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[8];
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[9];
			
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[10];
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[11];
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[12];
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[13];
			
			g_tModS.TxCount += package_len;
			
			MODS_SendWithCRC();
		}
		else
		{
			MODS_SendAckErr(g_tModS.RspCode);	/* ��������������� */
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_60H
*	����˵��: PC����ȡ�������ݣ������ʽ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODS_60H(void)
{
	/*
		PC���� 60H 
			01  ; �ӻ���ַ
			60  ; ������
			00  : 00��ʾPC�·���01��ʾ�豸Ӧ�� �����������˹�������
			01 00 00 00  : ͨ����ʹ�ܱ�־ 32bit��bit0��ʾCH1��bit1��ʾCH2
			00 00 04 00: ÿ��ͨ����������
			01 00 : ÿͨ�Ű���������. ��λΪ1��������
			00 00 00 00 : ͨ������ƫ�� ��������λ�������ط���
			CC CC : CRC16
	
		�ӻ�����Ӧ��: 60H -  
			
			01  ; �ӻ���ַ
			60  ; ������
			01  : 00��ʾPC�·���01��ʾ�豸Ӧ�� �����������˹�������
			01 00 00 00  : ͨ����ʹ�ܱ�־ 32bit��bit0��ʾCH1��bit1��ʾCH2
			00 00 04 00 : ÿ��ͨ����������
			01 00 : ÿͨ�Ű���������. ��λΪ1��������
			00 00 00 00 : ͨ������ƫ�� ��������λ�������ط���
			CCCC : CRC16
	
		�ӻ�Ӧ��: ��Ȼ��ʼ�������Ӧ��)
			01  ; �ӻ���ַ
			61  ; ������
			00  ; ͨ���ţ�00��ʾͨ��1,01��ʾͨ��2,
			00 00 00 00 : ƫ�Ƶ�ַ��������λ��
			01 00 : �������ݳ��ȡ�������λ��ÿ������4�ֽڡ�0x100��ʾ1024�ֽڡ�
			..... : ������
			CCCC : CRC16
	*/
//	uint16_t lual_len;		/* ���򳤶� */
	
	g_tModS.RspCode = RSP_OK;
	
	if (g_tModS.RxCount != 19)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ����ֵ����� */
		goto err_ret;
	}
	
	if (g_tModS.RxBuf[2] != 00)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ����ֵ����� */
		goto err_ret;		
	}
		
	g_tModWave.ChEn = BEBufToUint32(&g_tModS.RxBuf[3]);
	g_tModWave.SampleSize = BEBufToUint32(&g_tModS.RxBuf[7]);
	g_tModWave.PackageSize = BEBufToUint16(&g_tModS.RxBuf[11]);
	g_tModWave.SampleOffset = BEBufToUint32(&g_tModS.RxBuf[13]);
	
	g_tModWave.TransPos = 0;	/* �����λ�ü��� */
	g_tModWave.StartTrans = 1;	/* ��ʼ����ı�־ */

err_ret:
	if (g_tModS.RxBuf[0] != 0x00)	/* 00�㲥��ַ��Ӧ��, FF��ַӦ��g_tParam.Addr485 */
	{	
		if (g_tModS.RspCode == RSP_OK)			/* ��ȷӦ�� */
		{
			g_tModS.TxCount = 0;		
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tParam.Addr485;	/* ������ַ */
			g_tModS.TxBuf[g_tModS.TxCount++] = 0x60;	/* ������ */
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[2];
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[3];
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[4];
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[5];
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[6];
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[7];
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[8];
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[9];
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[10];
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[11];
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[12];
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[13];
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[14];
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[15];
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[16];			
			
			MODS_SendWithCRC();
		}
		else
		{
			MODS_SendAckErr(g_tModS.RspCode);	/* ��������������� */
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: Send_61H
*	����˵��: ���䲨�Ρ� �Զ�����������䡣���������ȶ���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void Send_61H(uint8_t _Ch, uint32_t _Offset, uint16_t _PackageLen)
{
	/*
		�ӻ�Ӧ��: ��Ȼ��ʼ�������Ӧ��)
			01  ; �ӻ���ַ
			61  ; ������
			00  ; ͨ���ţ�00��ʾͨ��1,01��ʾͨ��2,
			00 00 00 00 : ƫ�Ƶ�ַ��������λ��
			01 00 : �������ݳ��ȡ�������λ��ÿ������4�ֽڡ�0x100��ʾ1024�ֽڡ�
			..... : ������
			CRC16
	*/
	uint16_t i;
	uint8_t *p;	

	g_tModS.TxCount = 0;		
	g_tModS.TxBuf[g_tModS.TxCount++] = g_tParam.Addr485;	/* ������ַ */
	g_tModS.TxBuf[g_tModS.TxCount++] = 0x61;	/* ������ */
	g_tModS.TxBuf[g_tModS.TxCount++] = _Ch;
	g_tModS.TxBuf[g_tModS.TxCount++] = _Offset >> 24;
	g_tModS.TxBuf[g_tModS.TxCount++] = _Offset >> 16;
	g_tModS.TxBuf[g_tModS.TxCount++] = _Offset >> 8;
	g_tModS.TxBuf[g_tModS.TxCount++] = _Offset;	
	g_tModS.TxBuf[g_tModS.TxCount++] = _PackageLen >> 8;
	g_tModS.TxBuf[g_tModS.TxCount++] = _PackageLen;
	
	if (_Ch == 0)
	{
		for (i = 0; i < _PackageLen; i++)
		{			
			p = (uint8_t *)&g_Ch1WaveBuf[_Offset + i];
			g_tModS.TxBuf[g_tModS.TxCount++] = p[3];
			g_tModS.TxBuf[g_tModS.TxCount++] = p[2];
			g_tModS.TxBuf[g_tModS.TxCount++] = p[1];
			g_tModS.TxBuf[g_tModS.TxCount++] = p[0];
		}
	}
	else if (_Ch == 1)
	{
		for (i = 0; i < _PackageLen; i++)
		{
			p = (uint8_t *)&g_Ch2WaveBuf[_Offset + i];
			g_tModS.TxBuf[g_tModS.TxCount++] = p[3];
			g_tModS.TxBuf[g_tModS.TxCount++] = p[2];
			g_tModS.TxBuf[g_tModS.TxCount++] = p[1];
			g_tModS.TxBuf[g_tModS.TxCount++] = p[0];
		}
	}
	
	MODS_SendWithCRC();
	
	USBCom_SendBufNow(0, g_tModS.TxBuf, g_tModS.TxCount);
}

#if 0
/*
*********************************************************************************************************
*	�� �� ��: TransWaveTask
*	����˵��: ���䲨������ ����bsp_Idle����. ����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void TransWaveTask(void)
{
	static uint32_t s_TransPos = 0;
	static uint8_t s_ChPos = 0;
		
	if (g_tModWave.StartTrans == 0)
	{
		return;
	}
	
	while (g_tModWave.StartTrans != 0)
	{
		lwip_pro();		/* ��̫��Э��ջ��ѯ */	
	
		wifi_task();
		
		switch (g_tModWave.StartTrans)
		{
			case 1:
				s_ChPos = 0;
				s_TransPos = g_tModWave.SampleOffset;
				g_tModWave.StartTrans++;
				break;
			
			case 2:
				if (g_tModWave.ChEn & (1 << s_ChPos))
				{
					g_tModWave.StartTrans++;
				}
				else
				{
					s_ChPos++;
					
					if (s_ChPos >= 2)
					{
						g_tModWave.StartTrans = 100;	/* ������� */
					}
					else
					{
						g_tModWave.StartTrans++;
					}
				}
				break;
			
			case 3:
				Send_61H(s_ChPos, s_TransPos, g_tModWave.PackageSize);
				g_tModWave.StartTrans++;
				break;
			
			case 4:	/* �ȴ�������� - ��ʱδ�� */
				g_tModWave.StartTrans++;
				break;
			
			case 5: 
				s_TransPos += g_tModWave.PackageSize;
				if (s_TransPos >= g_tModWave.SampleSize)
				{
					s_ChPos++;
					s_TransPos = g_tModWave.SampleOffset;
					g_tModWave.StartTrans = 2;
				}
				else
				{
					g_tModWave.StartTrans = 3;
				}
				break;
			
			case 100:
				g_tModWave.StartTrans = 0;	/* ������� */
				break;
		}
	}
}
#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
