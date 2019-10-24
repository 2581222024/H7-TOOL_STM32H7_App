/*
*********************************************************************************************************
*
*	ģ������ : cpu�ڲ�falsh����ģ��(for F4)
*	�ļ����� : bsp_cpu_flash.c
*	��    �� : V1.0
*	˵    �� : �ṩ��дCPU�ڲ�Flash�ĺ���
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2013-02-01 armfly  ��ʽ����
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

#define FLASH_BASE_ADDR      (uint32_t)(FLASH_BASE)
#define FLASH_END_ADDR       (uint32_t)(0x081FFFFF)

/* Base address of the Flash sectors Bank 1 */
#define ADDR_FLASH_SECTOR_0_BANK1     ((uint32_t)0x08000000) /* Base @ of Sector 0, 128 Kbytes */
#define ADDR_FLASH_SECTOR_1_BANK1     ((uint32_t)0x08020000) /* Base @ of Sector 1, 128 Kbytes */
#define ADDR_FLASH_SECTOR_2_BANK1     ((uint32_t)0x08040000) /* Base @ of Sector 2, 128 Kbytes */
#define ADDR_FLASH_SECTOR_3_BANK1     ((uint32_t)0x08060000) /* Base @ of Sector 3, 128 Kbytes */
#define ADDR_FLASH_SECTOR_4_BANK1     ((uint32_t)0x08080000) /* Base @ of Sector 4, 128 Kbytes */
#define ADDR_FLASH_SECTOR_5_BANK1     ((uint32_t)0x080A0000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6_BANK1     ((uint32_t)0x080C0000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7_BANK1     ((uint32_t)0x080E0000) /* Base @ of Sector 7, 128 Kbytes */

/* Base address of the Flash sectors Bank 2 */
#define ADDR_FLASH_SECTOR_0_BANK2     ((uint32_t)0x08100000) /* Base @ of Sector 0, 128 Kbytes */
#define ADDR_FLASH_SECTOR_1_BANK2     ((uint32_t)0x08120000) /* Base @ of Sector 1, 128 Kbytes */
#define ADDR_FLASH_SECTOR_2_BANK2     ((uint32_t)0x08140000) /* Base @ of Sector 2, 128 Kbytes */
#define ADDR_FLASH_SECTOR_3_BANK2     ((uint32_t)0x08160000) /* Base @ of Sector 3, 128 Kbytes */
#define ADDR_FLASH_SECTOR_4_BANK2     ((uint32_t)0x08180000) /* Base @ of Sector 4, 128 Kbytes */
#define ADDR_FLASH_SECTOR_5_BANK2     ((uint32_t)0x081A0000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6_BANK2     ((uint32_t)0x081C0000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7_BANK2     ((uint32_t)0x081E0000) /* Base @ of Sector 7, 128 Kbytes */


/*
*********************************************************************************************************
*	�� �� ��: bsp_GetSector
*	����˵��: ���ݵ�ַ���������׵�ַ
*	��    ��: ��
*	�� �� ֵ: �����ţ�0-7)
*********************************************************************************************************
*/
uint32_t bsp_GetSector(uint32_t Address)
{
	uint32_t sector = 0;

	if (((Address < ADDR_FLASH_SECTOR_1_BANK1) && (Address >= ADDR_FLASH_SECTOR_0_BANK1)) || \
		((Address < ADDR_FLASH_SECTOR_1_BANK2) && (Address >= ADDR_FLASH_SECTOR_0_BANK2)))    
	{
		sector = FLASH_SECTOR_0;  
	}
	else if (((Address < ADDR_FLASH_SECTOR_2_BANK1) && (Address >= ADDR_FLASH_SECTOR_1_BANK1)) || \
	  ((Address < ADDR_FLASH_SECTOR_2_BANK2) && (Address >= ADDR_FLASH_SECTOR_1_BANK2)))    
	{
		sector = FLASH_SECTOR_1;  
	}
	else if (((Address < ADDR_FLASH_SECTOR_3_BANK1) && (Address >= ADDR_FLASH_SECTOR_2_BANK1)) || \
	  ((Address < ADDR_FLASH_SECTOR_3_BANK2) && (Address >= ADDR_FLASH_SECTOR_2_BANK2)))    
	{
		sector = FLASH_SECTOR_2;  
	}
	else if (((Address < ADDR_FLASH_SECTOR_4_BANK1) && (Address >= ADDR_FLASH_SECTOR_3_BANK1)) || \
	  ((Address < ADDR_FLASH_SECTOR_4_BANK2) && (Address >= ADDR_FLASH_SECTOR_3_BANK2)))    
	{
		sector = FLASH_SECTOR_3;  
	}
	else if (((Address < ADDR_FLASH_SECTOR_5_BANK1) && (Address >= ADDR_FLASH_SECTOR_4_BANK1)) || \
	  ((Address < ADDR_FLASH_SECTOR_5_BANK2) && (Address >= ADDR_FLASH_SECTOR_4_BANK2)))    
	{
		sector = FLASH_SECTOR_4;  
	}
	else if (((Address < ADDR_FLASH_SECTOR_6_BANK1) && (Address >= ADDR_FLASH_SECTOR_5_BANK1)) || \
	  ((Address < ADDR_FLASH_SECTOR_6_BANK2) && (Address >= ADDR_FLASH_SECTOR_5_BANK2)))    
	{
		sector = FLASH_SECTOR_5;  
	}
	else if (((Address < ADDR_FLASH_SECTOR_7_BANK1) && (Address >= ADDR_FLASH_SECTOR_6_BANK1)) || \
	  ((Address < ADDR_FLASH_SECTOR_7_BANK2) && (Address >= ADDR_FLASH_SECTOR_6_BANK2)))    
	{
		sector = FLASH_SECTOR_6;  
	}
	else if (((Address < ADDR_FLASH_SECTOR_0_BANK2) && (Address >= ADDR_FLASH_SECTOR_7_BANK1)) || \
	  ((Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_7_BANK2)))
	{
		sector = FLASH_SECTOR_7;  
	}
	else
	{
		sector = FLASH_SECTOR_7;
	}

	return sector;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_ReadCpuFlash
*	����˵��: ��ȡCPU Flash������
*	��    ��:  _ucpDst : Ŀ�껺����
*			 _ulFlashAddr : ��ʼ��ַ
*			 _ulSize : ���ݴ�С����λ���ֽڣ�
*	�� �� ֵ: 0=�ɹ���1=ʧ��
*********************************************************************************************************
*/
uint8_t bsp_ReadCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpDst, uint32_t _ulSize)
{
	uint32_t i;

	if (_ulFlashAddr + _ulSize > FLASH_BASE + FLASH_SIZE)
	{
		return 1;
	}

	/* ����Ϊ0ʱ����������,������ʼ��ַΪ���ַ����� */
	if (_ulSize == 0)
	{
		return 1;
	}

	for (i = 0; i < _ulSize; i++)
	{
		*_ucpDst++ = *(uint8_t *)_ulFlashAddr++;
	}

	return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_CmpCpuFlash
*	����˵��: �Ƚ�Flashָ����ַ������.
*	��    ��: _ulFlashAddr : Flash��ַ
*			 _ucpBuf : ���ݻ�����
*			 _ulSize : ���ݴ�С����λ���ֽڣ�
*	�� �� ֵ:
*			FLASH_IS_EQU		0   Flash���ݺʹ�д���������ȣ�����Ҫ������д����
*			FLASH_REQ_WRITE		1	Flash����Ҫ������ֱ��д
*			FLASH_REQ_ERASE		2	Flash��Ҫ�Ȳ���,��д
*			FLASH_PARAM_ERR		3	������������
*********************************************************************************************************
*/
uint8_t bsp_CmpCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpBuf, uint32_t _ulSize)
{
	uint32_t i;
	uint8_t ucIsEqu;	/* ��ȱ�־ */
	uint8_t ucByte;

	/* ���ƫ�Ƶ�ַ����оƬ�������򲻸�д��������� */
	if (_ulFlashAddr + _ulSize > FLASH_BASE + FLASH_SIZE)
	{
		return FLASH_PARAM_ERR;		/*��������������*/
	}

	/* ����Ϊ0ʱ������ȷ */
	if (_ulSize == 0)
	{
		return FLASH_IS_EQU;		/* Flash���ݺʹ�д���������� */
	}

	ucIsEqu = 1;			/* �ȼ��������ֽںʹ�д���������ȣ���������κ�һ������ȣ�������Ϊ 0 */
	for (i = 0; i < _ulSize; i++)
	{
		ucByte = *(uint8_t *)_ulFlashAddr;

		if (ucByte != *_ucpBuf)
		{
			if (ucByte != 0xFF)
			{
				return FLASH_REQ_ERASE;		/* ��Ҫ��������д */
			}
			else
			{
				ucIsEqu = 0;	/* ����ȣ���Ҫд */
			}
		}

		_ulFlashAddr++;
		_ucpBuf++;
	}

	if (ucIsEqu == 1)
	{
		return FLASH_IS_EQU;	/* Flash���ݺʹ�д���������ȣ�����Ҫ������д���� */
	}
	else
	{
		return FLASH_REQ_WRITE;	/* Flash����Ҫ������ֱ��д */
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_WriteCpuFlash
*	����˵��: д���ݵ�CPU �ڲ�Flash��
*	��    ��: _ulFlashAddr : Flash��ַ
*			 _ucpSrc : ���ݻ�����
*			 _ulSize : ���ݴ�С����λ���ֽ�, 32�ֽ���������
*	�� �� ֵ: 0-�ɹ���1-���ݳ��Ȼ��ַ�����2-дFlash����(����Flash������)
*********************************************************************************************************
*/
uint8_t bsp_WriteCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpSrc, uint32_t _ulSize)
{
	uint32_t i;
	uint8_t ucRet;

	/* ���ƫ�Ƶ�ַ����оƬ�������򲻸�д��������� */
	if (_ulFlashAddr + _ulSize > FLASH_BASE + FLASH_SIZE)
	{
		return 1;
	}

	/* ����Ϊ0ʱ����������  */
	if (_ulSize == 0)
	{
		return 0;
	}

	ucRet = bsp_CmpCpuFlash(_ulFlashAddr, _ucpSrc, _ulSize);

	if (ucRet == FLASH_IS_EQU)
	{
		return 0;
	}

	__set_PRIMASK(1);  		/* ���ж� */

	/* FLASH ���� */
	HAL_FLASH_Unlock();

	/* ��Ҫ���� */
	if (ucRet == FLASH_REQ_ERASE)
	{
		uint32_t FirstSector = 0, NbOfSectors = 0;
		FLASH_EraseInitTypeDef EraseInitStruct;
		uint32_t SECTORError = 0;
		
		/* Get the 1st sector to erase */
		FirstSector = bsp_GetSector(_ulFlashAddr);
		/* Get the number of sector to erase from 1st sector*/
		NbOfSectors = bsp_GetSector(_ulFlashAddr + _ulSize) - FirstSector + 1;

		/* Fill EraseInit structure*/
		EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
		EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
		EraseInitStruct.Banks         = FLASH_BANK_1;
		EraseInitStruct.Sector        = FirstSector;
		EraseInitStruct.NbSectors     = NbOfSectors;
		if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
		{
			/*
			Error occurred while sector erase.
			User can add here some code to deal with this error.
			SECTORError will contain the faulty sector and then to know the code error on this sector,
			user can call function 'HAL_FLASH_GetError()'
			*/
			/* Infinite loop */
			while (1)
			{
				;
			}
		}
	}

	for (i = 0; i < _ulSize / 32; i++)	/* δ�����32�ֽ�������������� */
	{
		uint64_t FlashWord[4];
		
		memcpy((char *)FlashWord, _ucpSrc, 32);	/* �Ƿ���Ҫ�����ֽ����д����������� */
		_ucpSrc += 32;
		
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, _ulFlashAddr, (uint64_t)((uint32_t)FlashWord)) == HAL_OK)
		{
			_ulFlashAddr = _ulFlashAddr + 32; /* increment for the next Flash word*/			
		}			
	}

  	/* Flash ��������ֹдFlash���ƼĴ��� */
  	HAL_FLASH_Lock();

  	__set_PRIMASK(0);  		/* ���ж� */

	return 0;
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
