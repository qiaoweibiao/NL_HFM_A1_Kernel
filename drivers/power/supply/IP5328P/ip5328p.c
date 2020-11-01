/****                                                                 ****/
/****  说明：  根据IP5328P (with reg) V1.0编写                        ****/
/****          寄存器手册来自51黑电子论坛 ID：没有你                  ****/
/****          程序功能并不完全，仅写自认常用功能函数，可自行扩展     ****/
/****  作者：  超级网吧                                               ****/
/****  时间：  2020-10-04                                             ****/
/****                                                                 ****/

#include "i2c.h"
#include "ip5328p.h"

//写寄存器(8bit)
//reg:寄存器地址
//data:待写入数据
void IP5328P_Write(unsigned char reg,unsigned char data)
{
	I2CStart();		    	//起始信号
	I2CSendByte(0xEA);		//写从机地址
	I2CWaitAck();			//等待应答
	I2CSendByte(reg);	   	//待写入数据寄存器地址
	I2CWaitAck();			//等待应答
	I2CSendByte(data);	   	//写入数据
	I2CWaitAck();			//等待应答
	I2CStop();				//停止信号
}

//读寄存器(8bit)
//reg:寄存器地址
//data:待写入数据
unsigned char IP5328P_Read(unsigned char reg)
{
	unsigned char R_dat;
	I2CStart();		    			//起始信号
	I2CSendByte(0xEA);				//写从机地址
	I2CWaitAck();					//等待应答(在未收到应答时 未做处理 可自行添加)

	I2CSendByte(reg);	   			//待写入数据寄存器地址
	I2CWaitAck();					//等待应答(在未收到应答时 未做处理 可自行添加)

	I2CStart();						//当作重启信号	

	I2CSendByte(0xEB);	   			//写入从机地址 开启读操作
	I2CWaitAck();					//等待应答 (在未收到应答时 未做处理 可自行添加)
	R_dat=I2CReceiveByte();			//接收8bit数据
	I2CSendNotAck();				//发送mNACK信号 停止接收
	I2CStop();						//停止信号
	return R_dat;
}


//读取电池电量显示级别(只能读到对应电量的LED个数)
//返回：0001 1111:4课灯亮  0000 1111:3课灯亮  0000 0111:2课灯亮	0000 0011:1课灯亮
//		0000 0001:放电时低电闪灯 0000 0000:关机
unsigned char IP5328P_Electricity(void)
{
	int dat;
	dat=IP5328P_Read(0xDB);	  				//获取电量指示级别
	return dat;
}

/**************************************************读取芯片参数功能函数********************************************************/

//读取电池两端的真实电压值
//返回float型的电压值(小数)
float IP5328P_BatVoltage(void)
{
	int dat;
	float BatVoltage=0.0f;
	dat=IP5328P_Read(0x64);	  				//获取电池电压低8位
	BatVoltage += dat;
	dat=IP5328P_Read(0x65);			 		//获取电池电压高8位
	BatVoltage = (dat<<8)+BatVoltage;		//高低位合并
	if(BatVoltage==0xffff) return 0;		//如果读到0xffff，此时芯片未激活 读取值错误
	BatVoltage = BatVoltage*0.00026855+2.6;	//计算为实际电压值
	return BatVoltage;
}

//读取电池输入或输出电流
//返回float型的电流值(小数)	 单位ma
float IP5328P_BatCurrent(void)
{
	int dat;
	float Current=0.0f;
	dat=IP5328P_Read(0x66);	  				//获取电池电流低8位
	Current += dat;
	dat=IP5328P_Read(0x67);			 		//获取电池电流高8位
	Current = (dat<<8)+Current;				//高低位合并
	if(Current>32767)Current=65535-Current;	//将负值电流转为正值
	Current = Current*0.00127883;			//计算为实际电流值
	return Current;
}

//读取电池电压经过电芯内阻和电芯电流进行补偿后的电压
//返回float型的电压值(小数)
float IP5328P_BatOCV(void)
{
	int dat;
	float BatVoltage=0.0f;
	dat=IP5328P_Read(0x7A);	  				//获取电池电压低8位
	BatVoltage += dat;
	dat=IP5328P_Read(0x7B);			 		//获取电池电压高8位
	BatVoltage = (dat<<8)+BatVoltage;		//高低位合并
	if(BatVoltage==0xffff) return 0;		//如果读到0xffff，此时芯片未激活 读取值错误
	BatVoltage = BatVoltage*0.00026855+2.6;	//计算为实际电压值
	return BatVoltage;
}

//获取Type-C接口连接状态
//返回：0x00未连接
//		0x01连接手机等需供电设备(充电宝在放电)
//		0x02连接电源适配器(充电宝在充电)
unsigned char IP5328P_TypeC_Flag(void)
{
	unsigned char flag=0,dat=0;
    dat=IP5328P_Read(0xB8);
	if(dat==0xff)return 0;					//如果读到0xff，芯片未激活
	if(dat>>1&0x01)flag=0x01;
	if(dat>>5&0x01)flag=0x02;
	return flag;
}

//获取Type-C连接的适配器输出能力
//返回：0x00 芯片未激活
//		0x01标准USB
//		0x02输出能力1.5A
//		0x03输出能力3.0A
unsigned char IP5328P_TypeC_Ability(void)
{
	unsigned char flag=0,dat=0;
    dat=IP5328P_Read(0xFF);
	if(dat==0xff)return 0;					//如果读到0xff，芯片未激活
	if(dat>>5&0x01)flag=0x01;
	if(dat>>6&0x01)flag=0x02;
	if(dat>>7&0x01)flag=0x03;
	return flag;
}


/********************以下电流读取有条件限制*********************************/
/*********大体可以理解为当有多个接口同时开启时才可以读取********************/
/***************其中，VBUS是TYPE-C口，VIN是安卓口***************************/

//读取Type-C输入或输出电流
//返回float型的电流值(小数)	 单位ma
//条件：需要在充电状态 VINOK 和 VBUSOK 同时有效且 VBUS MOS开启时；
//      或者VBUS MOS开启的同时有其他MOS也开启时,该ADC才会启动
//		(以上符号状态都可用寄存器读取)
float IP5328P_TypeC_Current(void)
{
	int dat;
	float Current=0.0f;
	dat=IP5328P_Read(0x6E);	  				//获取电流低8位
	Current += dat;
	dat=IP5328P_Read(0x6F);			 		//获取电流高8位
	Current = (dat<<8)+Current;				//高低位合并
	if(Current>32767)Current=65535-Current;	//将负值电流转为正值
	Current = Current*0.0006394;			//计算为实际电流值
	return Current;
}

//读取安卓口输入电流
//返回float型的电流值(小数)	 单位ma
//条件：需要在充电状态 VINOK 和VBUSOK 同时有效且 VIN MOS开启时该 ADC才会启动
//		(以上符号状态都可用寄存器读取)
float IP5328P_VIN_Current(void)
{
	int dat;
	float Current=0.0f;
	dat=IP5328P_Read(0x6D);	  				//获取电流低8位
	Current += dat;
	dat=IP5328P_Read(0x6C);			 		//获取电流高8位
	Current = (dat<<8)+Current;				//高低位合并
	if(Current>32767)Current=65535-Current;	//将负值电流转为正值
	Current = Current*0.0006394;			//计算为实际电流值
	return Current;
}

//读取OUT1输出电流
//返回float型的电流值(小数)	 单位ma
//条件：需要在 OUT1 MOS 开启的同时有其他 MOS 也开启时，该 ADC 才会启动；
//		(以上符号状态都可用寄存器读取)
float IP5328P_OUT1_Current(void)
{
	int dat;
	float Current=0.0f;
	dat=IP5328P_Read(0x70);	  				//获取电流低8位
	Current += dat;
	dat=IP5328P_Read(0x71);			 		//获取电流高8位
	Current = (dat<<8)+Current;				//高低位合并
	if(Current>32767)Current=65535-Current;	//将负值电流转为正值
	Current = Current*0.0006394;			//计算为实际电流值
	return Current;
}

//读取OUT2输出电流
//返回float型的电流值(小数)	 单位ma
//条件：需要在 OUT1 MOS 开启的同时有其他 MOS 也开启时，该 ADC 才会启动；
//		(以上符号状态都可用寄存器读取)
float IP5328P_OUT2_Current(void)
{
	int dat;
	float Current=0.0f;
	dat=IP5328P_Read(0x72);	  				//获取电流低8位
	Current += dat;
	dat=IP5328P_Read(0x73);			 		//获取电流高8位
	Current = (dat<<8)+Current;				//高低位合并
	if(Current>32767)Current=65535-Current;	//将负值电流转为正值
	Current = Current*0.0006394;			//计算为实际电流值
	return Current;
}

/********************以上电流读取有条件限制*********************************/

//获取当前电源功率
//返回float型的功率值(小数)
float IP5328P_Power(void)
{
	int dat;
	float Power=0.0f;
	dat=IP5328P_Read(0x7C);	  				//获取功率低8位
	Power += dat;
	dat=IP5328P_Read(0x7D);			 		//获取功率高8位
	Power = (dat<<8)+Power;					//高低位合并
	if(Power==0xffff) return 0;				//如果读到0xffff，此时芯片未激活 读取值错误
	Power = Power*0.00844;					//计算为实际功率值
	return Power;
}

//获取芯片当前电源状态
//第四位：0:放电 1:充电
//2:0位 ：000:待机 001:5V充电 010:单口同充同放 011:多口同充同放
//		  100:高压快充充电 101:5V放电 110:多口5V放电 111:高压快充放电
//其他位：无效位
unsigned char IP5328P_SYS_Status(void)
{
	unsigned char flag=0;
	flag=IP5328P_Read(0xD1);
	if(flag==0xff)return 0;
	return flag;
}


//获取输入电压有效状态以及按键是否被按下
//返回：7:6无效位
//		5 VBUSOK: 1 TYPE-C接口电压有效（充放电都会有效） 0 接口电压无效
//		4 VINOK : 1 安卓口接口电压有效	                 0 安卓口接口电压无效
//		3:1无效位
//		0 key_in: 0按键按下    1按键未按下
unsigned char IP5328P_KEY_IN(void)
{
	unsigned char flag=0;
	flag=IP5328P_Read(0xD2);
	if(flag==0xff)return 0;
	return flag;
}


//获取安卓口和TYPE-C的输入电压
//返回：7:6无效位
//		5:3  TYPE-C_STATE:000-5V,001-7V,011-9V,111-12V 
//		2:0  VIN_STATE   :000-5V,001-7V,011-9V,111-12V
unsigned char IP5328P_VinTypeC_State(void)
{
	unsigned char flag=0;
	flag=IP5328P_Read(0xD5);
	if(flag==0xff)return 0;
	return flag;
}

//获取充电状态
//返回：7:	0可能刚好是在停充检测，也可能是充满了，也可能是异常保护了	 1 正在充电
//		6:  0未充满  1充满 
//		5:  0恒压恒流总计时未超时   1恒压恒流总计时超时
//		4:  0恒压计时未超时         1恒压计时超时
//		3:  0涓流计时未超时         1涓流计时超时
//		2:0 000空闲  001涓流充电  010恒流充电  011恒压充电 100停充检测 101电池充满结束 110超时
unsigned char IP5328P_GHG_State(void)
{
	unsigned char flag=0;
	flag=IP5328P_Read(0xD7);
	return flag;
}


//获取MOS状态
//返回：7:	0当前安卓口在充电	      1当前TYPE-C在充电
//		6:  0安卓口电压无效           1安卓口电压有效  
//		5:  0TYPE-C电压无效           1TYPE-C电压有效 
//		4:  0VIN MOS(安卓)未开启      1VIN MOS(安卓)开启
//		3:  无效位
//		2:  0VBUS MOS(TYPE-C)未开启   1VBUS MOS(TYPE-C)开启
//		1:  0VOUT2 MOS未开启          1VOUT2 MOS开启
//		0:  0VOUT1 MOS未开启          1VOUT1 MOS开启
unsigned char IP5328P_MOS_ON(void)
{
	unsigned char flag=0;
	flag=IP5328P_Read(0xE5);
	if(flag==0xff)return 0;
	return flag;
}


//获取升压输出电压值范围
//返回：7:4	无效位 
//		3:  0无效      1输出电压10-12V
//		2:  0无效      1输出电压8-10V
//		1:  0无效      1输出电压6-8V
//		0:  0非快充    1快充
unsigned char IP5328P_BOOST(void)
{
	unsigned char flag=0;
	flag=IP5328P_Read(0xFB);
	if(flag==0xff)return 0;
	return flag;
}

//获取QC快充是否使能(不是是否正在使用，而是说这个功能可以用)
//返回：7:4	无效位 
//		3:  0非快充      1TYPE-C快充使能
//		2:  0非快充      1安卓口快充使能
//		1:  0非快充      1OUT2快充开使能
//		0:  0非快充      1OUT1快充开使能
unsigned char IP5328P_QC_State(void)
{
	unsigned char flag=0;
	flag=IP5328P_Read(0x3E);
	if(flag==0xff)return 0;
	return flag;
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
unsigned char IP5328P_DCP_DIG(void)
{
	unsigned char flag=0;
	flag=IP5328P_Read(0xA2);
	if(flag==0xff)return 0;
	return flag;
}

/*----------------------------------------------读取芯片参数功能函数-end-----------------------------------------------------*/

/**************************************************写芯片参数功能函数********************************************************/

//设置电池低电关机电压(关机后需重新充电才可启动)
//输入：0x30 3.00V-3.10V
//      0x20 2.90V-3.00V
//      0x10 2.81V-2.89V
//      0x00 2.73V-2.81V
void IP5328P_BAT_LOW(unsigned char dat)
{
	if(dat==0x30||dat==0x20||dat==0x10||dat==0x00)//为保证器件不出错，确认指令正确后在写入
	IP5328P_Write(0x10,dat);
}

//设置SYS4(测试，不好用)
//输入：chg2bst：拔出充电自动开启升压输出                0不开启  1开启
//      swclk2 ：使能I2C2待机时钟（使能后待机可读数据）  0不开启  1开启
//      swclk1 ：使能I2C1待机时钟（使能后待机可读数据）  0不开启  1开启
void IP5328P_SYS_CTL14(unsigned char chg2bst,unsigned char swclk2,unsigned char swclk1)
{
	unsigned char dat=0x00;
	if(chg2bst)dat|=0x40;
	if(swclk2)dat|=0x08;
	if(swclk1)dat|=0x04;
	IP5328P_Write(0x0E,dat);
}




/*------------------------------------------------写芯片参数功能函数-end----------------------------------------------------*/

/*---------------------------------------------------获取常用参数-----------------------------------------------------------*/

float power;						//总功率
Interface mos_state;	  //各接口开启状态结构体
Interface current;			//各接口电流结构体
Interface voltage;			//各接口电压结构体

//获取常用参数
//包括:总功率、各接口电压、各接口电流、各接口开启状态
void read_Parameters(void)
{
	unsigned char mos,Sys_status,Bat_Grade,GHG_State,TypeC_VIN_voltage,boost;	
/*----------------------------------------------获取参数----------------------------------------------------------*/	
	mos=IP5328P_MOS_ON();				 									//获取接口状态
	power=IP5328P_Power();			     									//获取功率
	TypeC_VIN_voltage=IP5328P_VinTypeC_State();	 							//获取安卓口和TYPE-C电压
	current.TypeC=IP5328P_TypeC_Current();									//获取Type-C电流值
	current.VIN=IP5328P_VIN_Current();										//获取VIN电流值
	current.OUT1=IP5328P_OUT1_Current();									//获取VOUT1电流值
	current.OUT2=IP5328P_OUT2_Current();									//获取VOUT2电流值
	boost=IP5328P_BOOST();													//获取升压值
//	Sys_status=IP5328P_SYS_Status();										//获取充电状态
//	Bat_Grade=IP5328P_Electricity();										//获取电量等级
//	GHG_State=IP5328P_GHG_State();											//获取充电状态
/*--------------------------------------------处理接口状态--------------------------------------------------------*/
	mos_state.OUT1=mos&0x01;												//获得OUT1开启状态 [因为按下按键激活时，OUT1会开启输出，但可能并没有连接]
	mos_state.OUT2=mos>>1&0x01;												//获得OUT2开启状态
	mos_state.TypeC=mos>>2&0x01;											//获得TypeC开启状态
	mos_state.VIN=mos>>4&0x01;												//获得VIN开启状态
	if(mos_state.OUT1)														//纠正OUT1开启状态 [因为按下按键激活时，OUT1会开启输出，但可能并没有连接]
	{
		if(power<=0.2)mos_state.OUT1=0;										//如果功率为0则未连接
		if(power>0)															//如果功率＞0
			if(mos_state.OUT2||mos_state.TypeC||mos_state.VIN)				//且有其他接口开启
				if(current.OUT1==0)mos_state.OUT1=0;						//如果OUT1电流还是为0，则证明OUT1接口实际未开启
	}
/*--------------------------------------------处理接口电压--------------------------------------------------------*/
	if(mos_state.VIN){														//如果VIN接口是开启的
		switch(TypeC_VIN_voltage&0x07)										//VIN电压
		{
			case 0x07:voltage.VIN=12;break;									//12V
			case 0x03:voltage.VIN=9;break;									//9V
			case 0x01:voltage.VIN=7;break;									//7V
			case 0x00:voltage.VIN=5;break;									//5V
		}																	
	}else voltage.VIN=0;													//其他
	if(mos_state.TypeC){													//如果TYPE-C接口是开启的
		switch(TypeC_VIN_voltage>>3&0x07)									//TYPE-C电压
		{
			case 0x07:voltage.TypeC=12;break;								//12V
			case 0x03:voltage.TypeC=9;break;								//9V
			case 0x01:voltage.TypeC=7;break;								//7V
			case 0x00:voltage.TypeC=5;break;								//5V
		}
	}else voltage.TypeC=0;													//其他
	if(mos_state.OUT1){														//如果接口开启
		if(current.OUT1)voltage.OUT1=5;										//如果接口有电流
		else{																//如果接口无电流
			 if(boost>1&0x01)voltage.OUT1=5;								//电压值就是升压值
			 if(boost>2&0x01)voltage.OUT1=9;								//电压值就是升压值
			 if(boost>3&0x01)voltage.OUT1=12;								//电压值就是升压值
		}																	
	}else voltage.OUT1=0;													//如果接口未开启
	if(mos_state.OUT2){														//如果接口开启
		if(current.OUT2)voltage.OUT2=5;										//如果接口有电流
		else{																//如果接口无电流
			 if(boost>1&0x01)voltage.OUT2=5;								//电压值就是升压值
			 if(boost>2&0x01)voltage.OUT2=9;								//电压值就是升压值
			 if(boost>3&0x01)voltage.OUT2=12;								//电压值就是升压值
		}																	
	}else voltage.OUT2=0;													//如果接口未开启
/*--------------------------------------------处理接口电流--------------------------------------------------------*/
	if(current.OUT1==0&&mos_state.OUT1)current.OUT1=power/voltage.OUT1;		//如果电流为0，可能是仅插入一个接口，此时电流需要用功率换算
	if(current.OUT2==0&&mos_state.OUT2)current.OUT2=power/voltage.OUT2;		//如果电流为0，可能是仅插入一个接口，此时电流需要用功率换算
	if(current.TypeC==0&&mos_state.TypeC)current.TypeC=power/voltage.TypeC;	//如果电流为0，可能是仅插入一个接口，此时电流需要用功率换算
	if(current.VIN==0&&mos_state.VIN)current.VIN=power/voltage.VIN;			//如果电流为0，可能是仅插入一个接口，此时电流需要用功率换算
/*--------------------------------------------处理xxxx--------------------------------------------------------*/

}

/*---------------------------------------------------获取常用参数-----------------------------------------------------------*/











