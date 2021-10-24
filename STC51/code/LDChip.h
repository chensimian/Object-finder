/*******************************************************
**	CPU: STC11L08XE
**	����22.1184MHZ
**	�����ʣ�9600 bit/S
**  ��ʱ����ģʽ�� ��ʶ��ʱ��˵��С�ܡ��������Ѻ����15���ڲ���������ʶ�����˳�����
/*********************************************************/


#ifndef LD_CHIP_H
#define LD_CHIP_H

#define uint8 unsigned char
#define uint16 unsigned int
#define uint32 unsigned long

//	�������״̬����������¼������������ASRʶ������е��ĸ�״̬
#define LD_ASR_NONE				0x00	 /*	��ʾû������ASRʶ��*/
#define LD_ASR_RUNING			0x01	/*	��ʾLD3320������ASRʶ����*/
#define LD_ASR_FOUNDOK			0x10	/*��ʾһ��ʶ�����̽�������һ��ʶ����*/
#define LD_ASR_FOUNDZERO 		0x11	/*��ʾһ��ʶ�����̽�����û��ʶ����*/
#define LD_ASR_ERROR	 		0x31	/*��ʾһ��ʶ��������LD3320оƬ�ڲ����ֲ���ȷ��״̬*/


#define CLK_IN   		22.1184	/* �û�ע���޸�����ľ���ʱ�Ӵ�С */
#define LD_PLL_11			(uint8)((CLK_IN/2.0)-1)
#define LD_PLL_MP3_19		0x0f
#define LD_PLL_MP3_1B		0x18
#define LD_PLL_MP3_1D   	(uint8)(((90.0*((LD_PLL_11)+1))/(CLK_IN))-1)

#define LD_PLL_ASR_19 		(uint8)(CLK_IN*32.0/(LD_PLL_11+1) - 0.51)
#define LD_PLL_ASR_1B 		0x48
#define LD_PLL_ASR_1D 		0x1f

//��������
void LD_Reset();
void LD_Init_Common();
void LD_Init_ASR();
uint8 RunASR(void);
void LD_AsrStart();
uint8 LD_AsrRun();
uint8 LD_AsrAddFixed();
uint8 LD_GetResult();


//ʶ����ͻ��޸Ĵ�
#define CODE_CMD  0x00   //��������0x00�û����ɽ����޸ġ�

#define CODE_1	0x01
#define CODE_2	0x02
#define CODE_3 	0x04
#define CODE_4 	0x14

#define CODE_5 	0x15
#define CODE_6 	0x16
#define CODE_7	0x17
#define CODE_8	0x18

#define CODE_9  0x19
#define CODE_10 0x1A
#define CODE_11 0x1B
#define CODE_12 0x1C

#define CODE_13 0x1D
#define CODE_14 0x1E
#define CODE_15 0x1F
#define CODE_16 0x20

#define CODE_17 0x21
#define CODE_18 0x22
#define CODE_19 0x23
#define CODE_20 0x24

#define CODE_21 0x25
#define CODE_22 0x26
#define CODE_23 0x27
#define CODE_24 0x28

#define CODE_25 0x29
#define CODE_26 0x2A
#define CODE_27 0x2B
#define CODE_28 0x2C

#define CODE_29 0x2D
#define CODE_30 0x2E
#define CODE_31 0x2F
#define CODE_32 0x30

#define CODE_33 0x31
#define CODE_34 0x32
#define CODE_35 0x33
#define CODE_36 0x34

#define CODE_37 0x35
#define CODE_38 0x36
#define CODE_39 0x37
#define CODE_40 0x38

#define CODE_41 0x39
#define CODE_42 0x3A
#define CODE_43 0x3B
#define CODE_44 0x3C

#define CODE_45 0x3D
#define CODE_46 0x3E
#define CODE_47 0x3F
#define CODE_48 0x40

#define CODE_49 0x41

//��ֵԽ��Խ����ʶ�����ԽԶ������ʶ���ʾ�Խ�� �����Լ���ʵ��������ڡ�
#define MIC_VOL 0x43	 //��ͷ���棨�����ȵ��ڣ� ��Χ��00-7f 
#endif
