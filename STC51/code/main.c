/*******************************************************
**	CPU: STC11L08XE
**	晶振：晶振频率在config.h下更改,只适用于12MHz和22.1184MHz
**	波特率：9600 bit/S
**  延时口令模式： 即识别时都说“寻物”这个口令唤醒后，如果15秒内不进行语音识别则退出唤醒
/*********************************************************/
#include "config.h"
/************************************************************************************/
//	nAsrStatus 用来在main主程序中表示程序运行的状态，不是LD3320芯片内部的状态寄存器
//	LD_ASR_NONE:		表示没有在作ASR识别
//	LD_ASR_RUNING：		表示LD3320正在作ASR识别中
//	LD_ASR_FOUNDOK:		表示一次识别流程结束后，有一个识别结果
//	LD_ASR_FOUNDZERO:	表示一次识别流程结束后，没有识别结果
//	LD_ASR_ERROR:		表示一次识别流程中LD3320芯片内部出现不正确的状态
/***********************************************************************************/
uint8 idata nAsrStatus = 0;
void MCU_init();
void ProcessInt0(); //识别处理函数
void delay(unsigned long uldata);
void User_handle(uint8 dat); //用户执行操作函数
void Delay200ms();
int over_time;
uint8_t Run_Mode = 0;
uint8_t G0_flag = DISABLE; //运行标志，ENABLE:运行。DISABLE:禁止运行
sbit STC_LED = P4 ^ 2;	   //信号指示灯
sbit STM32_SIG = P3 ^ 3;
sbit STM32_RST = P1 ^ 5;
sbit SW0 = P1 ^ 0;
sbit SW1 = P1 ^ 1;
sbit SW2 = P1 ^ 2;
sbit SW3 = P1 ^ 3;
sbit LED = P1 ^ 7;
sbit BEEP = P1 ^ 6;
/***********************************************************
* 名    称： void  main(void)
* 功    能： 主函数	程序入口
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
**********************************************************/
void main(void)
{
	uint8 idata nAsrRes;
	uint8 i = 0;
	MCU_init();
	LD_Reset();
	UartInit();				  /*串口初始化*/
	nAsrStatus = LD_ASR_NONE; //初始状态：没有在作ASR
	if (SW3 == 1)
		Run_Mode += 8;
	if (SW2 == 1)
		Run_Mode += 4;
	if (SW1 == 1)
		Run_Mode += 2;
	if (SW0 == 1)
		Run_Mode += 1;
	UARTSendStr("系统初始化成功,模式为");
	UARTSendByte(Run_Mode / 10 + '0');
	UARTSendByte(Run_Mode % 10 + '0');
	UARTSendStr("\r\n");
	while (1)
	{
		if (STM32_SIG == 0)
		{
			BEEP = 1;
			LED = 1;
			UARTSendStr("接收到STM32 蓝牙信号\r\n");
		}
		if (over_time >= 500)
		{
			over_time = 0;
			G0_flag = DISABLE;
			STC_LED = 1;
			UARTSendStr("识别超时\r\n");
		}
		switch (nAsrStatus)
		{
		case LD_ASR_RUNING:
		case LD_ASR_ERROR:
			break;
		case LD_ASR_NONE:
		{
			nAsrStatus = LD_ASR_RUNING;
			if (RunASR() == 0) /*	启动一次ASR识别流程：ASR初始化，ASR添加关键词语，启动ASR运算*/
			{
				nAsrStatus = LD_ASR_ERROR;
			}
			break;
		}
		case LD_ASR_FOUNDOK: /*	一次ASR识别流程结束，去取ASR识别结果*/
		{
			nAsrRes = LD_GetResult(); /*获取结果*/
			User_handle(nAsrRes);	  //用户执行函数
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
* 名    称： void MCU_init()
* 功    能： 单片机初始化
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
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

	AUXR &= 0x7F; //定时器时钟12T模式
	TMOD |= 0x01; //设置定时器模式
#if (FOSC == 22118400L)
	TL0 = 0x00; //设置定时初值
	TH0 = 0x28; //设置定时初值
#else
	TL0 = 0xD0;
	TH0 = 0x8A;
#endif
	TF0 = 0; //清除TF0标志
	TR0 = 1; //定时器0开始计时
	ET0 = 1;

	LD_MODE = 0; //	设置MD管脚为低，并行模式读写
	IE0 = 1;
	EX0 = 1;
	EA = 1;
	WDT_CONTR = 0x3D;
}
/***********************************************************
* 名    称：	延时函数
* 功    能：
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
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
* 名    称： 中断处理函数
* 功    能：
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
**********************************************************/
void ExtInt0Handler(void) interrupt 0
{
	ProcessInt0();
}
/***********************************************************
* 名    称：用户执行函数
* 功    能：识别成功后，执行动作可在此进行修改
* 入口参数： 无
* 出口参数：无
* 说    明：
**********************************************************/
void User_handle(uint8 dat)
{
	uint8_t i = 0;
	if (0 == dat)
	{
		G0_flag = ENABLE;
		over_time = 0;
		UARTSendStr("成功识别一级口令\r\n");
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
			UARTSendStr("钥匙\r\n");
			if (Run_Mode == 0)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("模块响应\r\n");
			}
			break;
		case CODE_2:
			UARTSendStr("钱包\r\n");
			if (Run_Mode == 1)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("模块响应\r\n");
			}
			break;
		case CODE_3:
			UARTSendStr("遥控器\r\n");
			if (Run_Mode == 2)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("模块响应\r\n");
			}
			break;
		case CODE_4:
			UARTSendStr("校园卡\r\n");
			if (Run_Mode == 3)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("模块响应\r\n");
			}
			break;
		case CODE_5:
			UARTSendStr("手机\r\n");
			if (Run_Mode == 4)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("模块响应\r\n");
			}
			break;
		case CODE_6:
			UARTSendStr("鼠标\r\n");
			if (Run_Mode == 5)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("模块响应\r\n");
			}
			break;
		case CODE_7:
			UARTSendStr("门卡\r\n");
			if (Run_Mode == 6)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("模块响应\r\n");
			}
			break;
		case CODE_8:
			UARTSendStr("U盘\r\n");
			if (Run_Mode == 7)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("模块响应\r\n");
			}
			break;
		case CODE_9:
			UARTSendStr("杯子\r\n");
			if (Run_Mode == 8)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("模块响应\r\n");
			}
			break;
		case CODE_10:
			UARTSendStr("药箱\r\n");
			if (Run_Mode == 9)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("模块响应\r\n");
			}
			break;
		case CODE_11:
			UARTSendStr("耳机\r\n");
			if (Run_Mode == 10)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("模块响应\r\n");
			}
			break;
		case CODE_12:
			UARTSendStr("记事本\r\n");
			if (Run_Mode == 11)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("模块响应\r\n");
			}
			break;
		case CODE_13:
			UARTSendStr("存钱罐\r\n");
			if (Run_Mode == 12)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("模块响应\r\n");
			}
			break;
		case CODE_14:
			UARTSendStr("电脑\r\n");
			if (Run_Mode == 13)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("模块响应\r\n");
			}
			break;
		case CODE_15:
			UARTSendStr("血压计\r\n");
			if (Run_Mode == 14)
			{
				G0_flag = DISABLE;
				LED = 1;
				BEEP = 1;
				STC_LED = 1;
				UARTSendStr("模块响应\r\n");
			}
			break;
		default:
			UARTSendStr("识别错误\r\n");
			break;
		}
	}
	else
	{
		UARTSendStr("先读出一级口令\r\n");
	}
}

void tm0_isr() interrupt 1
{
#if (FOSC == 22118400L)
	TL0 = 0x00; //设置定时初值
	TH0 = 0x28; //设置定时初值
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
