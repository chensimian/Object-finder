
#include "config.h"
uint32_t baud = 9600; //UART baudrate

uint8_t RX_BUF[50];
uint8_t NUM_1 = 0;
/************************************************************************
�� �� ���� ���ڳ�ʼ��
���������� STC10L08XE ��Ƭ�����ڳ�ʼ������
���غ����� none
����˵���� none
**************************************************************************/
void UartInit(void)
{
	SCON = 0x50;						  //8-bit variable UART
	TMOD |= 0x20;						  //Set Timer1 as 8-bit auto reload mode
	TH1 = TL1 = -(FOSC / 12 / 32 / baud); //Set auto-reload vaule
	TR1 = 1;							  //Timer1 start run
	ES = 1;								  //Enable UART interrupt
	EA = 1;								  //Open master interrupt switch
}
/************************************************************************
���������� 	���ڷ���һ�ֽ�����
��ڲ�����	DAT:�����͵�����
�� �� ֵ�� 	none
����˵����	none
**************************************************************************/
void UARTSendByte(uint8_t DAT)
{
	ES = 0;
	TI = 0;
	SBUF = DAT;
	while (TI == 0)
		;
	TI = 0;
	ES = 1;
}
/************************************************************************
���������� ���ڷ����ַ�������
��ڲ����� 	*DAT���ַ���ָ��
�� �� ֵ�� none
����˵���� API ���ⲿʹ�ã�ֱ�ۣ�
**************************************************************************/
void UARTSendStr(uint8_t *DAT)
{
	while (*DAT)
	{
		UARTSendByte(*DAT++);
	}
}

void Uart_Isr() interrupt 4 using 1
{
	if (RI)
	{
		RI = 0;
		RX_BUF[NUM_1] = SBUF;
		NUM_1++;
		if (NUM_1 >= 49)
			NUM_1 = 0;
	}
}