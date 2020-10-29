/*
 *  ip5328p_battery.h
 *  fuel-gauge systems for lithium-ion (Li+) batteries
 *
 *  Copyright (C) 2019 SStar
 *  lei.qin <lei.qin@sigmastar.com.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */


#ifndef __ip5328p_BATTERY_H_
#define __ip5328p_BATTERY_H_

#define GPIO_HIGH 1
#define GPIO_LOW  0
#define PMU_IRQ_ACT_LEVEL		(1)

#define BIT0	0x01
#define BIT1	0x02
#define BIT2	0x04
#define BIT3	0x08
#define BIT4	0x10
#define BIT5	0x20
#define BIT6	0x40
#define BIT7	0x80

typedef unsigned int    MMP_ULONG;

struct ip5328p_platform_data {
	int (*battery_online)(void);
	int (*charger_online)(void);
	int (*charger_enable)(void);
};

enum _IP_PMU_INTR_MASK
{
	IP_PMU_INTR_UNMASK = 0,
	IP_PMU_INTR_MASK
};

enum _ip5328p_REG_E
{
	/*以下为可读写寄存器*/

    
    SYS_CTL1 = 0x01,//boost 和 和 charger  使能寄存器
  
    SYS_CTL2 = 0x03,//按键控制寄存器
    SYS_CTL3 = 0X04,//芯片内部温度和 NTC  温度控制寄存器
    SYS_CTL4 = 0X05,//照明灯控制寄存器
    SYS_CTL5 = 0X07,//轻载关机时间设置
	SYS_CTL6 = 0X0A,//电量灯配置寄存器
	SYS_CTL7 = 0X0B,//输出口自动检测负载寄存器
	SYS_CTL8 = 0X0C,//按键控制输出口寄存器
	SYS_CTL9 = 0X0D,//边充边放功能寄存器
	SYS_CTL10 = 0X0E,//按键控制输出口寄存器

	VBAT_LO = 0X10,	//低电关机阈值设置
	VINOV = 0X11,	//VIN  输入过压设置
	VBUSOV = 0X12,	//VBUS  输入过压设置
	BOOST_LINC = 0X13,	//输出线补设置寄存器

	TYPEC_CTRL0 = 0X1A,	//typec CC  上拉设置 寄存器
	TYPEC_CTRL1 = 0X1B,//typec CC  模式 配置寄存
	TYPEC_CTRL2 = 0X1C,//typec PD  协议使能寄存器
	TYPEC_CTRL3 = 0X1E,//输入请求最高电压设置
	
	CHG_CTL1 = 0X21,//充电停充电压设置寄存器
	CHG_CTL2 = 0X22,//充电充满电压设置寄存器
	CHG_CTL3 = 0X23,//充电欠压环设置寄存器 5V 7V 
	CHG_CTL4 = 0X24,//充电欠压环设置寄存器 9V 12V

	CHG_ISET_9V = 0x26,//
	CHG_ISET_12V = 0x27,//
	CHG_ISET_5V_VBUS = 0x29,
	CHG_ISET_5V_VIN = 0x2A,
	CHG_ISET_7V = 0x2B,
	CHG_TIMER_EN = 0x2C,
	CHG_TIMER_SET = 0x2D,
	
	DCDC_FREQ = 0x31,//DC-DC  开关频率设置寄存器

	QC_EN = 0x3E,//输入输出口 DCP  快充协议使能寄存器

	IC_TEMP = 0X42,//IC  内部过温寄存器
	CHG_NTC_TEMP = 0X43,//充电 NTC  阈值寄存器

	
	BST_NTC_TEMP = 0x54,//放电 NTC  阈值寄存器
	PMOS_REG_CTL0 = 0x59,//输入输出口控制寄存器
	PMOS_REG_CTL1 = 0x5A,//输入输出口控制寄存器
	FORCE_EN = 0x5B,//寄存器关机和复位控制寄存器
	
	FLAG0 = 0x7E,//异常标志位
	FLAG1 = 0x7F,//按键和过压标志
	BST_POWERLOW = 0x81,//轻载关机 功率 阈值设置寄存器
	RSET = 0x82,//电池内阻补偿寄存器
	IPMOSLOW = 0x86,//多口转单口 MOS  关电流阈值设置寄存器
	BATOCV_LOW = 0x88,//低电退出主动退出快充设置寄存器
	IPMOSLOW_TIME = 0x90,//多口转单口时间设置寄存器
	QC_VMAX = 0x96,// （设置 QC  协议最大输出电压
	BATOCV_LOW_DN = 0x9F,//
	
	DCP_DIG_CTL0 = 0xA0,
	DCP_DIG_CTL1 = 0xA1,
	DCP_DIG_CTL2 = 0xA2,
	BOOST_5V_ISET = 0xA8,
	BOOST_VSET = 0x4C,

	DCP_DIG_CTL10 = 0xAA,
	LED_STATUS = 0XDB,//电量信息寄存器

	/*以下为只读寄存器*/
	BATVADC_DAT0 = 0x64,//BAT  真实电压 寄存器
	BATVADC_DAT1 = 0x65,//BAT  真实电压 寄存器
	BATIADC_DAT0 = 0x66,//BAT  端电流 寄存器 
	BATIADC_DAT1 = 0x67,//BAT  端电流 寄存器 
	SYSVADC_DAT0 = 0x68,//VSYS  端电压值 寄存器
	SYSVADC_DAT1 = 0x69,//VSYS  端电压值 寄存器
	SYSIADC_DAT0 = 0x6A,//10  毫欧 采样 电阻 流过的电流 寄存器 
	SYSIADC_DAT1 = 0x6B,//10  毫欧 采样 电阻 流过的电流 寄存器 
	VINIADC_DAT0 = 0x6C,//VIN  通路 MOS  流过的电流 寄存器 
	VINIADC_DAT1 = 0x6D,//VIN  通路 MOS  流过的电流 寄存器
	VBUSIADC_DAT0 = 0x6E,//VBUS  通路 MOS  流过的电流 寄存器
	VBUSIADC_DAT1 = 0x6F,//VBUS  通路 MOS  流过的电流 寄存器
	VOUT1IADC_DAT0 = 0x70,//VOUT1  通路 MOS  流过的电流
	VOUT1IADC_DAT1 = 0x71,//VOUT1  通路 MOS  流过的电流
	VOUT2IADC_DAT0 = 0x72,//VOUT2  通路 MOS  流过的电流
	VOUT2IADC_DAT1 = 0x73,//VOUT2  通路 MOS  流过的电流
	RSETADC_DAT0 = 0x74,//
	RSETADC_DAT0 = 0x75,//
	GPIADC_DAT0 = 0x78,//GPIO ADC  电压值寄存器
	GPIADC_DAT1 = 0x79,//GPIO ADC  电压值寄存器
	BATOCV_DAT0 = 0x7A,//BATOCV  电压 寄存器
	BATOCV_DAT1 = 0x7B,//BATOCV  电压 寄存器
	POWER_DAT0 = 0x7C,//输入输出功率寄存器
	POWER_DAT1 = 0x7D,//输入输出功率寄存器
	SYS_STATUS = 0xD1,//系统状态指示寄存器
	KEY_IN = 0xD2,//系统状态指示寄存器
	OV_FLAG  = 0xD3,//系统过压/ 欠压寄存器
	VIN_VBUS_STATE  = 0xD5,//VIN/VBUS  充电电压寄存器
	CHG_STATUS = 0xD7,//充电状态指示寄存器
	LOW_STATUS  = 0xD9,//系统轻载标志位
	NTC_FLAG  = 0xDA,//NTC  状态指示寄存器
	LOWCUR_FLAG   = 0xDE,//常开 N  小时标准位
	MOS_ON  = 0xE5,//径 MOS  开启状态寄存器 
	BST_V_FLAG  = 0xFB,//BOOST  电压范围寄存器
	TYPEC_OK  = 0XB8,//  TYPEC  连接状态寄存器
	TYPEC_FLAG  = 0xFC,//TYPEC  上拉状态寄存器
	    
};

typedef struct _IpWDogCtl_T
{
	unsigned int	uTimerType : 2;     //00:0.5s   01:2s   10:8s   11:16s
	unsigned int	bTimerClr : 1;      //д1  ���
	unsigned int	bIsWDogEn : 1;      // 1:ʹ��   0:��ֹ
	unsigned int	bRsvd : 28;
}IpWDogCtl_T, *PIpWDogCtl_T;

typedef union _ip5328p_WDogCtl_U
{
	unsigned int	uData;
	IpWDogCtl_T	tFlag;
}ip5328pWDogCtl_U, *pip5328pWDogCtl_U;

//REG_0x00
typedef struct _IPPStateCtl0_T
{
	MMP_ULONG	bPwrOffEn : 1;//1:����S2/S3
	MMP_ULONG	bInstPowerDown : 1;//д0 :˳����磬1:ͬʱ����
	MMP_ULONG	bPorOffEn : 1;//1:ʹ��POR���͹ػ�ʹ��
	MMP_ULONG	bVBusWkEn : 1;//1:ʹ��VBUS����
	MMP_ULONG	bSOnoffWkEn : 1;//1:ʹ�̰ܶ�ONOFF����
	MMP_ULONG	bLOnoffWkEn : 1;//1:ʹ�ܳ���ONOFF����
	MMP_ULONG	bIrqWkEn : 1;//1:ʹ���ⲿ�жϻ���
	MMP_ULONG	bAlarmWkEn : 1;//1:ʹ��Alarm����
	MMP_ULONG	bRsvd : 24;
}IPPStateCtl0_T, *PIPPStateCtl0_T;
typedef union _IPPStateCtl0_U
{
	MMP_ULONG		uData;
	IPPStateCtl0_T	tFlag;
}IPPStateCtl0_U, *PIPPStateCtl0_U;

//REG_0x58
typedef struct _IpChgDigCtl3_T
{
	MMP_ULONG	bRsvd0 : 1;
	MMP_ULONG	bEnChg : 1;//1:ʹ�ܳ��
	MMP_ULONG	uChgLedMode : 1;//0:������������𣬷ŵ��𣬵͵�����;1:�������������������ŵ糣�����͵�����
	MMP_ULONG	bRsvd : 29;
}IpChgDigCtl3_T, *PIpChgDigCtl3_T;
typedef union _IpChgDigCtl3_U
{
	MMP_ULONG		uData;
	IpChgDigCtl3_T	tFlag;
}IpChgDigCtl3_U, *PIpChgDigCtl3_U;

//REG_0x60
typedef struct _IpAdcAnaCtl0_T
{
	MMP_ULONG	bEnVBatAdc : 1;//1:ʹ�ܵ�ص�ѹADC ���
	MMP_ULONG	bEnIBatAdc : 1;//ʹ�ܵ�ص���ADC ���
	MMP_ULONG	bEnIChgAdc : 1;//1:ʹ�ܳ�����ADC ���
	MMP_ULONG	bEnGp1Adc : 1;//1:ʹ��GP1  ADC ���
	MMP_ULONG	bEnGp2Adc : 1;//1:ʹ��GP2  ADC ���
	MMP_ULONG	bRsvd : 27;
}IpAdcAnaCtl0_T, *PIpAdcAnaCtl0_T;
typedef union _IpAdcAnaCtl0_U
{
	MMP_ULONG		uData;
	IpAdcAnaCtl0_T	tFlag;
}IpAdcAnaCtl0_U, *PIpAdcAnaCtl0_U;

//REG_0x70
typedef struct _IpIntsCtl_T
{
	MMP_ULONG	bIrqPolHighValid : 1;//IRQ����1:High Valid,0:Low Valid
	MMP_ULONG	bIrqClr : 1;//д1  ��IRQ�����Ч��ƿ��delay 32 us���������Ч��ƿ
	MMP_ULONG	bRsvd : 30;
}IpIntsCtl_T, *PIpIntsCtl_T;
typedef union _IpIntsCtl_U
{
	MMP_ULONG		uData;
	IpIntsCtl_T	tFlag;
}IpIntsCtl_U, *PIpIntsCtl_U;


//REG_0x71
typedef struct _IpIntrFlag0_T
{
	MMP_ULONG	bShortPressOnOff : 1;//�̰���
	MMP_ULONG	bLongPressOnOff : 1;//������
	MMP_ULONG	bUShortPressOnOff : 1;//���̰�����
	MMP_ULONG	bVBusIn : 1;//VBUS����
	MMP_ULONG	bVBusOut : 1;//VBUS�γ�
	MMP_ULONG	bLowBat : 1;//��ص͵�
	MMP_ULONG	bRsvd : 1;
	MMP_ULONG	bAlarm : 1;//ALARM
	MMP_ULONG	bRsvd1 : 24;
}IpIntrFlag0_T, *PIpIntrFlag0_T;
typedef union _IpIntrFlag0_U
{
	MMP_ULONG		uData;
	IpIntrFlag0_T	tFlag;
}IpIntrFlag0_U, *PIpIntrFlag0_U;


//REG_0x73
typedef struct _IpIntrMask0_T
{
	MMP_ULONG	bShortPressOnOff : 1;//�̰���
	MMP_ULONG	bLongPressOnOff : 1;//������
	MMP_ULONG	bUShortPressOnOff : 1;//���̰�����
	MMP_ULONG	bVBusIn : 1;//VBUS����
	MMP_ULONG	bVBusOut : 1;//VBUS�γ�
	MMP_ULONG	bLowBat : 1;//��ص͵�
	MMP_ULONG	bRsvd : 1;
	MMP_ULONG	bAlarm : 1;//ALARM
	MMP_ULONG	bRsvd1 : 24;
}IpIntrMask0_T, *PIpIntrMask0_T;
typedef union _IpIntrMask0_U
{
	MMP_ULONG		uData;
	IpIntrMask0_T	tFlag;
}IpIntrMask0_U, *PIpIntrMask0_U;

#endif
