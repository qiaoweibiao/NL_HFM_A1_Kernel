#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "usart2.h"
#include "includes.h"
#include "stdio.h"
#include "main.h"
#include "lcd.h"
#include "gui.h"
#include "test.h"
#include "ip5328p.h"
#include "i2c.h"
#include "powerControl.h"

//START 任务
#define START_TASK_PRIO      		10
#define START_STK_SIZE  				64
OS_STK START_TASK_STK[START_STK_SIZE];
void start_task(void *pdata);


//主任务
#define MAIN_TASK_PRIO       		4
#define MAIN_STK_SIZE  					128
OS_STK MAIN_TASK_STK[MAIN_STK_SIZE];
void main_task(void *pdata);

//电源管理任务
#define POWER_TASK_PRIO       		2
#define POWER_STK_SIZE  					256
OS_STK POWER_TASK_STK[POWER_STK_SIZE];
void power_task(void *pdata);

//电池管理任务
#define BATTERY_TASK_PRIO       		3
#define BATTERY_STK_SIZE  					256
OS_STK BATTERY_TASK_STK[BATTERY_STK_SIZE];
void battery_task(void *pdata);

OS_TMR   * tmr1;						//软件定时器1
OS_TMR   * tmr2;						//软件定时器2
OS_TMR   * tmr3;						//软件定时器3


//软件定时器1的回调函数,每100ms执行一次
void tmr1_callback(OS_TMR *ptmr,void *p_arg) 
{
	   
}

//软件定时器2的回调函数				  	   
void tmr2_callback(OS_TMR *ptmr,void *p_arg) 
{	
		
}
//软件定时器3的回调函数				  	   
void tmr3_callback(OS_TMR *ptmr,void *p_arg) 
{	
		
} 

int main(void)
{
 	delay_init();	    	 																			//延时函数初始化
	NVIC_Configuration();																			//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 			//开启AFIO时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//使能JTAGDisable，即禁用JTAG接口
	powerControl_init();																			//电源控制
	uart_init(115200);																				//初始化串口
	uart2_init(9600);																					//初始化串口2
	LCD_Init();																								//LCD初始化
	i2c_init();
	Show_Str(0,30,BLUE,BLACK," OUT2   TYPE-C    VIN    OUT1",16,0);
	
	OSInit();  	 						//初始化UCOSII
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	OSStart();							//UCOSII开始
}							    

void start_task(void *pdata)//开始任务
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 	
	
	OSStatInit();							//初始化统计任务.这里会延时1秒钟左右	
 	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)
	
 	OSTaskCreate(main_task,(void *)0,(OS_STK*)&MAIN_TASK_STK[MAIN_STK_SIZE-1],MAIN_TASK_PRIO);
	OSTaskCreate(power_task,(void *)0,(OS_STK*)&POWER_TASK_STK[POWER_STK_SIZE-1],POWER_TASK_PRIO);									//启动电源管理任务
	OSTaskCreate(battery_task,(void *)0,(OS_STK*)&BATTERY_TASK_STK[BATTERY_STK_SIZE-1],BATTERY_TASK_PRIO);					//启动电池管理任务
	
 	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();							//退出临界区(可以被中断打断)
}


u8 buf[20];
u8 mode;


void main_task(void *pdata)//主任务
{
 
//	u8 err;
// 	tmr1=OSTmrCreate(10,10,OS_TMR_OPT_PERIODIC,(OS_TMR_CALLBACK)tmr1_callback,0,"tmr1",&err);		//100ms执行一次
//	tmr2=OSTmrCreate(10,20,OS_TMR_OPT_PERIODIC,(OS_TMR_CALLBACK)tmr2_callback,0,"tmr2",&err);		//200ms执行一次
//	tmr3=OSTmrCreate(10,10,OS_TMR_OPT_PERIODIC,(OS_TMR_CALLBACK)tmr3_callback,0,"tmr3",&err);		//100ms执行一次
//	OSTmrStart(tmr1,&err);//启动软件定时器1
//	OSTmrStart(tmr2,&err);//启动软件定时器2
	

//		main_test(); 		//测试主界面
//		menu_test();     //3D菜单显示测试
//		Test_Color();  		//简单刷屏填充测试
//		Test_FillRec();		//GUI矩形绘图测试
//		Test_Circle(); 		//GUI画圆测试
//		Test_Triangle();    //GUI三角形绘图测试
//		English_Font_test();//英文字体示例测试
//		Chinese_Font_test();//中文字体示例测试
//		Pic_test();			//图片显示示例测试
//		Rotate_Test();   //旋转显示测试
	
/*-----------------------------------获取电池电压电流----------------------------------*/
		sprintf(buf, "BAT:%1.2fV  %1.2fA", IP5328P_BatOCV(),IP5328P_BatCurrent());						//电池两端的电压和电流
		Show_Str(0,0,BLUE,BLACK,buf,16,0);
/*-----------------------------------接口状态--------------------------------------*/		
		sprintf(buf, "  %1d      %1d        %1d      %1d  ",mos_state.OUT2,mos_state.TypeC,mos_state.VIN,mos_state.OUT1);		
		Show_Str(0,50,BLUE,BLACK,buf,16,0);
/*-----------------------------------获取接口电压----------------------------------*/
		sprintf(buf, " %2dV     %2dV     %2dV    %2dV",voltage.OUT2,voltage.TypeC,voltage.VIN,voltage.OUT1);
		Show_Str(0,70,BLUE,BLACK,buf,16,0);
/*-----------------------------------获取电池电压电流----------------------------------*/
		sprintf(buf, " %1.2fA  %1.2fA    %1.2fA  %1.2fA",current.OUT2,current.TypeC,current.VIN,current.OUT1);
		Show_Str(0,90,BLUE,BLACK,buf,16,0);
/*-----------------------------------获取电源功率----------------------------------*/
		sprintf(buf, "Power:%2.1fW   ",power);
		Show_Str(150,0,BLUE,BLACK,buf,16,0);
	
		while(1)
		{
		read_Parameters();										//获取参数

/*-----------------------------------获取电池电压电流----------------------------------*/
		sprintf(buf, "BAT:%1.2fV  %1.2fA", IP5328P_BatVoltage(),IP5328P_BatCurrent());						//电池两端的电压和电流
		Show_Str(0,0,BLUE,BLACK,buf,16,0);
/*-----------------------------------接口状态--------------------------------------*/		
		sprintf(buf, "  %1d      %1d        %1d      %1d  ",mos_state.OUT2,mos_state.TypeC,mos_state.VIN,mos_state.OUT1);		
		Show_Str(0,50,BLUE,BLACK,buf,16,0);
/*-----------------------------------获取接口电压----------------------------------*/
		sprintf(buf, " %2dV    %2dV      %2dV    %2dV",voltage.OUT2,voltage.TypeC,voltage.VIN,voltage.OUT1);
		Show_Str(0,70,BLUE,BLACK,buf,16,0);
/*-----------------------------------获取电池电压电流----------------------------------*/
		sprintf(buf, " %1.2fA  %1.2fA    %1.2fA  %1.2fA",current.OUT2,current.TypeC,current.VIN,current.OUT1);
		Show_Str(0,90,BLUE,BLACK,buf,16,0);
/*-----------------------------------获取电源功率----------------------------------*/
		sprintf(buf, "Power:%2.1fW   ",power);
		Show_Str(150,0,BLUE,BLACK,buf,16,0);
			
		delay_ms(100);


		}
}

void power_task(void *pdata)												//电源管理任务
{
	unsigned int Long_press=0;
	delay_ms(500);delay_ms(500);delay_ms(500);delay_ms(500);//刚上电电压不稳 延时等待电压稳定
		while(1)
		{
				if(power_read==Bit_RESET)//按下关机键 关机
				{
					Long_press++;
				}
				else
				{
					if(Long_press>30)
					{
						while(power_read==Bit_RESET)delay_ms(10);
						delay_ms(500);delay_ms(500);delay_ms(500);
						power_off();														//关闭电源
					}
				}
				delay_ms(10);

		}
}


void battery_task(void *pdata)											//电池管理任务
{
	while(1)
	{
		if(IP5328P_BatOCV()<=2.6)
		{
			delay_ms(500);delay_ms(500);
			delay_ms(500);delay_ms(500);
			delay_ms(500);delay_ms(500);
			power_off();														//关闭电源
		}
		else
		{
			delay_ms(500);delay_ms(500);
			delay_ms(500);delay_ms(500);
			delay_ms(500);delay_ms(500);
			delay_ms(500);delay_ms(500);
			delay_ms(500);delay_ms(500);
			delay_ms(500);delay_ms(500);
			delay_ms(500);delay_ms(500);
			delay_ms(500);delay_ms(500);
			delay_ms(500);delay_ms(500);
			delay_ms(500);delay_ms(500);
			delay_ms(500);delay_ms(500);
			delay_ms(500);delay_ms(500);
			delay_ms(500);delay_ms(500);
			delay_ms(500);delay_ms(500);
			power_off();														//关闭电源
		}	
		delay_ms(10);
	}
}


