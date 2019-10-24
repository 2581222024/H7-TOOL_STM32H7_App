/*
*********************************************************************************************************
*
*	ģ������ : wifiģ�������ӿ� interface
*	�ļ����� : wifi_if.c
*	��    �� : V1.0
*	˵    �� : ���ݲ�ͬ���ҵ�ģ�飬�ṩapi��wifiͨ�ų���ʹ�á�����WiFiģ��֮��Ĳ��졣
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		V1.0    2015-12-17 armfly  ��ʽ����
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "wifi_if.h"
#include "param.h"
#include "main.h"

uint8_t link_id = 0xFF;	/* ��ǰ��TCP����id,  0-4��Ч��  FF��ʾ��TCP���� */
uint8_t wifi_state = WIFI_STOP;

/* ��¼���ͻ����������ݳ��� */
uint8_t s_wifi_send_req = 0;
uint8_t *s_wifi_send_buf;
uint8_t s_wifi_send_len;

uint8_t wifi_link_tcp_ok = 0;
uint8_t wifi_join_ap_ok = 0;

uint8_t wifi_server_ack_ok = 0;	/* ���͵�ƽ̨��ƽ̨Ӧ��OK */
			
void wifi_ReprotIOEvent(void);
void wifi_ReprotIOEvent(void);
void wifi_WatchDog(void);
uint8_t wifi_SendRegisterPackage(void) ;

void wifi_Poll(void);

// wifi����������.... 
void wifi_led_joinap(void)
{
	PERIOD_Start(&g_tRunLed, 50, 30, 0);	/* ����wifi�����п�����˸ */
}

// wifi�ɹ��� home wifiģʽ 
void wifi_led_ok(void)
{
	PERIOD_Stop(&g_tRunLed);		// ���ӳɹ��ر�LED 
	//PERIOD_Start(&g_tRunLed, 500, 1500, 0);	/* ÿ2����˸һ�� */
}

// ����TCP Server ʧ��
void wifi_led_err_link_tcp_server(void)
{
	PERIOD_Start(&g_tRunLed, 250, 250, 0);	/* ÿ500ms��˸һ�� */	
}


// wifi�ɹ�, P2P ģʽ
void wifi_led_ok_p2p(void)
{
	PERIOD_Start(&g_tRunLed, 200, 800, 0);	/* ÿ1����˸һ�� */
}

// wifi����ʧ��
void wifi_led_err(void)
{
	//PERIOD_Stop(&g_tWiFiLed);		
}

// �յ�ָ���˸1��
void wifi_led_rx_data(void)
{
	PERIOD_Start(&g_tRunLed, 100, 10, 1);  // 3���Ӳ�һ�����ݣ�̫���ˣ��ɴ಻ֻ���������� 
}

// ����ָ��ʱ��˸1��
void wifi_led_tx_data(void)
{
	PERIOD_Start(&g_tRunLed, 100, 10, 1);  // 3���Ӳ�һ�����ݣ�̫���ˣ��ɴ಻ֻ���������� 
}

uint8_t wifi_WatchDog_PT(uint8_t _init);

/*
*********************************************************************************************************
*	�� �� ��: wifi_task
*	����˵��: NB73���񣬲���bsp_Idle()ִ�С�������ע�⣬�˺����ڲ������� bsp_Idle���ã�����Ƕ��.
*	��    ��: ��
*	�� �� ֵ: 0:����������  1:�ɹ���⵽�ַ���  2:��ʱ��
*********************************************************************************************************
*/
uint8_t wifi_task(void)
{
	static int32_t s_last_time = 0;
	char buf[64];
	static uint8_t retry = 0;
	uint16_t rx_len;
	uint8_t re;
	
	if (g_tVar.WiFiRecivedIPD == 1)		/* �յ�UDP. TCP���ݰ� */
	{
		g_tVar.WiFiRecivedIPD = 0;
		
		wifi_led_rx_data();		/* ����led��һ�� */
		
		wifi_server_ack_ok = 1;		/* 2019-01-17, �յ�ƽ̨���ݰ��� �򵥴���δ��У�� */
		
		return 1;
	}	
			
//	g_tVar.WiFiIdleDoRS485 = 1;		/* bsp_Idle ִ��MODBUS RTU 485 */
		
	switch (wifi_state)
	{
		case WIFI_STOP:
			break;
						
		case WIFI_INIT:
//			g_tVar.WiFiDebugEn  = 1;
		
			wifi_led_joinap();		/* ����AP���̣����� */
		
			comSetBaud(COM_ESP32, 115200);
			bsp_InitESP32();	/* �ڲ��� 20ms �ӳ� */
			s_last_time = bsp_GetRunTime();
			WIFI_CheckAck("", 0);			/* 0������ʾ��ʼ�������ڲ��ľ�̬���� */
			wifi_state++;
			break;
		
		case WIFI_INIT + 1:
			re = WIFI_CheckAck("ready", 300);
			if (re == 1)
			{
				wifi_state++;			/* �յ���ȷӦ�� */
			}
			else if (re == 2)
			{
				wifi_state = WIFI_INIT;	/* ��ʱ */
			}			
			break;
			
		case WIFI_INIT + 2:
			/* �رջ��Թ��ܣ��������͵��ַ���ģ�����践�� */	
			ESP32_SendAT("ATE0");		
			ESP32_WaitResponse("OK\r\n", 100);

			/* ��ȡMAC */
			{
				uint8_t mac[6];
				const uint8_t mac_0[6] = {0,0,0,0,0,0};
				
				ESP32_GetMac(mac);		
				
				if (memcmp(mac, g_tParam.WiFiMac, 6) != 0 && memcmp(mac, mac_0, 6) != 0)
				{
					memcpy(g_tParam.WiFiMac, mac, 6);
					SaveParam();
				}
			}
			
			/* ���ݲ��������Ƿ����softAP */
			if (g_tParam.APSelfEn == 1)
			{
				wifi_state = WIFI_SOFT_AP;
			}
			else
			{
				wifi_state = WIFI_LINK_AP;
			}
			break;

		case WIFI_LINK_AP:
			ESP32_SetWiFiMode(1);		/* 1 = STA, 2 = SAP,  3 = SAP + Stationģʽ */	
			
			if (g_tParam.DHCPEn == 0)	/* DHCH = 0, ʹ�þ�̬IP */
			{				
				ESP32_SendAT("AT+CWDHCP_CUR=1,0");	
				ESP32_WaitResponse("OK\r\n", 300);

				ESP32_SetLocalIP(g_tParam.WiFiIPAddr, g_tParam.WiFiNetMask, g_tParam.WiFiGateway);	/* ���þ�̬IP */	
			}
			else	/* DHCH = 1, ʹ�ö�̬IP */
			{				
				ESP32_SendAT("AT+CWDHCP_CUR=1,1");	
				ESP32_WaitResponse("OK\r\n", 300);
			}			
			wifi_led_joinap();	/* ����wifi�����п�����˸ */	
			wifi_state++;
			break;

		case WIFI_LINK_AP + 1:				
			if (ESP32_ValidSSID((char *)g_tParam.AP_SSID) == 0 || ESP32_ValidPassword((char *)g_tParam.AP_PASS) == 0)
			{
				wifi_state = WIFI_STOP;		/* iFi SSID����������쳣 */
				break;
			}	
			
			sprintf(buf, "AT+CWJAP_CUR=\"%s\",\"%s\"", g_tParam.AP_SSID, g_tParam.AP_PASS);
			ESP32_SendAT(buf);
			s_last_time = bsp_GetRunTime();
			wifi_state++;
			break;
			
		case WIFI_LINK_AP + 2:
			if (ESP32_ReadLineNoWait(buf, 64))
			{
				if (memcmp(buf, "OK", 2) == 0)
				{				
					wifi_state = WIFI_LINK_AP + 3;	/* ����AP OK */
				}					
				else if (memcmp(buf, "WIFI CONNECTED", 14) == 0
					|| memcmp(buf, "WIFI GOT IP", 11) == 0)
				{
					; /* ���ӹ����У�����Ӧ�𣬲���ᣬ�����ȴ�����OK */
				}
				else if (memcmp(buf, "+CWJAP:", 7) == 0 || memcmp(buf, "FAIL", 4) == 0
					|| memcmp(buf, "DISCONNECT", 10) == 0)
				{
					wifi_state = WIFI_INIT;	/* ����ʧ�� */
				}
			}
			
			if (bsp_CheckRunTime(s_last_time) > 20 * 1000)	/* ��ʱ */
			{
				wifi_state = WIFI_INIT;
			}
			break;
	
		case WIFI_LINK_AP + 3:			/* ����AP OK */
			g_tVar.HomeWiFiLinkOk = 1;	/* �����ӵ�HOME WIFI */
			wifi_led_ok();		/* LEDϨ�� */
			
			ESP32_CIPMUX(1);		/* ���ö�����ģʽ */
			//ESP32_CloseTcpUdp(LINK_ID_UDP_SERVER);
		
			/* ����TCP������. */
			ESP32_CreateTCPServer(g_tParam.LocalTCPPort);		

			/* ����UDP�����˿�, id = 0 */
			ESP32_CreateUDPServer(LINK_ID_UDP_SERVER, g_tParam.LocalTCPPort);		
			
			wifi_state = WIFI_READY;
			break;		
		
		/*---------------------------------------------------------------------------*/			
		case WIFI_WATCH_DOG:					/* WiFi���� */
			retry = 0;
			wifi_state++;
			break;

		case WIFI_WATCH_DOG + 1:				/* WiFi���� */
			wifi_link_tcp_ok = 0;
			wifi_join_ap_ok = 0;
			ESP32_SendAT("AT+CIPSTATUS");
			s_last_time = bsp_GetRunTime();
			wifi_state++;
			break;		
		
		case WIFI_WATCH_DOG + 2:
			while (1)
			{
				rx_len = ESP32_ReadLineNoWait(buf, 64);
				if (rx_len == 0)
				{
					break;
				}
				else if (rx_len > 7 && memcmp(buf, "STATUS:", 7) == 0)
				{
					/*  STATUS:3 */						
					if (buf[7] == '2' || buf[7] == '3' || buf[7] == '4')
					{
						wifi_join_ap_ok = 1;	/* ����AP ok */
					}
					else
					{
						if (g_tParam.APSelfEn == 1)	/* ��SoftAP */
						{
							;	/* ���ж�, ����UDP״̬�ж� */
						}
						else	/* ��STAվ�� */
						{
							wifi_join_ap_ok = 0;	/* û�����ӵ�AP */
						}
					}
				}
				else if (rx_len >= 18 && memcmp(buf, "+CIPSTATUS:0,\"UDP\"", 18) == 0)
				{
					if (g_tParam.APSelfEn == 1)
					{
						wifi_join_ap_ok = 1;
					}
				}					
				else if (rx_len >= 18 && memcmp(buf, "+CIPSTATUS:4,\"TCP\"", 18) == 0)
				{
					/* +CIPSTATUS:4,"TCP","192.168.1.3",9800,37299,0 */
					wifi_link_tcp_ok = 1;
				}
				else if (rx_len >= 2 && memcmp(buf, "OK", 2) == 0)
				{
					if (wifi_join_ap_ok == 1)
					{
						wifi_state = WIFI_READY;
					}
					else
					{
						//wifi_state = WIFI_INIT; ��Ҫ�����ı�״̬����3�β�ѯʧ�ܺ�����
					}							
				}	
				else if (rx_len >= 5 && memcmp(buf, "busy p...", 5) == 0)	/* �ڲ�æ */
				{
					;
				}		
			}
			
			if (bsp_CheckRunTime(s_last_time) > 100)	/* ��ʱ */
			{	
				if (++retry > 2)
				{			
					if (wifi_join_ap_ok == 0)
					{
						wifi_state = WIFI_INIT;		/* ��λWIFIģ�飬����AP */
					}
					else
					{
						wifi_state = WIFI_READY;	/* Ӧ�ý����� */
					}
				}
				else
				{
					wifi_state--;
				}
			}		
			break;

		/*---------------------------------------------------------------------------*/			
		case WIFI_READY:				/* wifi ���� */			
			wifi_Poll();
			break;
	}
	
	return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: wifi_Start_SoftAP_Station
*	����˵��: ����WIFIģ�鹤����SoftAP + Stationģʽ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void wifi_Start_SoftAP_Station(void)
{
	uint8_t re;
	
	ESP32_SetWiFiMode(2);		/* 1 = STA, 2 = SAP,  3 = SAP + Stationģʽ */
	
	/*
	CPU ID :
		00 43 00 3B 57 18 52 34 30 38 20 33
		00 36 00 42 57 17 52 34 30 38 20 33
	       *      *  *  *
	*/
	
	/* ��Soft AP ��SSID ������ */
	{
//		char ap_name[40];
//		uint16_t sn0, sn1, sn2;
//		
//		sn0 = *(uint16_t *)(0x1ffff7ac);
//		sn1 = *(uint16_t *)(0x1ffff7ac + 2);
//		sn2 = *(uint16_t *)(0x1ffff7ac + 4);
//		
//		/* g_tParam1.GrillID ��8λascii */
//		sprintf(ap_name, "R%04X_%03d_%02X%02X%04X", HARD_MODEL,  g_tParam.Addr485, sn0 & 0xFF, sn1 & 0xFF, sn2);
		
		/* ����AP��IP��ַ */
		{
			char ip_str[20];
			
			sprintf(ip_str, "%d.%d.%d.%d", g_tParam.WiFiIPAddr[0], g_tParam.WiFiIPAddr[1],g_tParam.WiFiIPAddr[2],g_tParam.WiFiIPAddr[3]);
			ESP32_Set_AP_IP(ip_str);
		}
		
		/* ����AP�����ֺ����� */
		ESP32_Set_AP_NamePass((char *)g_tParam.AP_SSID, (char *)g_tParam.AP_PASS, 1, ECN_WPA2_PSK);		
	}
	
	/* DHCH = 0 */
	ESP32_SendAT("AT+CWDHCP=2,0");	
	ESP32_WaitResponse("OK\r\n", 1000);
	
	ESP32_CIPMUX(1);		/* ���ö�����ģʽ */

	/* ����UDP�����˿�, id = 0 */
	re = ESP32_CreateUDPServer(LINK_ID_UDP_SERVER, g_tParam.LocalTCPPort);

	if (re == 1)	/* ִ�гɹ� */
	{
		wifi_led_ok();
	}
	else	/* ִ��ʧ�� */
	{
		wifi_led_err();
	}	
}

/*
*********************************************************************************************************
*	�� �� ��: wifi_LinkHomeAP
*	����˵��: ���ӵ�AP������UDP�����TCP����������Ҫ�õġ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t wifi_LinkHomeAP(void)
{	
	uint8_t re;	
	
	/* 2018-12-12 ��������ʱ��Ӳ����λWiFiģ�� */
	{
		bsp_InitESP32();
		comClearRxFifo(COM_ESP32);	/* �ȴ����ͻ�����Ϊ�գ�Ӧ�����*/	
		ESP32_PowerOn();
	}
	
	g_tVar.HomeWiFiLinkOk = 0;	/* �����ӵ�HOME WIFI */
	
	/* �رջ��Թ��ܣ��������͵��ַ���ģ�����践�� */	
	ESP32_SendAT("ATE0");		
	ESP32_WaitResponse("OK\r\n", 100);
	
	/* �Լ���SoftAPʱ�������������� */
	if (g_tParam.APSelfEn == 1)
	{
		wifi_Start_SoftAP_Station();	/* �Լ�����SoftAP, ����UDP�������� */		
		return 1;	
	}
	
	/* ��ȡMAC */
	{
		uint8_t mac[6];
		const uint8_t mac_0[6] = {0,0,0,0,0,0};
		
		ESP32_GetMac(mac);		
		
		if (memcmp(mac, g_tParam.WiFiMac, 6) != 0 && memcmp(mac, mac_0, 6) != 0)
		{
			memcpy(g_tParam.WiFiMac, mac, 6);
			SaveParam();
		}
	}

	ESP32_SetWiFiMode(1);		/* 1 = STA, 2 = SAP,  3 = SAP + Stationģʽ */	
	
	if (g_tParam.DHCPEn == 0)
	{
		/* DHCH = 0, ʹ�þ�̬IP */
		ESP32_SendAT("AT+CWDHCP_DEF=1,0");	
		ESP32_WaitResponse("OK\r\n", 300);

		ESP32_SetLocalIP(g_tParam.WiFiIPAddr, g_tParam.WiFiNetMask, g_tParam.WiFiGateway);	/* ���þ�̬IP */	
	}
	else
	{
		/* DHCH = 1, ʹ�ö�̬IP */
		ESP32_SendAT("AT+CWDHCP_DEF=1,1");	
		ESP32_WaitResponse("OK\r\n", 300);
	}
	
	wifi_led_joinap();	/* ����wifi�����п�����˸ */

	re = ESP32_JoinAP((char *)g_tParam.AP_SSID, (char *)g_tParam.AP_PASS, 20000);		
	if (re != 0)
	{
		g_tVar.HomeWiFiLinkOk = 0;
		g_tVar.RemoteTCPServerOk = 0;
		return 0;
	}
	if (re == 0)	/* ִ�гɹ� */
	{
		wifi_led_ok();
	}
	else	/* ִ��ʧ�� */
	{
		wifi_led_err();
	}		

	g_tVar.HomeWiFiLinkOk = 1;	/* �����ӵ�HOME WIFI */
	
	/* ��ӡ����IP */
	{
		char ip[20];
		char mac[64];
	
		ESP32_GetLocalIP(ip, mac);

		ESP32_QueryIPStatus();	/* ��ѯ��ǰIP����״̬ */			
	}
	
	ESP32_CIPMUX(1);		/* ���ö�����ģʽ */

//	ESP32_CloseTcpUdp(LINK_ID_UDP_SERVER);
	
	/* ����TCP������. */
	ESP32_CreateTCPServer(g_tParam.LocalTCPPort);
	
	/* ����UDP�����˿�, id = 0 */
	re = ESP32_CreateUDPServer(LINK_ID_UDP_SERVER, g_tParam.LocalTCPPort);
	
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: wifi_LinkSoftAP
*	����˵��: ����ESP32��AP.
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t wifi_LinkSoftAP(void)
{	
	uint8_t re;
	
	/* �رջ��Թ��ܣ��������͵��ַ���ģ�����践�� */	
	ESP32_SendAT("ATE0");		
	ESP32_WaitResponse("OK\r\n", 100);
	
	/* ��ȡMAC */
	{
		uint8_t mac[6];
		
		ESP32_GetMac(mac);		
		
		if (memcmp(mac, g_tParam.WiFiMac, 6) != 0)
		{
			memcpy(g_tParam.WiFiMac, mac, 6);
			SaveParam();
		}
	}

	ESP32_SetWiFiMode(1);		/* 1 = STA, 2 = SAP,  3 = SAP + Stationģʽ */	
	
	/* DHCH = 0, ʹ�þ�̬IP */
	ESP32_SendAT("AT+CWDHCP_DEF=1,0");	
	ESP32_WaitResponse("OK\r\n", 300);

	ESP32_SetLocalIP(g_tParam.WiFiIPAddr, g_tParam.WiFiNetMask, g_tParam.WiFiGateway);	/* ���þ�̬IP */	
	
	wifi_led_joinap();	/* ����wifi�����п�����˸ */

	re = ESP32_JoinAP((char *)g_tParam.AP_SSID, (char *)g_tParam.AP_PASS, 20000);		
	if (re != 0)
	{
		g_tVar.HomeWiFiLinkOk = 0;
		return 0;
	}
	if (re == 0)	/* ִ�гɹ� */
	{
		wifi_led_ok();
	}
	else	/* ִ��ʧ�� */
	{
		wifi_led_err();
	}		

	g_tVar.HomeWiFiLinkOk = 1;	/* �����ӵ�HOME WIFI */
	
	ESP32_CIPMUX(1);		/* ���ö�����ģʽ */

	ESP32_CloseTcpUdp(LINK_ID_UDP_SERVER);
	
	/* ����UDP�����˿�, id = 0 */
	re = ESP32_CreateUDPServer(LINK_ID_UDP_SERVER, g_tParam.LocalTCPPort);
	
	return 1;
}


/*
*********************************************************************************************************
*	�� �� ��: wifi_Poll
*	����˵��: ����main��ѭ��������WiFiָ��
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void wifi_Poll(void)
{	
	static int32_t s_rx_ok_time = 0;	/* ���һ���շ�ok��ʱ�� */
	uint8_t rx_byte;
	uint8_t re;		
		
	/* ���ݰ��շ� */
	/* ������֡����AT����֡���� */
	/* ���� +IPD ����֡ */
	
	while (1)
	{
		re = ESP32_GetChar(&rx_byte);		/* GetChar�����ڲ�����������¼���TCP CLOSE�¼� */
		if (re == 0)
		{
			/* 10��û�������շ���������·״̬ */
			if (bsp_CheckRunTime(s_rx_ok_time) > 10000)
			{	
				wifi_WatchDog();	/* wifi ���ӿ�������, ʧȥ���Ӻ��Զ����� */	
				
				s_rx_ok_time = bsp_GetRunTime();
			}						
			break;
		}
		else	/* �յ����� */
		{
			;
		}
	}
	
	if (g_tVar.WiFiRecivedIPD == 1)		/* �յ�UDP. TCP���ݰ� */
	{
		g_tVar.WiFiRecivedIPD = 0;
		
		s_rx_ok_time = bsp_GetRunTime();
		
		wifi_led_rx_data();		/* ����led��һ�� */

		return;
	}
	
	/* ������TCP����ر����ӣ�����������.  */
	{
		if (g_tVar.HomeWiFiLinkOk == 0)
		{
			wifi_state = WIFI_INIT;		/* ��������AP */
			return;
		}
	}		
}

/*
*********************************************************************************************************
*	�� �� ��: wifi_SendBuf
*	����˵��: ��ǰ���ӷ���һ������
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void wifi_SendBuf(uint8_t *str, uint16_t len) 
{	
	/* �ȴ�WIFI���� */
	while (wifi_state != WIFI_READY)
	{
		wifi_task();
	}
		
	if (link_id < 5)
	{
		if (ESP32_SendTcpUdp(link_id, str, len) == 0)
		{
			/* �������ʧ�ܣ�����Ҫ��λwifiģ�� */			
			wifi_LinkHomeAP();
			
			ESP32_SendTcpUdp(link_id, str, len);	/* V2.14���� 2018-12-12 */
		}
		else
		{
			wifi_led_tx_data();		/* tx����led��һ�� */
		}
	}	
}

/*
*********************************************************************************************************
*	�� �� ��: wifi_WatchDog
*	����˵��: WiFi���Ӽ�س��򡣼����Ƿ����ӵ�AP�� �����Ƿ����ӵ�TCP ������.
*	��    ��: �ޡ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void wifi_WatchDog(void)
{
	
	/*
	��ȷ��Ӧ�����
	
	(4451)=>AT+CIPSTATUS
	(4451)=>
	STATUS:3		<- 
	+CIPSTATUS:0,"UDP","255.255.255.255",8080,6200,0
	+CIPSTATUS:4,"TCP","192.168.1.3",9800,37299,0

	OK
	
	----------------------
	
	STATUS: ����
		2: ESP32 station ������AP�����IP��ַ
		3: �ѽ���TCP��UDP����
		4��ESP32 �ѶϿ���������
		5��δ���ӵ�AP
	
	
	----------------------- ��������λ�� ---
	AT+CIPSTATUS
	STATUS:5

	OK
	*/	

	/*
		SoftAP , ���ͻ��˽���ʱ��
				
		+STA_DISCONNECTED:"18:fe:34:d1:b0:07"
		+STA_CONNECTED:"18:fe:34:d1:b0:07"

	
	*/

	
	/* ֹͣͨ�ź�10�룬��ʼ��ʱɨ������״̬ */	
	#if 1
	{
		uint8_t buf[64];
		uint16_t rx_len;
		uint8_t net_status = 0;
		uint8_t link_ap_ok = 0;
		uint8_t i;

		for (i = 0; i < 3; i++)
		{
			ESP32_SendAT("AT+CIPSTATUS");
			while (1)
			{
				rx_len = ESP32_ReadLine((char *)buf, sizeof(buf), 100);
				if (rx_len == 0)	/* ��ʱû���յ�OK */
				{
					break;
				}				
				else if (rx_len > 7 && memcmp(buf, "STATUS:", 7) == 0)
				{
					/*  STATUS:3 */
					net_status = buf[7];
					
					if (net_status == '2' || net_status == '3' || net_status == '4')
					{
						link_ap_ok = 1;	/* ����AP ok */
					}
					else
					{
						if (g_tParam.APSelfEn == 1)	/* ��SoftAP */
						{
							;	/* ���ж�, ����UDP״̬�ж� */
						}
						else	/* ��STAվ�� */
						{
							link_ap_ok = 0;	/* û�����ӵ�AP */
						}
					}
				}
				else if (rx_len >= 18 && memcmp(buf, "+CIPSTATUS:0,\"UDP\"", 18) == 0)
				{
					if (g_tParam.APSelfEn == 1)
					{
						link_ap_ok = 1;
					}
				}					
//				else if (rx_len >= 18 && memcmp(buf, "+CIPSTATUS:4,\"TCP\"", 18) == 0)
//				{
//					/* +CIPSTATUS:4,"TCP","192.168.1.3",9800,37299,0 */
//					tcp_ok = 1;
//				}
				else if (rx_len >= 2 && memcmp(buf, "OK", 2) == 0)
				{
					/* ����Ҫ���ӵ�TCP������ */
					{
						if (link_ap_ok == 1)
						{
							i = 4;	/* ֻҪ��1�γɹ����˳�for */
						}							
					}
					break;
				}
			}	
		}
		
		if (link_ap_ok == 0)
		{
			wifi_LinkHomeAP();	/* �������ȴ�����AP */
		}
		else
		{
			;
		}
	}
	#endif
}

/*
*********************************************************************************************************
*	�� �� ��: wifi_DebugATCommand
*	����˵��: ����485�յ���MODBUS ���ݰ��������ATָ���͸����WiFiģ��.
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void wifi_DebugATCommand(uint8_t *_rxbuf, uint16_t _rxlen)
{
	if (_rxbuf[0] != 'A' || _rxbuf[1] != 'T')
	{
		return;		/* ����ATָ�͸�� */
	}
	
	if (_rxbuf[_rxlen - 1] != 0x0A)
	{
		return;		/* ĩβ���ǻس����� */
	}
	
	/* 120���ڿ���͸�� */
	
	if (g_tVar.WiFiDebugEn == 1)
	{
	//	g_tVar.WiFiDebugTime = bsp_GetRunTime();
		
		comSendBuf(COM_DEBUG, _rxbuf, _rxlen);
		
		/* ��ȫ��������ת����WiFiģ�� */
		comSendBuf(COM_ESP32, _rxbuf, _rxlen);
	}
}

#if 0
/********************************************** �������� **********************/


char Rx_Protocol[STRINGLEN] = {0};

static uint8_t s_sethomewifi_state = 0;
static uint8_t s_sethomewifi_mode = 0;

static uint8_t s_setserver_state = 0;
static char s_setserver_ip[48];
static uint16_t s_setserver_port;

uint8_t g_wifi_tx_halt = 0;		/* wifi����,��Ҫ������ ����ʧ������Ϊ1 */

void sethomewifi_loop(void);
void setserver_loop(void);
void WifiWatchdog(void);

wifi���ڵȴ���������.... 
void wifi_led_smartlink(void)
{
	PERIOD_Start(&g_tWiFiLed, 50, 100, 0);	/* ����wifi�����п�����˸ */
}


/*
*********************************************************************************************************
*	�� �� ��: putcharA
*	����˵��: ��ǰ���ӷ���һ���ַ�����0������
*	��    ��: str : 0�������ַ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void putcharA(char *str) 
{
	uint16_t len;
	
	len = strlen(str);
	if (len > STRINGLEN)
	{
		len = STRINGLEN;
	}
	
	putcharbuf(str, len);
}


// ���ڽ����жϷ�֮1�� ����¯�ӿ���
void get_char(void)
{
	uint8_t r = 0;

	//uint8_t r = UART3_ReceiveData8();  // �ѽ��յ������ݸ��Ƹ�����
	//UART3_ClearFlag(UART3_FLAG_RXNE);
		
	if (Rx_len >= STRINGLEN)
	{
		memset(Rx_String,0,STRINGLEN);
		Rx_len = 0;
	}

	//if(r == 'U')
	//{
	//	KMemset(Rx_String,0,Rx_len);
	//	Rx_len = 0;
	//	Rx_String[Rx_len] = r;
	//	Rx_len++;
	//}
	//else 
	if(r == '!')
	{
		ParseString();
		memset(Rx_String,0,Rx_len);
		Rx_len = 0;
	}
	else
	{
		Rx_String[Rx_len] = r;
		Rx_len++;
	}

}


// �����жϷ������2 -  ���ں�Զ�̷�����ͨ��
void recvdata(void)
{
	uint8_t r;// = UART3_ReceiveData8();  // �ѽ��յ������ݸ��Ƹ�����
	//UART3_ClearFlag(UART3_FLAG_RXNE);

	if(Rx_len >= STRINGLEN)
	{
		memset(Rx_String,0,STRINGLEN);
		Rx_len = 0;
	}

	Rx_String[Rx_len] = r;
	Rx_len++;
	if(Rx_len >= 8)
	{
		//if(r == '1')
		{
			if(strncmp(&Rx_String[Rx_len - 8],GMGHeader,8) == 0)    // ��ͷ�ŵ���ǰ��
			{
				//memcpy(Rx_String,Rx_String + Rx_len - 8,8);
				Rx_String[0] = Rx_String[Rx_len - 8];
				Rx_String[1] = Rx_String[Rx_len - 7];
				Rx_String[2] = Rx_String[Rx_len - 6];
				Rx_String[3] = Rx_String[Rx_len - 5];
				Rx_String[4] = Rx_String[Rx_len - 4];
				Rx_String[5] = Rx_String[Rx_len - 3];
				Rx_String[6] = Rx_String[Rx_len - 2];
				Rx_String[7] = Rx_String[Rx_len - 1];
				Rx_len = 8;
			}
		}

		if(strncmp(&Rx_String[Rx_len - 4],GMGTail,4) == 0) 
		{
			int index = backfindGMGHead(Rx_String,Rx_len);
			
			if(index != -1)
			{
				memcpy(Rx_Protocol,Rx_String + index,Rx_len - index);
				ParseProtocol();
			}
			memset(Rx_String,0,STRINGLEN);
			Rx_len = 0;

		}
	}

	//if(UART3_GetITStatus(UART3_IT_RXNE) != RESET)
	//{
	//	UART3_ClearITPendingBit(UART3_IT_RXNE); 
	//}

}

/* */
void factory_atoupdate(void) 
{
	uint16_t i; 
	
	printf("\r\n��factory_atoupdate��\r\n");
	
	DispUPD();
	for(i = 0;i < 30;i++)
	{
		Wait1s();
		if((i & 0x01) == 1)
		{
			bsp_LedOn();   
		}
		else
		{
			bsp_LedOff();   
		}
	}
        
	setserver("dlink_lyq","123456789a","50.63.156.227","5533");
        
   // setserver("hans203","123456887af","221.235.53.92","6699");
	for(i = 0;i < 20;i++)
	{
		Wait1s();
		if((i & 0x01) == 1)
		{
			bsp_LedOn();   
		}
		else
		{
			bsp_LedOff();   
		}
	}

	iWifiCtrl = WIFI_CMD_SOFT_UPDATE;
}


/* UH ָ��ֻ�����¯�ӣ�����¯�Ӽ�ͥAP�����ֺ�����  */
/*
*********************************************************************************************************
*	�� �� ��: sethomewifi
*	����˵��: ����WiFiΪHome Wifiģʽ����������UDP�����TCP����������Ҫ�õġ�
*	��    ��: mode �� 0��ʾTCP���� 1��ʾUDP����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void sethomewifi(int mode,char* ssid,char* password)
{	
	uint8_t re;
	
	printf("\r\n������Ϊ home wifi ģʽ��\r\n");
	
	ESP32_PowerOn();	/* WIFIģ����縴λһ�� */
	
	/* �л�ģʽǰ����ѯ�µ�ǰ״̬ */
	{
		ESP32_QueryIPStatus();	/* ��ѯ��ǰIP����״̬ */	
	}
	
	ESP32_SetWiFiMode(1);		/* 1 = STA, 2 = SAP,  3 = SAP + Stationģʽ */
	
	/* �����ͥAP */
	printf("\r\n������Home AP...��\r\n");
	
	wifi_led_joinap();	/* ����wifi�����п�����˸ */
	re = ESP32_JoinAP(g_tParam2.wifi_ssid, g_tParam2.wifi_password, 20000);


	/* ��ӡ����IP */
	{
		char ip[20];
		char mac[64];
	
		ESP32_GetLocalIP(ip, mac);

		ESP32_QueryIPStatus();	/* ��ѯ��ǰIP����״̬ */		
	}
	
	/* ����DHCH = 1 , �������дflash���� */
	//SP8266_SendAT("AT+CWDHCP=1,1");	
	//ESP32_WaitResponse("OK\r\n", 1000);
	
	ESP32_CIPMUX(1);		/* ���ö�����ģʽ */

	if (mode == 0)
	{
		/* ����TCP������ */
		printf("\r\n������TCP������.��\r\n");
		re  = ESP32_CreateTCPServer(8080);		
	}
	else
	{
		/* ����UDP�����˿� */
		printf("\r\n������UDP�����˿ڡ�\r\n");
		re  = ESP32_CreateUDPServer(0, 8080);
	}
	
	if (re == 1)	/* ִ�гɹ� */
	{
		wifi_led_ok();
	}
	else	/* ִ��ʧ�� */
	{
		wifi_led_err();
	}	
}

/*
*********************************************************************************************************
*	�� �� ��: sethomewifi_start
*	����˵��: ����WiFiΪHome Wifiģʽ����������UDP�����TCP����������Ҫ�õġ� 
*			 �����������sethomewifi_loop����ʹ��
*	��    ��: mode �� 0��ʾTCP���� 1��ʾUDP���� 2��ʾͬʱ����2������ 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void sethomewifi_start(int mode)
{	
	s_sethomewifi_state = 1;
	s_sethomewifi_mode = mode;

	g_tParam2.CommState = WIFI_STATE_HOMEWIFI;		/* ����WIFIģ��ΪSTAģʽ  */
		
	SaveParam2();		
}

void sethomewifi_loop(void)
{	
	static int32_t s_last_time = 0;
	static uint8_t s_err_cout = 0;
	
	switch (s_sethomewifi_state)
	{
		case 0:			/* ִ����ϣ�����״̬ */
			break;

		case 1:			/* ����һ�� */
			s_err_cout = 0;		/* �������� */
			s_sethomewifi_state++;
			break;
		
		case 2:		/* <------ ����ʱ��ѭ�����  */
			printf("\r\n������Ϊ home wifi ģʽ��\r\n");
		
			ESP32_PowerOn();	/* WIFIģ����縴λһ�� */

			s_sethomewifi_state++;
			break;
	
		case 3:
			/* �л�ģʽǰ����ѯ�µ�ǰ״̬ */
			{
				ESP32_QueryIPStatus();	/* ��ѯ��ǰIP����״̬ */	
			}						
			ESP32_SetWiFiMode(1);		/* 1 = STA, 2 = SAP,  3 = SAP + Stationģʽ */		

			s_sethomewifi_state++;
			break;
			
		case 4:
			/* �����ͥAP */
			printf("\r\n������Home AP...��\r\n");	
			wifi_led_joinap();	/* ����wifi�����п�����˸ */
			ESP32_PT_JoinAP(g_tParam2.wifi_ssid, g_tParam2.wifi_password, 30000);	
			
			s_sethomewifi_state++;
			break;

		case 5:	/* �����״̬�ȴ�APӦ�� 30s��ʱ */	
			{
				uint8_t re;
				
				re = ESP32_PT_WaitResonse();
				if (re == PT_NULL)	/* �ղ�������Ҫ�����ȴ� */
				{
					;
				}
				else if (re == PT_OK)	/* �ɹ�ִ�� */
				{
					s_sethomewifi_state++;		/* ִ�гɹ� */
				}
				else if (re == PT_ERR || re == PT_TIMEOUT)
				{					
					wifi_led_err();	/* ִ��ʧ�� */
					
					s_sethomewifi_state = 10;	/* ʧ�� */
				}				
			}
			break;
		
		case 6:
			/* ��ӡ����IP */
			{
				char ip[20];
				char mac[64];
			
				ESP32_GetLocalIP(ip, mac);

				ESP32_QueryIPStatus();	/* ��ѯ��ǰIP����״̬ */		
			}
			
			/* ����DHCH = 1 , �������дflash���� */
			//SP8266_SendAT("AT+CWDHCP=1,1");	
			//ESP32_WaitResponse("OK\r\n", 1000);
			
			ESP32_CIPMUX(1);		/* ���ö�����ģʽ */

			{
				uint8_t re;
				
				if (s_sethomewifi_mode == 0)
				{
					/* ����TCP������ */
					printf("\r\n������TCP������.��\r\n");
					re  = ESP32_CreateTCPServer(8080);		
				}
				else if (s_sethomewifi_mode == 1)
				{
					/* ����UDP�����˿� */
					printf("\r\n������UDP�����˿ڡ�\r\n");
					re  = ESP32_CreateUDPServer(0, 8080);
				}
				else   /* 3 ��ʾͬʱ����UDP��TCP */
				{
					/* ����UDP�����˿� */
					printf("\r\n������UDP�����˿ڡ�\r\n");
					re  = ESP32_CreateUDPServer(0, 8080);

					/* ����TCP������ */
					printf("\r\n������TCP������.��\r\n");
					re  = ESP32_CreateTCPServer(8080);						
				}				
				
				if (re == 1)		/* ִ�гɹ� */
				{
					wifi_led_ok();
					
					s_sethomewifi_state = 0;	/* �ɹ����� */
				}
				else				/* ִ��ʧ�� */
				{
					wifi_led_err();
					
					s_sethomewifi_state = 10;	/* ʧ�� */
				}								
			}				
			break;

		case 10:			/* ִ��ʧ��, ��Ϣ1������� */
			if (++s_err_cout > 10)
			{
				s_sethomewifi_state = 0;
			}
			else
			{
				s_sethomewifi_state = 11;
				s_last_time = bsp_GetRunTime();	/* ���浱ǰ����ʱ�� */	
			}
			break;
		
		case 11:
			if (bsp_CheckRunTime(s_last_time) > 1000)
			{
				//s_sethomewifi_state = 2;	2�� Ӳ����λWIFIģ��
				s_sethomewifi_state = 4;
			}
			break;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: status_SmartLink
*	����˵��: ������������״̬����ʱwifiģ����STAģʽ���ֻ�ͨ��APP����wifiģ�� AP�����ֺ����롣
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void status_SmartLink(void)
{
	uint16_t time = 300;
	char ssid[SSID_MAX_LEN + 1];
	char password[PASSWORD_MAX_LEN + 1];
	uint8_t ucKey;
	uint8_t fQuit = 0;
	
	printf("\r\n��������������״̬��\r\n");
	
	wifi_led_smartlink();	/* �Զ����� */

	ESP32_PowerOn();	/* WIFIģ����縴λһ�� */
	
	LED8_DispStr("CFG");
	
	bsp_StartAutoTimer(0, 1000);
	
	ESP32_PT_SmartStrat(0);	/* �����������ӡ� 0 = �����ż���  1 = ESP-TOUCH   2 = AIR-KISS */
	
	while (fQuit == 0)		
	{
		/* �ȴ��ֻ����� */
		if (ESP32_PT_SmartWait(ssid, password) == PT_OK)
		{
			LED8_DispStr("SUCC");
			BEEP_Start(5, 10, 2);	/* ���óɹ�����2�� */
			
			/* ���óɹ���ģ����Զ��˳�smart link ģʽ */
			//memcpy(g_tParam2.wifi_ssid, ssid, strlen(ssid));
			//memcpy(g_tParam2.wifi_password, password, strlen(password));
			strncpy(g_tParam2.wifi_ssid, ssid, SSID_MAX_LEN);
			strncpy(g_tParam2.wifi_password, password, PASSWORD_MAX_LEN);   			

			g_tParam2.CommState = WIFI_STATE_HOMEWIFI;	   		
			
			SaveParam2();	/* ������� */

			bsp_DelayMS(1000);	/* �ӳ�1�� */
			
			/* ���óɹ� */
			sethomewifi_start(2);	/* ���ӵ�HOME WIFI , UDP����  ---> UDP + TCPͬʱ���� */
			break;
		}
		
		if (bsp_CheckTimer(0))
		{
			if (time > 0)
			{
				time--;
				//LED8_DispNumber(time, 0);
				
				if (time == 0)
				{
					break;
				}
			}
		}
		
		ucKey = bsp_GetKey();
		switch (ucKey)
		{
			case KEY_PWR_DOWN:	/* ��Դ������ */
				fQuit = 1;
				break;

			case KEY_INC_DOWN:	/* ��1������ */
			case KEY_DEC_DOWN:	/* ��1������ */
				ESP32_PT_SmartStrat(0);
				time = 300;
				break;			
		}
	}
	bsp_StopTimer(0);
}

/*
*********************************************************************************************************
*	�� �� ��: setserver
*	����˵��: ���ݸ�����ssid���������ӵ�home ap�� Ȼ�����Զ��TCP������������һ��TCP�ͻ������ӡ� ����ʽ��
*	��    ��: ssid, [assword, 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void setserver(char* ssid, char* password, char* serverip, char* port)
{
	uint8_t re;
	
	printf("\r\n�����ӵ�Home WiFi������Զ��TCP��������\r\n");

	ESP32_PowerOn();	/* WIFIģ����縴λһ�� */
	
	ESP32_SetWiFiMode(1);		/* 1 = STA, 2 = SAP,  3 = SAP + Stationģʽ */
	
	/* �����ͥAP -- ��ʱESP32 ���ܸ�λ���� */
	wifi_led_joinap();	/* ����wifi�����п�����˸ */
	re = ESP32_JoinAP(ssid, password, 20000);


	ESP32_CIPMUX(1);		/* ���ö�����ģʽ */
	
	/* ��ӡ����IP */
	{
		char ip[20];
		char mac[64];
	
		ESP32_GetLocalIP(ip, mac);
		
		ESP32_QueryIPStatus();	/* ��ѯ��ǰIP����״̬ */
	}
	
	//ESP32_CIPMUX(1);		/* ���ö�����ģʽ */

	/* ���ӵ�TCP������ */
	printf("\r\n�����ӵ�Զ��TCP������.��\r\n");
	re = ESP32_LinkTCPServer(0, serverip, str_to_int(port));
	if (re == 1)	/* ִ�гɹ� */
	{
		wifi_led_ok();
	}
	else	/* ִ��ʧ�� */
	{
		wifi_led_err();
	}	
	link_id = 0;
}

/*
*********************************************************************************************************
*	�� �� ��: setserver_start
*	����˵��: ���ӵ�Զ�̷�����,TCP����˿�.  ¯����TCP�ͻ���
*			 �����������sethomewifi_loop����ʹ��
*	��    ��: serverip : ������IP��ַ
*			  port �� Զ�̷����TCP�˿�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void setserver_start(char * serverip, char * port)
{	
	s_setserver_state = 1;
	strncpy(s_setserver_ip, serverip, sizeof(s_setserver_ip));
	s_setserver_port = str_to_int(port);	
	
	printf("\r\n�����ӵ�Home WiFi������Զ��TCP��������\r\n");	
}

void setserver_start2(char * serverip, char * port)
{	
	s_setserver_state = 99;
	strncpy(s_setserver_ip, serverip, sizeof(s_setserver_ip));
	s_setserver_port = str_to_int(port);	
	
	printf("\r\n�����ӵ�Home WiFi������Զ��TCP��������\r\n");	
}

/* ����ʧ���������� Զ�̷������������ڳ��������е��� */
void setserver_reconnect(void)
{			
	printf("\r\n��ʧ�ܺ�����������Զ��TCP��������\r\n");	
	ESP32_LinkTCPServer(2, s_setserver_ip, s_setserver_port);
}



/* ��������ִ����� */
uint8_t setserver_finished(void)
{
	if (s_setserver_state == 0)
	{
		return 1;
	}
	return 0;
}

void setserver_loop(void)
{	
	static int32_t s_last_time = 0;
	static uint8_t s_err_cout = 0;
	
	switch (s_setserver_state)
	{
		case 0:			/* ִ����ϣ�����״̬ */
			break;

		case 1:			/* ����һ�� */
			s_err_cout = 0;		/* �������� */
			s_setserver_state = 6;	/* ����λWIFIģ�� */
			break;

		case 99:
			s_err_cout = 0;		/* �������� */
			s_setserver_state = 2;	/* ��λwifiģ�� */	
			break;
		
		case 2:		/* <------ ����ʱ��ѭ�����  */	
			ESP32_PowerOn();	/* WIFIģ����縴λһ�� */

			s_setserver_state++;
			break;
	
		case 3:
			/* �л�ģʽǰ����ѯ�µ�ǰ״̬ */
			{
				ESP32_QueryIPStatus();	/* ��ѯ��ǰIP����״̬ */	
			}						
			ESP32_SetWiFiMode(1);		/* 1 = STA, 2 = SAP,  3 = SAP + Stationģʽ */		

			s_setserver_state++;
			break;
			
		case 4:
			/* �����ͥAP */
			printf("\r\n������Home AP...��\r\n");	
			wifi_led_joinap();	/* ����wifi�����п�����˸ */
			ESP32_PT_JoinAP(g_tParam2.wifi_ssid, g_tParam2.wifi_password, 30000);	
			
			s_setserver_state++;
			break;

		case 5:	/* �����״̬�ȴ�APӦ�� 30s��ʱ */	
			{
				uint8_t re;
				
				re = ESP32_PT_WaitResonse();
				if (re == PT_NULL)	/* �ղ�������Ҫ�����ȴ� */
				{
					;
				}
				else if (re == PT_OK)	/* �ɹ�ִ�� */
				{
					s_setserver_state++;		/* ִ�гɹ� */
				}
				else if (re == PT_ERR || re == PT_TIMEOUT)
				{					
					wifi_led_err();	/* ִ��ʧ�� */
					
					s_setserver_state = 10;	/* ʧ�� */
				}				
			}
			break;
		
		case 6:
			/* ��ӡ����IP */
			{
				char ip[20];
				char mac[64];
			
				ESP32_GetLocalIP(ip, mac);

				ESP32_QueryIPStatus();	/* ��ѯ��ǰIP����״̬ */		
			}
			
			/* ����DHCH = 1 , �������дflash���� */
			//SP8266_SendAT("AT+CWDHCP=1,1");	
			//ESP32_WaitResponse("OK\r\n", 1000);
			
			ESP32_CIPMUX(1);		/* ���ö�����ģʽ */

			{
				uint8_t re;
				
				printf("\r\n�����ӵ�Զ��TCP������.��\r\n");
				re = ESP32_LinkTCPServer(2, s_setserver_ip, s_setserver_port);  // ZHG ʹ������ID2��  id = 1��8080 tcp
				if (re == 1)	/* ִ�гɹ� */
				{
					wifi_led_ok();
				}
				else	/* ִ��ʧ�� */
				{
					wifi_led_err();
				}	
				link_id = 2;
				
				if (re == 1)		/* ִ�гɹ� */
				{
					wifi_led_ok();
					
					s_setserver_state = 0;	/* �ɹ����� */
				}
				else				/* ִ��ʧ�� */
				{
					wifi_led_err();
					
					s_setserver_state = 10;	/* ʧ�� */
				}								
			}				
			break;

		case 10:			/* ִ��ʧ��, ��Ϣ1�������10�� */
			if (++s_err_cout > 10)
			{
				s_setserver_state = 0;
			}
			else
			{
				s_setserver_state = 11;
				s_last_time = bsp_GetRunTime();	/* ���浱ǰ����ʱ�� */	
			}
			break;
		
		case 11:
			if (bsp_CheckRunTime(s_last_time) > 1000)		/* 5������� */
			{
				//s_setserver_state = 2;
				s_setserver_state = 4;
			}
			break;
	}
}

	
#endif

/********************************************************/

