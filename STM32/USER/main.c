/**
 * �޸�Keil���� system_stm32f10x.h stm32f10x���þ���Ƶ��
 */
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "string.h"
unsigned long Max_Timer = 50;
unsigned long Timer = 0;
int main(void)
{
	delay_init();									//��ʱ������ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);								//���ڳ�ʼ��Ϊ9600
	LED_Init();										//LED�˿ڳ�ʼ��
	while (1)
	{
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11))
		{
			if (Max_Timer != 50)
			{
				printf("����������\r\n");
				Max_Timer = 50;
			}
		}
		else
		{
			if (Max_Timer != 20)
			{
				printf("�����ѶϿ�\r\n");
				Max_Timer = 20;
			}
		}
		Timer++;
		if (Timer > Max_Timer)
		{
			Timer = 0;
			if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_1))
			{
				GPIO_ResetBits(GPIOA, GPIO_Pin_1);
			}
			else
			{
				GPIO_SetBits(GPIOA, GPIO_Pin_1);
			}
		}
		delay_ms(1);
	}
}
