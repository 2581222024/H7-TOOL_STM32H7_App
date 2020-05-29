
-------------------------------------------------------
-- �ļ��� : STM32F0xx_64.lua
-- ��  �� : V1.0  2020-04-28
-- ˵  �� :
-------------------------------------------------------
function config_cpu(void)
	CHIP_TYPE = "SWD"		--ָ�������ӿ�����: "SWD", "SWIM", "SPI", "I2C", "UART"

	AlgoFile_FLASH = "0:/H7-TOOL/Programmer/Device/Nuvoton/M0518/M0518_AP_64.FLM.FLM"
	AlgoFile_OTP   = ""
	AlgoFile_OPT   = "0:/H7-TOOL/Programmer/Device/Nuvoton/M0518/M0518_CFG.FLM"
	AlgoFile_QSPI  = ""
	AlgoFile_DAT   = "0:/H7-TOOL/Programmer/Device/Nuvoton/M0518/M0518_DAT.FLM"

	FLASH_ADDRESS = 0x00000000		--CPU�ڲ�FLASH��ʼ��ַ

	DATA_ADDRESS = 0x0001F000		--CPU�ڲ�FLASH��ʼ��ַ


	RAM_ADDRESS = 0x20000000		--CPU�ڲ�RAM��ʼ��ַ

	--Flash�㷨�ļ������ڴ��ַ�ʹ�С
	AlgoRamAddr = RAM_ADDRESS
	AlgoRamSize = 4 * 1024

	MCU_ID = 0x0BB11477

	UID_ADDR = 0x1FFFF7AC	   	--UID��ַ����ͬ��CPU��ͬ
	UID_BYTES = 12

	OB_ADDRESS     = "00300000 00300001 00300002 00300003 00300004 00300005 00300006 00300007"

	OB_SECURE_OFF  = "FF FF FF FF FF FF FF FF"	--SECURE_ENABLE = 0ʱ�������Ϻ�д���ֵ(�������)
	OB_SECURE_ON   = "FD FF FF FF FF FF FF FF"	--SECURE_ENABLE = 1ʱ�������Ϻ�д���ֵ(оƬ����)

	--�ж϶�������д����������(WRP = Write protection)  1101
	OB_WRP_ADDRESS   = {0x00300000} 	--�ڴ��ַ
	OB_WRP_MASK  	 = {0x20}			--�����������������
	OB_WRP_VALUE 	 = {0x20}			--�����������Ƚϣ���ȱ�ʾû�б���
end

function init_cpu(void)
	local reg
	local re

	--SYS_UnlockReg();
	print(pg_write32(0x50000100, 0x59))
	print(pg_write32(0x50000100, 0x16))
	print(pg_write32(0x50000100, 0x88))

	--FMC_Open
	--FMC->ISPCON |=  FMC_ISPCON_ISPEN_Msk;
	re,reg = pg_read32(0x5000C000)
	pg_write32(0x5000C000, reg | 1)
end

---------------------------����-----------------------------------
