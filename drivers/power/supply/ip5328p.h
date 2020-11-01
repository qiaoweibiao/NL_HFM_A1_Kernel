#ifndef  __IP5328P_H__
#define  __IP5328P_H__


extern float power;				//总功率

typedef struct{
	unsigned char OUT1;		   	//OUT1接口
	unsigned char OUT2;		   	//OUT2接口
	unsigned char TypeC;	   	//TypeC接口
	unsigned char VIN;		   	//VIN(安卓口)
}Interface;

extern Interface mos_state;	  //各接口开启状态结构体
extern Interface current;			//各接口电流结构体
extern Interface voltage;			//各接口电压结构体





//基础操作函数
void IP5328P_Write(unsigned char reg,unsigned char data);	 //写IP5328P寄存器数据
unsigned char IP5328P_Read(unsigned char reg);				 //读IP5328P寄存器数据

//功能函数
/**************************************读参数*****************************************************************/
float IP5328P_BatVoltage(void);		 						 //读电池两端电压值
float IP5328P_BatOCV(void);									 //电池电压经过电芯内阻和电芯电流进行补偿后的电压
float IP5328P_BatCurrent(void);								 //读取电池电流
float IP5328P_TypeC_Current(void);							 //读取TYPE-C输入输出电流
float IP5328P_VIN_Current(void);							 //读取安卓口的输入电流
float IP5328P_OUT1_Current(void);							 //读取OUT1输出电流
float IP5328P_OUT2_Current(void);							 //读取OUT2输出电流
float IP5328P_Power(void);									 //获取功率
unsigned char IP5328P_TypeC_Flag(void);						 //获取Type-C接口连接状态
unsigned char IP5328P_TypeC_Ability(void);					 //获取Type-C连接的适配器输出能力
unsigned char IP5328P_Electricity(void);					 //获取电量指示级别
unsigned char IP5328P_SYS_Status(void);						 //获取电源状态
unsigned char IP5328P_VinTypeC_State(void);				 	 //获取安卓口和TYPE-C的输入电压
unsigned char IP5328P_GHG_State(void);						 //获取充电状态
unsigned char IP5328P_MOS_ON(void);							 //获取MOS开启状态
unsigned char IP5328P_BOOST(void);							 //获取升压输出电压值范围
unsigned char IP5328P_QC_State(void);						 //获取QC快充是否使能(不是是否正在使用，而是说这个功能是否可以用)
unsigned char IP5328P_DCP_DIG(void);						 //获取获取快充是否使能(不是是否正在使用，而是说这个功能是否可以用)

/**************************************写参数*****************************************************************/
void IP5328P_BAT_LOW(unsigned char dat);	 				 //设置电池低电关机电压
void IP5328P_SYS_CTL14(unsigned char chg2bst,unsigned char swclk2,unsigned char swclk1);//设置SYS4

/***********************************获取常用参数***************************************************************/
void read_Parameters(void);															 

#endif






