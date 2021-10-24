/**
 * 修改Keil配置 system_stm32f10x.h stm32f10x设置晶振频率
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
	delay_init();									//延时函数初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);								//串口初始化为9600
	LED_Init();										//LED端口初始化
	while (1)
	{
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11))
		{
			if (Max_Timer != 50)
			{
				printf("蓝牙已连接\r\n");
				Max_Timer = 50;
			}
		}
		else
		{
			if (Max_Timer != 20)
			{
				printf("蓝牙已断开\r\n");
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
