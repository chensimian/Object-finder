#include "led.h"

void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE); //ʹ��PB�˿�ʱ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                                                          //LED0-->PB.10 �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                                                    //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                                   //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);                                                              //�����趨������ʼ��GPIOB.10
    GPIO_SetBits(GPIOB, GPIO_Pin_10);                                                                   //PB.10 �����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;                                                           //LED0-->PB.10 �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                                                    //��������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                                   //IO���ٶ�Ϊ2MHz
    GPIO_SetBits(GPIOA, GPIO_Pin_1);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;            //LED0-->PB.10 �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);                //�����趨������ʼ��GPIOA11
}
