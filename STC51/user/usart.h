#ifndef __USART_H
#define __USART_H

void UartInit(void);             //���ڳ�ʼ��
void UARTSendByte(uint8_t DAT); //���ڷ���һ�ֽ�����
void UARTSendStr(uint8_t *DAT); //��ӡ�����ַ�������

#endif
