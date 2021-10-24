/*******************************************************
**	CPU: STC11L08XE
**	���񣺾���Ƶ����config.h�¸���,ֻ������12MHz��22.1184MHz
**	�����ʣ�9600 bit/S
**  ��ʱ����ģʽ�� ��ʶ��ʱ��˵��Ѱ��������Ѻ����15���ڲ���������ʶ�����˳�����
/*********************************************************/
#include "config.h"
/************************************************************************************/
//	nAsrStatus ������main�������б�ʾ�������е�״̬������LD3320оƬ�ڲ���״̬�Ĵ���
//	LD_ASR_NONE:		��ʾû������ASRʶ��
//	LD_ASR_RUNING��		��ʾLD3320������ASRʶ����
//	LD_ASR_FOUNDOK:		��ʾһ��ʶ�����̽�������һ��ʶ����
//	LD_ASR_FOUNDZERO:	��ʾһ��ʶ�����̽�����û��ʶ����
//	LD_ASR_ERROR:		��ʾһ��ʶ��������LD3320оƬ�ڲ����ֲ���ȷ��״̬
/***********************************************************************************/
uint8 idata nAsrStatus = 0;
void MCU_init();
void ProcessInt0(); //ʶ������
void delay(unsigned long uldata);
void User_handle(uint8 dat); //�û�ִ�в�������
void Delay200ms();
int over_time;
uint8_t Run_Mode = 0;
uint8_t G0_flag = DISABLE; //���б�־��ENABLE:���С�DISABLE:��ֹ����
sbit STC_LED = P4 ^ 2;	   //�ź�ָʾ��
sbit STM32_SIG = P3 ^ 3;
sbit STM32_RST = P1 ^ 5;
sbit SW0 = P1 ^ 0;
sbit SW1 = P1 ^ 1;
sbit SW2 = P1 ^ 2;
sbit SW3 = P1 ^ 3;
sbit LED = P1 ^ 7;
sbit BEEP = P1 ^ 6;
/***********************************************************
* ��    �ƣ� void  main(void)
* ��    �ܣ� ������	�������
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
**********************************************************/
void main(void)
{
	uint8 idata nAsrRes;
	uint8 i = 0;
	MCU_init();
	LD_Reset();
	UartInit();				  /*���ڳ�ʼ��*/
	nAsrStatus = LD_ASR_NONE; //��ʼ״̬��û������ASR
	if (SW3 == 1)
		Run_Mode += 8;
	if (SW2 == 1)
		Run_Mode += 4;
	if (SW1 == 1)
		Run_Mode += 2;
	if (SW0 == 1)
		Run_Mode += 1;
	UARTSendStr("ϵͳ��ʼ���ɹ�,ģʽΪ");
	UARTSendByte(Run_Mode / 10 + '0');
	UARTSendByte(Run_Mode % 10 + '0');
	UARTSendStr("\r\n");
	while (1)
	{
		if (STM32_SIG == 0)
		{
			BEEP = 1;
			LED = 1;
			UARTSendStr("���յ�STM32 �����ź�\r\n");
		}
		if (over_time >= 500)
		{
			over_time = 0;
			G0_flag = DISABLE;
			STC_LED = 1;
			UARTSendStr("ʶ��ʱ\r\n");
		}
		switch (nAsrStatus)
		{
		case LD_ASR_RUNING:
		case LD_ASR_ERROR:
			break;
		case LD_ASR_NONE:
		{
			nAsrStatus = LD_ASR_RUNING;
			if (RunASR() == 0) /*	����һ��ASRʶ�����̣�ASR��ʼ����ASR��ӹؼ��������ASR����*/
			{
				nAsrStatus = LD_ASR_ERROR;
			}
			break;
		}
		case LD_ASR_FOUNDOK: /*	һ��ASRʶ�����̽�����ȥȡASRʶ����*/
		{
			nAsrRes = LD_GetResult(); /*��ȡ���*/
			User_handle(nAsrRes);	  //�û�ִ�к���
			nAsrStatus = LD_ASR_NONE;
			break;
		}
		case LD_ASR_FOUNDZERO:
		default:
		{
			nAsrStatus = LD_ASR_NONE;
			break;
		}
		} // switch
	}	  // while
}

/***********************************************************
* ��    �ƣ� void MCU_init()
* ��    �ܣ� ��Ƭ����ʼ��
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
**********************************************************/
void MCU_init()
{
	P0 = 0xff;
	P1 = 0x00;
	P2 = 0xff;
	P3 = 0xff;
	P4 = 0xff;
	
	P1M1 = 0x00;
	P1M0 = 0xE0;
	P3M1 &= 0111;
	P3M0 &= 0111;
	STM32_SIG = 1;
	SW3 = SW2 = SW1 = SW0 = 1;
	LED = 0;
	BEEP = 0;
	STC_LED = 1;
	STM32_RST = 0;
	Delay200ms();
	STM32_RST = 1;

	AUXR &= 0x7F; //��ʱ��ʱ��12Tģʽ
	TMOD |= 0x01; //���ö�ʱ��ģʽ
#if (FOSC == 22118400L)
	TL0 = 0x00; //���ö�ʱ��ֵ
	TH0 = 0x28; //���ö�ʱ��ֵ
#else
	TL0 = 0xD0;
	TH0 = 0x8A;
#endif
	TF0 = 0; //���TF0��־
	TR0 = 1; //��ʱ��0��ʼ��ʱ
	ET0 = 1;

	LD_MODE = 0; //	����MD�ܽ�Ϊ�ͣ�����ģʽ��д
	IE0 = 1;
	EX0 = 1;
	EA = 1;
	WDT_CONTR = 0x3D;
}
/***********************************************************
* ��    �ƣ�	��ʱ����
* ��    �ܣ�
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
**********************************************************/
void Delay200us() //@22.1184MHz
{
#if (FOSC == 22118400L)
	unsigned char i, j;
	_nop_();
	_nop_();
	i = 5;
	j = 73;
	do
	{
		while (--j)
			;
	} while (--i);
#else
	unsigned char i, j;

	_nop_();
	_nop_();
	i = 3;
	j = 81;
	do
	{
		while (--j)
			;
	} while (--i);
#endif
}

void delay(unsigned long uldata)
{
	unsigned int j = 0;
	unsigned int g = 0;
	while (uldata--)
		Delay200us();
}

void Delay200ms() //@22.1184MHz
{
#if (FOSC == 22118400L)
	unsigned char i, j, k;
	i = 17;
	j = 208;
	k = 27;
	do
	{
		do
		{
			while (--k)
				;
		} while (--j);
	} while (--i);
#else
	unsigned char i, j, k;

	i = 10;
	j = 31;
	k = 147;
	do
	{
		do
		{
			while (--k)
				;
		} while (--j);
	} while (--i);
#endif
}

/***********************************************************
* ��    �ƣ� �жϴ�����
* ��    �ܣ�
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
**********************************************************/
void ExtInt0Handler(void) interrupt 0
{
	ProcessInt0();
}
/***********************************************************
* ��    �ƣ��û�ִ�к���
* ��    �ܣ�ʶ��ɹ���ִ�ж������ڴ˽����޸�
* ��ڲ����� ��
* ���ڲ�������
* ˵    ����
**********************************************************/
void User_handle(uint8 dat)
{
	uint8_t i = 0;
	if (0 == dat)
	{
		G0_flag = ENABLE;
		over_time = 0;
		UARTSendStr("�ɹ�ʶ��һ������\r\n");
		STC_LED = 0;
		for (i = 0; i < 3; i++)
		{
			LED = 0;
			Delay200ms();
			Delay200ms();
			LED = 1;
			Delay200ms();
			Delay200ms();
		}
	}
	else if (ENABLE == G0_flag)
	{
		LED = 1;
		BEEP = 1;
		STC_LED = 1;
		switch (dat)
		{
		case CODE_1:
			UARTSendStr("Կ��\r\n");
			if (Run_Mode == 0)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("ģ����Ӧ\r\n");
			}
			break;
		case CODE_2:
			UARTSendStr("Ǯ��\r\n");
			if (Run_Mode == 1)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("ģ����Ӧ\r\n");
			}
			break;
		case CODE_3:
			UARTSendStr("ң����\r\n");
			if (Run_Mode == 2)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("ģ����Ӧ\r\n");
			}
			break;
		case CODE_4:
			UARTSendStr("У԰��\r\n");
			if (Run_Mode == 3)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("ģ����Ӧ\r\n");
			}
			break;
		case CODE_5:
			UARTSendStr("�ֻ�\r\n");
			if (Run_Mode == 4)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("ģ����Ӧ\r\n");
			}
			break;
		case CODE_6:
			UARTSendStr("���\r\n");
			if (Run_Mode == 5)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("ģ����Ӧ\r\n");
			}
			break;
		case CODE_7:
			UARTSendStr("�ſ�\r\n");
			if (Run_Mode == 6)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("ģ����Ӧ\r\n");
			}
			break;
		case CODE_8:
			UARTSendStr("U��\r\n");
			if (Run_Mode == 7)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("ģ����Ӧ\r\n");
			}
			break;
		case CODE_9:
			UARTSendStr("����\r\n");
			if (Run_Mode == 8)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("ģ����Ӧ\r\n");
			}
			break;
		case CODE_10:
			UARTSendStr("ҩ��\r\n");
			if (Run_Mode == 9)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("ģ����Ӧ\r\n");
			}
			break;
		case CODE_11:
			UARTSendStr("����\r\n");
			if (Run_Mode == 10)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("ģ����Ӧ\r\n");
			}
			break;
		case CODE_12:
			UARTSendStr("���±�\r\n");
			if (Run_Mode == 11)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("ģ����Ӧ\r\n");
			}
			break;
		case CODE_13:
			UARTSendStr("��Ǯ��\r\n");
			if (Run_Mode == 12)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("ģ����Ӧ\r\n");
			}
			break;
		case CODE_14:
			UARTSendStr("����\r\n");
			if (Run_Mode == 13)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("ģ����Ӧ\r\n");
			}
			break;
		case CODE_15:
			UARTSendStr("Ѫѹ��\r\n");
			if (Run_Mode == 14)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("ģ����Ӧ\r\n");
			}
			break;
		default:
			UARTSendStr("ʶ�����\r\n");
			break;
		}
	}
	else
	{
		UARTSendStr("�ȶ���һ������\r\n");
	}
}

void tm0_isr() interrupt 1
{
#if (FOSC == 22118400L)
	TL0 = 0x00; //���ö�ʱ��ֵ
	TH0 = 0x28; //���ö�ʱ��ֵ
#else
	TL0 = 0xD0;
	TH0 = 0x8A;
#endif
	WDT_CONTR = 0x3D;
	if (G0_flag == ENABLE)
	{
		over_time++;
	}
}
