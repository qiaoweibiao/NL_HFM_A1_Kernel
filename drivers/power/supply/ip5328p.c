/*
 * Driver for IP5328P Micro/Mini USB IC with integrated charger
 *
 *			Copyright (C) 2011 Texas Instruments
 *			Copyright (C) 2011 National Semiconductor
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/power_supply.h>
#include <linux/platform_data/ip5328p.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>

#include <linux/delay.h>

#include <linux/device.h>
#include <linux/gpio.h>

#include <linux/gpio/consumer.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#include <linux/slab.h>


#define IP5328P_MANUFACTURER		"injoinic"
#define IP5328P_B_IC_KEY			"B_IC_KEY"

#define IP5328P_NUM_INTREGS	2
#define DEFAULT_DEBOUNCE_MSEC	270

#if 1

/* Registers */
#define SYS_CTRL1  			0x01//boost 和 和 charger  使能寄存器

#define SYS_CTRL2  			0x03//按键控制寄存器
#define SYS_CTL3  			0X04//芯片内部温度和 NTC  温度控制寄存器
#define SYS_CTL4  			0X05//照明灯控制寄存器
#define SYS_CTL5  			0X07//轻载关机时间设置
#define SYS_CTL6  			0X0A//电量灯配置寄存器
#define SYS_CTL7  			0X0B//输出口自动检测负载寄存器
#define SYS_CTL8  			0X0C//按键控制输出口寄存器
#define SYS_CTL9  			0X0D//边充边放功能寄存器
#define SYS_CTL10  			0X0E//按键控制输出口寄存器

#define VBAT_LO  			0X10 //低电关机阈值设置
#define VINOV  				0X11	//VIN  输入过压设置
#define VBUSOV  			0X12	//VBUS	输入过压设置
#define BOOST_LINC  		0X13	//输出线补设置寄存器

#define TYPEC_CTRL0  		0X1A //typec CC	上拉设置 寄存器
#define TYPEC_CTRL1  		0X1B//typec CC  模式 配置寄存
#define TYPEC_CTRL2  		0X1C//typec PD  协议使能寄存器
#define TYPEC_CTRL3  		0X1E//输入请求最高电压设置

#define CHG_CTL1  			0X21//充电停充电压设置寄存器
#define CHG_CTL2  			0X22//充电充满电压设置寄存器
#define CHG_CTL3  			0X23//充电欠压环设置寄存器 5V 7V 
#define CHG_CTL4  			0X24//充电欠压环设置寄存器 9V 12V

#define CHG_ISET_9V  		0x26//
#define CHG_ISET_12V  		0x27//
#define CHG_ISET_5V_VBUS  	0x29
#define CHG_ISET_5V_VIN  	0x2A
#define CHG_ISET_7V  		0x2B
#define CHG_TIMER_EN  		0x2C
#define CHG_TIMER_SET  		0x2D

#define DCDC_FREQ 			0x31//DC-DC  开关频率设置寄存器

#define QC_EN  				0x3E//输入输出口 DCP	快充协议使能寄存器
		
#define IC_TEMP  			0X42//IC  内部过温寄存器
#define CHG_NTC_TEMP  		0X43//充电 NTC  阈值寄存器


#define BST_NTC_TEMP  		0x54//放电 NTC  阈值寄存器
#define PMOS_REG_CTL0  		0x59//输入输出口控制寄存器
#define PMOS_REG_CTL1  		0x5A//输入输出口控制寄存器
#define FORCE_EN  			0x5B//寄存器关机和复位控制寄存器

#define FLAG0  				0x7E//异常标志位
#define FLAG1  				0x7F//按键和过压标志
#define BST_POWERLOW  		0x81//轻载关机 功率 阈值设置寄存器
#define RSET  				0x82//电池内阻补偿寄存器
#define IPMOSLOW  			0x86//多口转单口 MOS  关电流阈值设置寄存器
#define BATOCV_LOW  		0x88//低电退出主动退出快充设置寄存器
#define IPMOSLOW_TIME  		0x90//多口转单口时间设置寄存器
#define QC_VMAX  			0x96// （设置 QC	协议最大输出电压
#define BATOCV_LOW_DN  		0x9F//

#define DCP_DIG_CTL0  		0xA0
#define DCP_DIG_CTL1  		0xA1
#define DCP_DIG_CTL2  		0xA2
#define BOOST_5V_ISET  		0xA8
#define BOOST_VSET  		0x4C

#define DCP_DIG_CTL10  		0xAA
#define LED_STATUS  		0XDB//电量信息寄存器

/*以下为只读寄存器*/
#define BATVADC_DAT0  		0x64//BAT  真实电压 寄存器
#define BATVADC_DAT1  		0x65//BAT  真实电压 寄存器
#define BATIADC_DAT0  		0x66//BAT  端电流 寄存器 
#define BATIADC_DAT1  		0x67//BAT  端电流 寄存器 
#define SYSVADC_DAT0  		0x68//VSYS	端电压值 寄存器
#define SYSVADC_DAT1  		0x69//VSYS	端电压值 寄存器
#define SYSIADC_DAT0  		0x6A//10  毫欧 采样 电阻 流过的电流 寄存器 
#define SYSIADC_DAT1  		0x6B//10  毫欧 采样 电阻 流过的电流 寄存器 
#define VINIADC_DAT0  		0x6C//VIN  通路 MOS  流过的电流 寄存器 
#define VINIADC_DAT1  		0x6D//VIN  通路 MOS  流过的电流 寄存器
#define VBUSIADC_DAT0  		0x6E//VBUS  通路 MOS  流过的电流 寄存器
#define VBUSIADC_DAT1  		0x6F//VBUS  通路 MOS  流过的电流 寄存器
#define VOUT1IADC_DAT0  	0x70//VOUT1  通路 MOS  流过的电流
#define VOUT1IADC_DAT1  	0x71//VOUT1  通路 MOS  流过的电流
#define VOUT2IADC_DAT0  	0x72//VOUT2  通路 MOS  流过的电流
#define VOUT2IADC_DAT1  	0x73//VOUT2  通路 MOS  流过的电流
#define RSETADC_DAT0  		0x74//
#define RSETADC_DAT0  		0x75//
#define GPIADC_DAT0  		0x78//GPIO ADC  电压值寄存器
#define GPIADC_DAT1  		0x79//GPIO ADC  电压值寄存器
#define BATOCV_DAT0  		0x7A//BATOCV  电压 寄存器
#define BATOCV_DAT1  		0x7B//BATOCV  电压 寄存器
#define POWER_DAT0  		0x7C//输入输出功率寄存器
#define POWER_DAT1  		0x7D//输入输出功率寄存器
#define SYS_STATUS  		0xD1//系统状态指示寄存器
#define KEY_IN  			0xD2//系统状态指示寄存器
#define OV_FLAG  			0xD3//系统过压/ 欠压寄存器
#define VIN_VBUS_STATE	 	0xD5//VIN/VBUS  充电电压寄存器
#define CHG_STATUS 			0xD7//充电状态指示寄存器
#define LOW_STATUS	 		0xD9//系统轻载标志位
#define NTC_FLAG   			0xDA//NTC	状态指示寄存器
#define LOWCUR_FLAG   		0xDE//常开 N  小时标准位
#define MOS_ON				0xE5// MOS  开启状态寄存器 
#define BST_V_FLAG	 		0xFB//BOOST	电压范围寄存器
#define TYPEC_OK  			0XB8//  TYPEC	连接状态寄存器
#define TYPEC_FLAG	 		0xFC//TYPEC	上拉状态寄存器


/*0xFF 8bit 数据 响应的bit 对应的功能参考 IC spec*/
#define BIT0		BIT(0)
#define BIT1		BIT(1)
#define BIT2		BIT(2)
#define BIT3		BIT(3)
#define BIT4		BIT(4)
#define BIT5		BIT(5)
#define BIT6		BIT(6)
#define BIT7		BIT(7)


#endif

struct IP5328P_chg {
	struct device *dev;
	struct i2c_client *client;
	struct mutex xfer_lock;
	struct IP5328P_psy *psy;
	struct IP5328P_platform_data *pdata;
	//struct gpio_desc *B_IC_KEY;
	int B_IC_KEY;
	/* Interrupt Handling */
};

static int IP5328P_read_bytes(struct IP5328P_chg *pchg, u8 reg, u8 *data, u8 len)
{
	s32 ret;

	mutex_lock(&pchg->xfer_lock);
	ret = i2c_smbus_read_i2c_block_data(pchg->client, reg, len, data);
	mutex_unlock(&pchg->xfer_lock);

	return (ret != len) ? -EIO : 0;//判断语句 return  -EIO : 0
}

static inline int IP5328P_read_byte(struct IP5328P_chg *pchg, u8 reg, u8 *data)
{
	return IP5328P_read_bytes(pchg, reg, data, 1);
}

static int IP5328P_write_byte(struct IP5328P_chg *pchg, u8 reg, u8 data)
{
	int ret;

	mutex_lock(&pchg->xfer_lock);
	ret = i2c_smbus_write_byte_data(pchg->client, reg, data);
	mutex_unlock(&pchg->xfer_lock);

	return ret;
}

//获取芯片当前电源状态
//第四位：0:放电 1:充电
//2:0位 ：000:待机 001:5V充电 010:单口同充同放 011:多口同充同放
//		  100:高压快充充电 101:5V放电 110:多口5V放电 111:高压快充放电
//其他位：无效位

static int IP5328P_SYS_Status(struct IP5328P_chg *pchg)
{
	u8 val;
	u8 val1;
	IP5328P_read_byte(pchg, SYS_STATUS, &val);//qwb007	
	IP5328P_read_byte(pchg, SYS_STATUS, &val1);//qwb007	
	
	if((val == 0xff) | (val1 == 0xff)){
	printk("IP5328P not init as the val  = %d\n",val);
	return 0;
	}

	val1 = val1>> 4 & 0x01;
	if (val1 == 0x00)
	printk("0:放电 1:充电 IP5328P 在放电 as the val1  = %d\n",val1);
	if (val1 == 0x01) 
		printk("0:放电 1:充电 IP5328P 充电 as the val1  = %d\n",val1);
	
	val = val &0x07;//
	
	switch(val){

		case 0x00 :
			printk("在待机 IP5328P_SYS_Status val	= %x\n",val);
			break;
		case 0x01 :
			printk("在5V 充电	IP5328P_SYS_Status val = %x\n",val);
			break;
		case 0x02 :
			printk("单口同充同放 充电	 IP5328P_SYS_Status val = %x\n",val);
			break;	
		case 0x03 :
			printk("多口同充同放 充电	 IP5328P_SYS_Status val = %x\n",val);
			break;			
		case 0x04 :
			printk("高压快充充电 充电	 IP5328P_SYS_Status val = %x\n",val);
			break;			
		
		case 0x05 :
			printk("5V放电 充电  IP5328P_SYS_Status val = %x\n",val);
			break;	
		case 0x06 :
			printk("多口5V放电 充电	 IP5328P_SYS_Status val = %x\n",val);
			break;
		case 0x07 :
			printk("高压快充放电 充电	 IP5328P_SYS_Status val = %x\n",val);
			break;

	}
	

	printk("IP5328P_SYS_Status() 执行完毕     val  = %x\n",val);
	
	return val;

}

/*----------------------------------------------读取芯片参数功能函数-start-----------------------------------------------------*/


/////////读取电池电量显示级别(只能读到对应电量的LED个数)
//返回：		0001 1111:4颗灯亮
//////		0000 1111:3颗灯亮
//////		0000 0111:2颗灯亮
//////		0000 0011:1颗灯亮
//////		0000 0001:放电时低电闪灯 
//////		0000 0000:关机

static int IP5328P_Electricity(struct IP5328P_chg *pchg)
{


	u8 dat;
	
	IP5328P_read_byte(pchg, SYS_CTL6, &dat);

	dat = dat | 0xE0;
	
	mdelay(500);
	
	IP5328P_write_byte(pchg, SYS_CTL6, dat);//写为4灯模式
	
	printk(" qwb007 SYS_CTL6 val	= %x\n",dat);

	mdelay(500);
	IP5328P_read_byte(pchg, LED_STATUS, &dat);
	dat = dat & 0x1F;

	switch(dat){
	
			case 0x00 :
				printk("关机 IP5328P_Electricity val	= %x\n",dat);
				break;
			case 0x01 :
				printk("放电时低电闪灯 IP5328P_Electricity val	= %x\n",dat);
				break;
			case 0x03 :
				printk("1颗灯亮 IP5328P_Electricity val	= %x\n",dat);
				break;	
			case 0x07 :
				printk("2颗灯亮 IP5328P_Electricity val	= %x\n",dat);
				break;			
			case 0x0F :
				printk("3颗灯亮 IP5328P_Electricity val	= %x\n",dat);
				break;			
			case 0x1F :
				printk("4颗灯亮 IP5328P_Electricity val	= %x\n",dat);
				break;		
	
		}

	return dat;

}

//读取电池两端的真实电压值
//由于kernel 打印 float 类型不方便 所以 返回u32 的电压值整数 需要除以100000000 mv 260000000 表示2.6mv 


static int IP5328P_BatVoltage(struct IP5328P_chg *pchg)
{

	u8 BAT_H_8bit;
	u8 BAT_L_8bit;
	u16 V_BAT;	  
	u32 V_BAT_F;
	IP5328P_read_byte(pchg, BATVADC_DAT0, &BAT_L_8bit);
	IP5328P_read_byte(pchg, BATVADC_DAT1, &BAT_H_8bit);
	V_BAT = (BAT_H_8bit<<8) + BAT_L_8bit;
	V_BAT_F = V_BAT*26855 + 260000000;
	printk("the value of int 000000000000 V_BAT_F = %d\n",V_BAT_F);
	return V_BAT_F;
}

//读取电池电压经过电芯内阻和电芯电流进行补偿后的电压
//由于kernel 打印 float 类型不方便 所以 返回u32 的电压值整数 需要除以100000000 mv 260000000 表示2.6mv 


static int IP5328P_BatOCV(struct IP5328P_chg *pchg)
{

	u8 BAT_H_8bit;
	u8 BAT_L_8bit;
	u16 V_BAT;	  
	u32 V_BAT_F;
	IP5328P_read_byte(pchg, BATOCV_DAT0, &BAT_L_8bit);
	IP5328P_read_byte(pchg, BATOCV_DAT1, &BAT_H_8bit);
	V_BAT = (BAT_H_8bit<<8) + BAT_L_8bit;
	V_BAT_F = V_BAT*26855 + 260000000;
	printk("the value of int 000000000000 V_BAT_F = %d\n",V_BAT_F);
	return V_BAT_F;
}

//读取电池输入或输出电流
//返回u32型的电流值整数	 单   位     	0.00127883 ma 

static int  IP5328P_BatCurrent(struct IP5328P_chg *pchg)
{
	u32 Current;
	u8 BAT_H_8bit;
	u8 BAT_L_8bit;
	u16 V_BAT;	  
	u32 V_BAT_F;
	
	IP5328P_read_byte(pchg, BATIADC_DAT0, &BAT_L_8bit);//获取电池电流低8位
	IP5328P_read_byte(pchg, BATIADC_DAT1, &BAT_H_8bit);//获取电池电流高8位
	Current = (BAT_H_8bit<<8)+BAT_L_8bit;				//高低位合并
	if(Current>32767){
		
		Current=65535-Current;	//将负值电流转为正值

	}
		
	Current = Current*127883;//计算为实际电流值 需要除以 100000000 mA
	return Current;
}


////////获取Type-C接口连接状态///////////
//返回：0x00未连接
//0x01连接手机等需供电设备(充电宝在放电)
//0x02连接电源适配器(充电宝在充电)

static int IP5328P_TypeC_OK(struct IP5328P_chg *pchg)
{
	u8 val;
	
	u8 flag=0;
	u8 dat=0;

	IP5328P_read_byte(pchg, TYPEC_OK, &val);
	
	if(val == 0xff){
	printk("IP5328P not init as the val  = %d\n",val);
	return 0;
	}

	if(val >> 1 & 0x01){
	flag =0x01;
	printk("IP5328P 在放电");
	}
	else
		printk("IP5328P 未连接");

	if(val >> 5 & 0x01){
	flag =0x02;
	printk("IP5328P 在充电");
	}	
	else
		printk("IP5328P 未连接");	
	return flag;
		
}



//获取Type-C连接的适配器输出能力
//返回：0x00 芯片未激活
//		0x01标准USB
//		0x02输出能力1.5A
//		0x03输出能力3.0A



static int IP5328P_TypeC_Ability(struct IP5328P_chg *pchg)
{

	u8 val;
	unsigned char	flag=0;
	unsigned char	dat=0;
	
	IP5328P_read_byte(pchg, TYPEC_FLAG, &val);

	if(val == 0xff){
	printk("IP5328P not init as the val  = %d\n",val);
	return 0;
	}

	if(val & 0x07 ==0x01 ){
	flag =0x01;
	printk("标准 USB ");
	}

	if(val & 0x07 ==0x03){
	flag =0x02;
	printk("输出能力1.5A");
	}	

	if(val & 0x07 ==0x07){
	flag =0x03;
	printk("输出能力3A");
	}

	return flag;


}

//获取输入电压有效状态以及按键是否被按下
//返回：7:6无效位
//		5 VBUSOK: 1 TYPE-C接口电压有效（充放电都会有效） 0 接口电压无效
//		4 VINOK : 1 安卓口接口电压有效	                 0 安卓口接口电压无效
//		3:1无效位
//		0 key_in: 0按键按下    1按键未按下

static int IP5328P_KEY_IN(struct IP5328P_chg *pchg)
{

	u8 val;
	val = IP5328P_read_byte(pchg, KEY_IN, &val);
	if(val==0xff)
		return 0;
	return val;
}






//按键 60ms < 2s  	短按 打开电量显示灯和升压输出
//按键		> 2s  	长按 开启或者关闭照明 WLED
//按键		< 30ms  无效 
//按键 				1s内连续2次短按 会关闭升压输出、电量显示和照明 WLED
//按键 				超长按 10s 可复位整个系统


static int IP5328P_IC_KEY_IN(struct IP5328P_chg *pchg)		
{

	gpio_set_value_cansleep(pchg->B_IC_KEY, 0);
	printk("qwb0007gpio_set_value_cansleep val = %d\n");
	mdelay(100);
	gpio_set_value_cansleep(pchg->B_IC_KEY, 1);
	printk("qwb0007gpio_set_value_cansleep val = %d\n");
	return pchg;
 
}

//获取充电状态
//返回：7:	0可能刚好是在停充检测，也可能是充满了，也可能是异常保护了	 1 正在充电
//		6:  0未充满  1充满 
//		5:  0恒压恒流总计时未超时   1恒压恒流总计时超时
//		4:  0恒压计时未超时         1恒压计时超时
//		3:  0涓流计时未超时         1涓流计时超时
//		2:  0000空闲  001涓流充电  010恒流充电  011恒压充电 100停充检测 101电池充满结束 110超时

static int IP5328P_GHG_State(struct IP5328P_chg *pchg)
{

	u8 val;
	val = IP5328P_read_byte(pchg, CHG_STATUS, &val);
	if(val == 0xff){
	printk("IP5328P not init as the val  = %d\n",val);
	return 0;
	}
	return val;
}


//获取升压输出电压值范围
//返回：7:4	无效位 
//		3:  0无效      1输出电压10-12V
//		2:  0无效      1输出电压8-10V
//		1:  0无效      1输出电压6-8V
//		0:  0非快充    1快充

static int IP5328P_BOOST(struct IP5328P_chg *pchg)
{

	u8 val;
	val = IP5328P_read_byte(pchg, BST_V_FLAG, &val);
	if(val == 0xff){
	printk("IP5328P not init as the val  = %d\n",val);
	return 0;
	}	
	return val;
}

//获取QC快充是否使能(不是是否正在使用，而是说这个功能可以用)
//返回：7:4	无效位 
//		3:  0非快充      1TYPE-C快充使能
//		2:  0非快充      1安卓口快充使能
//		1:  0非快充      1OUT2快充开使能
//		0:  0非快充      1OUT1快充开使能




static int IP5328P_QC_State(struct IP5328P_chg *pchg)
{

	u8 val;
	val = IP5328P_read_byte(pchg, QC_EN, &val);
	if(val == 0xff){
	printk("IP5328P not init as the val  = %d\n",val);
	return 0;
	}	
	return val;
}

//获取快充是否可以被使用(不是是否正在使用，而是说这个功能可以用)
//返回：7:	MTK PE 1.1 RX 支持的最大电压设置       0 12V	       1 9V
//		6:  MTK PE2.0 RX  使能                     0 失能       1 使能 
//		5:  MTK PE1.1 RX  使能                     0 失能       1 使能 
//		4:  SFCP SRC(展讯)使能                     0 失能       1 使能
//		3:  AFC SRC(三星) 使能                     0 失能       1 使能
//		2:  FCP SRC(华为) 使能                     0 失能       1 使能
//		1:  QC3.0 SRC     使能                     0 失能       1 使能
//		0:  QC2.0 SRC     使能                     0 失能       1 使能



static int IP5328P_DCP_DIG(struct IP5328P_chg *pchg)
{

	u8 val;
	val = IP5328P_read_byte(pchg, QC_EN, &val);
	if(val == 0xff){
	printk("IP5328P not init as the val  = %d\n",val);
	return 0;
	}	
	return val;
}

/*----------------------------------------------读取芯片参数功能函数-end-----------------------------------------------------*/


/**************************************************写芯片参数功能函数********************************************************/

//设置电池低电关机电压(关机后需重新充电才可启动)
//输入：0x30 3.00V-3.10V
//      0x20 2.90V-3.00V
//      0x10 2.81V-2.89V
//      0x00 2.73V-2.81V

static int IP5328P_BAT_LOW(struct IP5328P_chg *pchg)
{

	u8 dat;
	if(dat==0x30||dat==0x20||dat==0x10||dat==0x00)//为保证器件不出错，确认指令正确后在写入
	IP5328P_write_byte(pchg, VBAT_LO, dat);
}


static int IP5328P_init_device(struct IP5328P_chg *pchg)

{
	u8 ret;
	//判断目前是否在充电状态 充电状态可从 USB2_VBUS 状态获取，确认下这时候这部分驱动是否已经启用
	//如果是充电状态则直接读取i2c相关数值，由于有充电行为则继续可以开机
	//如果是非充电状态则连续2次按键（间隔70ms-80ms）然后 50ms 后读取 i2c的值 主要是电压值 以确认是否需要关机。满足开机电压则继续 开机 否则关机
	ret = IP5328P_SYS_Status(pchg);

	
	if (ret == 0 )
		{
		printk("001 IP5328P not init as the val  = %d\n",ret);
		
		ret = IP5328P_IC_KEY_IN(pchg);
		printk("IP5328P_IC_KEY_IN as the val  = %d\n",ret);

		ret = IP5328P_SYS_Status(pchg);
		printk("IP5328P_SYS_Status ret = %d\n",ret);

		
		}
	ret = IP5328P_Electricity(pchg);
	if (ret == 0 )
		{
		printk("002 IP5328P not init as the val  = %d\n",ret);
		return 0;
		}

	ret = IP5328P_BatVoltage(pchg);
	if (ret == 0 )
		{
		printk("003 IP5328P not init as the val  = %d\n",ret);
		return 0;
		}
	ret = IP5328P_BatCurrent(pchg);
	if (ret == 0 )
		{
		printk("004 IP5328P not init as the val  = %d\n",ret);
		return 0;
		}
	ret = IP5328P_TypeC_OK(pchg);
	if (ret == 0 )
		{
		printk("IP5328P_TypeC_OK 未连接");
		}
	
	ret = IP5328P_TypeC_Ability(pchg);

	printk("IP5328P_init_device is OK = %d\n");

		return 0;

}



#ifdef CONFIG_OF
static struct IP5328P_chg_param
*IP5328P_parse_charge_pdata(struct device *dev, struct device_node *np)
{
	struct IP5328P_chg_param *param;

	param = devm_kzalloc(dev, sizeof(*param), GFP_KERNEL);
	if (!param)
		goto out;

	of_property_read_u8(np, "eoc-level", (u8 *)&param->eoc_level);
	of_property_read_u8(np, "charging-current", (u8 *)&param->ichg);
out:
	return param;
}

static struct IP5328P_platform_data *IP5328P_parse_dt(struct device *dev)
{
	struct device_node *np = dev->of_node;
	struct device_node *child;
	struct IP5328P_platform_data *pdata;
	const char *type;

	pdata = devm_kzalloc(dev, sizeof(*pdata), GFP_KERNEL);
	if (!pdata)
		return ERR_PTR(-ENOMEM);

	of_property_read_u32(np, "debounce-ms", &pdata->debounce_msec);

	/* If charging parameter is not defined, just skip parsing the dt */
	if (of_get_child_count(np) == 0)
		return pdata;

	for_each_child_of_node(np, child) {
		of_property_read_string(child, "charger-type", &type);

		if (!strcmp(type, "ac"))
			pdata->ac = IP5328P_parse_charge_pdata(dev, child);

		if (!strcmp(type, "usb"))
			pdata->usb = IP5328P_parse_charge_pdata(dev, child);
	}
	
	return pdata;
}
#else
static struct IP5328P_platform_data *IP5328P_parse_dt(struct device *dev)
{
	return NULL;
}
#endif



/*------------------------------------------------写芯片参数功能函数-end----------------------------------------------------*/

/*---------------------------------------------------获取常用参数-----------------------------------------------------------*/


static int IP5328P_probe(struct i2c_client *cl, const struct i2c_device_id *id)
{	
	struct IP5328P_chg *pchg;
	
	struct IP5328P_platform_data *pdata;
	
	int ret;	
	if (!i2c_check_functionality(cl->adapter, I2C_FUNC_SMBUS_I2C_BLOCK))
		return -EIO;

	if (cl->dev.of_node) {//dts 参数传入
		pdata = IP5328P_parse_dt(&cl->dev);
		if (IS_ERR(pdata))
			return PTR_ERR(pdata);
	} else {
		pdata = dev_get_platdata(&cl->dev);
	}

	pchg = devm_kzalloc(&cl->dev, sizeof(*pchg), GFP_KERNEL);
	if (!pchg)
		return -ENOMEM;


	pchg->client = cl;//来源于结构体中
	pchg->dev = &cl->dev;
	pchg->pdata = pdata;				
	i2c_set_clientdata(cl, pchg);

	mutex_init(&pchg->xfer_lock);

	ret = IP5328P_init_device(pchg);//初始化设备
	if (ret) {
		dev_err(pchg->dev, "i2c communication err: %d", ret);
		return ret;
	}

	return 0;
}

static int IP5328P_remove(struct i2c_client *cl)
{
	struct IP5328P_chg *pchg = i2c_get_clientdata(cl);
	return 0;
}

static const struct of_device_id IP5328P_dt_ids[] = {
	{ .compatible = "injoinic,ip5328p", },
	{ }
};
MODULE_DEVICE_TABLE(of, IP5328P_dt_ids);

static const struct i2c_device_id IP5328P_ids[] = {
	{"IP5328P", 0},
	{ }
};
MODULE_DEVICE_TABLE(i2c, IP5328P_ids);

static struct i2c_driver IP5328P_driver = {
	.driver = {
		   .name = "IP5328P",
		   .of_match_table = of_match_ptr(IP5328P_dt_ids),
		   },
	.probe = IP5328P_probe,
	.remove = IP5328P_remove,
	.id_table = IP5328P_ids,
};
module_i2c_driver(IP5328P_driver);

MODULE_DESCRIPTION("TI/National Semiconductor IP5328P charger driver");
MODULE_AUTHOR("Milo Kim <milo.kim@ti.com>, Daniel Jeong <daniel.jeong@ti.com>");
MODULE_LICENSE("GPL");
