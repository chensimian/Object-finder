#include "led.h"

void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE); //使能PB端口时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                                                          //LED0-->PB.10 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                                                    //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                                   //IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);                                                              //根据设定参数初始化GPIOB.10
    GPIO_SetBits(GPIOB, GPIO_Pin_10);                                                                   //PB.10 输出高
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;                                                           //LED0-->PB.10 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                                                    //浮空输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                                   //IO口速度为2MHz
    GPIO_SetBits(GPIOA, GPIO_Pin_1);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;            //LED0-->PB.10 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //IO口速度为50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);                //根据设定参数初始化GPIOA11
}
