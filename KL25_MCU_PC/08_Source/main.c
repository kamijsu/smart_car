//说明见工程文件夹下的Doc文件夹内Readme.txt文件
//======================================================================

#define GLOBLE_VAR  //只需在main.c中定义一次，用来防止全局变量的重复定义

#include "includes.h"   //包含总头文件

int main(void)
{
	//1. 声明主函数使用的变量
	BreatheLightParams green_light;
	BreatheLightParams red_light;

	//2. 关总中断
	DISABLE_INTERRUPTS;

	//3. 初始化外设模块
	light_init(LIGHT_BLUE, LIGHT_OFF);				//蓝灯初始化
	breathe_light_init(BREATHE_LIGHT_GREEN, 0);		//初始化绿呼吸灯
	breathe_light_init(BREATHE_LIGHT_RED, 0);		//初始化红呼吸灯
	uart_init(UART_USE, 9600);			//初始化使用串口（在includes.h中定义），波特率为9600
	pit_init(PIT_CH0, 50);				//初始化PIT模块的0通道，每50ms产生一个中断

	//4. 给有关变量赋初值
	//消息队列队首与队尾下标初始为0
	g_msg.front = 0;
	g_msg.rear = 0;
	//时间标志清0
	g_time_flag.f_50ms = 0;
	g_time_flag.f_1s = 0;
	//绿呼吸灯未开启呼吸效果，占空比为0，正在变亮，每次变化大小为2.0
	green_light.is_start = 0;
	green_light.duty = 0;
	green_light.dir = 1;
	green_light.radio = 2.0;
	//红呼吸灯未开启呼吸效果，占空比为0，正在变亮，每次变化大小为2.0
	red_light.is_start = 0;
	red_light.duty = 0;
	red_light.dir = 1;
	red_light.radio = 2.0;

	//5. 使能模块中断
	uart_enable_re_int(UART_2);		//使能串口2接收中断
	pit_enable_int(PIT_CH0);		//使能PIT模块0通道中断

	//6. 开总中断
	ENABLE_INTERRUPTS;

	//======主循环开始======================================================
	for (;;)
	{
		//每1s执行一次的程序
		if (g_time_flag.f_1s)
		{
			//重置时间标志
			g_time_flag.f_1s = 0;
			//发送消息

		}

		//每50ms执行一次的程序
		if (g_time_flag.f_50ms)
		{
			//重置时间标志
			g_time_flag.f_50ms = 0;



		}

		//不断处理消息队列
		if (g_msg.front != g_msg.rear)
		{
			//获取队首消息并处理
			switch (g_msg.msg[g_msg.front++])
			{
			case '0':	//开启蓝灯
				light_control(LIGHT_BLUE, LIGHT_ON);
				break;
			case '1':	//关闭蓝灯
				light_control(LIGHT_BLUE, LIGHT_OFF);
				break;
			case '2':	//开启绿灯
				//关闭呼吸灯效果
				green_light.is_start = 0;
				//设置最亮
				breathe_light_set(BREATHE_LIGHT_GREEN, BREATHE_LIGHT_MAX_DUTY);
				break;
			case '3':	//关闭绿灯
				//关闭呼吸灯效果
				green_light.is_start = 0;
				//设置最暗
				breathe_light_set(BREATHE_LIGHT_GREEN, BREATHE_LIGHT_MIN_DUTY);
				break;
			case '4':	//开启红灯
				//关闭呼吸灯效果
				red_light.is_start = 0;
				//设置最亮
				breathe_light_set(BREATHE_LIGHT_RED, BREATHE_LIGHT_MAX_DUTY);
				break;
			case '5':	//关闭红灯
				//关闭呼吸灯效果
				red_light.is_start = 0;
				//设置最暗
				breathe_light_set(BREATHE_LIGHT_RED, BREATHE_LIGHT_MIN_DUTY);
				break;
			case '6':	//开启绿灯呼吸灯效果
				//开启呼吸灯效果
				green_light.is_start = 1;
				//获取绿呼吸灯当前亮度
				green_light.duty = breathe_light_get(BREATHE_LIGHT_GREEN);
				break;
			case '7':	//关闭绿灯呼吸灯效果
				//关闭呼吸灯效果
				green_light.is_start = 0;
				break;
			case '8':	//开启红灯呼吸灯效果
				//开启呼吸灯效果
				red_light.is_start = 1;
				//获取红呼吸灯当前亮度
				red_light.duty = breathe_light_get(BREATHE_LIGHT_RED);
				break;
			case '9':	//关闭红灯呼吸灯效果
				//关闭呼吸灯效果
				red_light.is_start = 0;
				break;
			}
			//防止越界
			if (g_msg.front == 16)
			{
				g_msg.front = 0;
			}
		}

	}  //主循环end_for
//======主循环结束======================================================
}

