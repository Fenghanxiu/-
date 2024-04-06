#include "hardware.h"

/* NE555频率计数器 */
void NE555Init(void)
{
	TMOD &= 0xF0;	//定时器0模式复位
	TMOD |= 0x04;	//设置为计数模式，计数引脚为P34
	TL0 = 0x00;		//复位初始值
	TH0 = 0x00;		//复位初始值
	TF0 = 0;			//清除TF0标志
	TR0 = 1;			//定时器0开始计时
}

/* ledpwm、数码管、继电器、蜂鸣器，以及各种周期延时均使用这个定时器 */
void Timer1Init(void)		//100微秒@12.000MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x50;		//设置定时初始值
	TH1 = 0xFB;		//设置定时初始值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	ET1 = 1;
}
/*led*/
uint8_t xdata led_buf = 0x00;//每个led的状态
//uint8_t xdata led_state = 0; 
uint8_t xdata led_pwm_cnt = 0;	//led的pwm计数器
uint8_t xdata led_pwm_duty = 10; //led的pwm占空比，默认满占空比，占空比不能超过周期
uint8_t xdata led_pwm_period = 10; //led的pwm周期（单位就是定时器的周期，这里是100us）

void led_output(uint8_t led_buf)
{
	P0 = ~led_buf;
	P2 |= 0x80;
	P2 &= 0x1f;
}

/*dig*/
/*************  本地常量声明    **************/
uint8_t code t_display[]={                       //标准字库
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
//	 0.   1.   2.   3.   4.   5.   6.   7.   8.   9.   -1
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};

uint8_t code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //位码

uint8_t xdata dig_com = 0; //当前显示的数码管位
uint8_t xdata dig_buf[SCREEN_NUM][8] = 
{
	{ 16, 16, 16, 16, 16, 16, 16, 16, },	//黑屏界面
	{ 25, 1, 16, 16, 16, 16, 16, 16, },	//界面1
	{ 25, 2, 16, 16, 16, 16, 16, 16, },	//界面2
	{ 25, 3, 16, 16, 16, 16, 16, 16, },	//界面3
	{ 25, 4, 16, 16, 16, 16, 16, 16, },	//界面4
	{ 25, 5, 16, 16, 16, 16, 16, 16, },	//界面5
	{ 25, 6, 16, 16, 16, 16, 16, 16, },	//界面6
};
uint8_t xdata *digs = dig_buf[0]; //当前显示界面
uint8_t xdata dig_switch = 0xFF; //数码管每一位的开关
void dig_output()
{
	static unsigned char dig_com = 0;
	
	P0 = 0x00; //消影
	P2 |= 0xC0;
	P2 &= ~0xE0;
	
	P0 = ~t_display[digs[dig_com]]; //段选
	P2 |= 0xE0;
	P2 &= ~0xE0;
	
	P0 = T_COM[dig_com]; //位选
	P2 |= 0xC0;
	P2 &= ~0xE0;
	
	if (++dig_com >= 8)
	{
		dig_com = 0;
	}
}

/*key*/
unsigned char key_value = 0; //按键键值
unsigned char key_state = 0; //按键状态
/* 每大概10ms调用一次 */
void key_scan_v2()
{
	P3 |= 0x0F;
	if (~P3 & 0x0F) // 如果有按键按下
	{
		/* 按键状态更新 */
		if (key_state < 255)
			key_state++;
		
		/* 按键键值判断 */
		if (P33 == 0)
			key_value = 4;
		if (P32 == 0)
			key_value = 5;
		if (P31 == 0)
			key_value = 6;
		if (P30 == 0)
			key_value = 7;
	}
	else //如果没有按键按下
	{
		key_value = 0;
		key_state = 0;
	}
}
//矩阵按键偷懒三行形式
unsigned char xdata ReadData=0;
unsigned char xdata trg=0;
unsigned char xdata cnt=0;
void KeyAction()  //测试长按键    独立式键盘 S4--S7
{
		static unsigned char cont_count = 0;	
		unsigned char xdata ReadData;
		ReadData = P3^0xff;
		trg = ReadData&(ReadData^cnt);//单次触发判断
		cnt = ReadData;//长按判断

		if(trg & 0x01)          	//按下S7
		{
//			freg += 1;
		}
		else if(trg & 0x02)     	//按下S6
		{
	//		freg -= 1;
		}		
		
		else if(trg & 0x04)     	//按下S5
		{
	//		freg -= 1;
		}		
		
		else if(trg & 0x08)     	//按下S4
		{
	//		freg -= 1;
		}		
		
		
		else if(cnt & 0x01)          //长按S7  1S
		{
			if(++cont_count == 100)
			{
					cont_count = 0;
	//				freg += 10;
			}
		}
}
/* 一般都是10ms左右调用一次，
	 10ms为消抖时间，可以根据需要更改 */
unsigned char code key_values[][4] = 
{
	{19, 18, 17, 16, }, 
	{15, 14, 13, 12, }, 
	{11, 10, 9, 8, }, 
	{7, 6, 5, 4, }, 
};
void key_pad_scan_v1()
{
	unsigned char R = 255, C = 255;
	
	P3 = 0x0F;
	P4 = 0x00;
	
	if (~P3 & 0x0F)
	{
		/* 按键状态更新 */
		if (key_state < 255)
			key_state++;
		
		/* 按键键值判断 */
		if (!P30) R = 0;
		if (!P31) R = 1;
		if (!P32) R = 2;
		if (!P33) R = 3;
		
		P3 = 0xF0;
		P4 = 0xFF;
		
		if (!P34) C = 0;
		if (!P35) C = 1;
		if (!P42) C = 2;
		if (!P44) C = 3;
		
		if (R != 255 && C != 255)
		{
			key_value = key_values[C][R];
		}
		else
		{
			key_value = key_state = 0;
		}
	}
	else
	{
		key_value = key_state = 0;
	}
}

void key_pad_scan_v2()//使用串口时，不扫描串口P30,P31||使用Ne555时，不扫描P34
{
	unsigned char R = 255, C = 255;
	
//	P3 = 0x0F;
	P3 |= 0x08;
	P3 &= ~0xF0;
	P4 = 0x00;
	
	if (~P3 & 0x0F)
	{
		/* 按键状态更新 */
		if (key_state < 255)
			key_state++;
		
		/* 按键键值判断 */
		if (!P30) R = 0;
		if (!P31) R = 1;
		if (!P32) R = 2;
		if (!P33) R = 3;
		
//		P3 = 0xF0;
		P3 |= 0xF0;
		P3 &= ~0x08;
		P4 = 0xFF;
		
		if (!P34) C = 0;
		if (!P35) C = 1;
		if (!P42) C = 2;
		if (!P44) C = 3;
		
		if (R != 255 && C != 255)
		{
			key_value = key_values[C][R];
		}
		else
		{
			key_value = key_state = 0;
		}
	}
	else
	{
		key_value = key_state = 0;
	}
}
void key_pad_scan_v3()//长短按
{
	unsigned char R = 255, C = 255;
	
//	P3 = 0x0F;
	P3 |= 0x08;
	P3 &= ~0xF0;
	P4 = 0x00;
	
	if (~P3 & 0x0F)
	{
		/* 按键状态更新 */
		if (key_state < 255)
			key_state++;
		
		/* 按键键值判断 */
		if (!P30) R = 0;
		if (!P31) R = 1;
		if (!P32) R = 2;
		if (!P33) R = 3;
		
//		P3 = 0xF0;
		P3 |= 0xF0;
		P3 &= ~0x08;
		P4 = 0xFF;
		
		if (!P34) C = 0;
		if (!P35) C = 1;
		if (!P42) C = 2;
		if (!P44) C = 3;
		
		if (R != 255 && C != 255)
		{
			key_value = key_values[C][R];
		}
		else
		{
			key_value = key_state = 0;
		}
	}
	else
	{
		if (key_value && key_state <= 250) //如果按键刚松开
		{
			if (key_state < 100) //若按下到松手时间没有超过1秒
				key_state = 255;
			else if (key_state < 200) //若按下到松手时间超过1秒而没超过2秒
				key_state = 254;
			else //若按下到松手时间超过2秒
				key_state = 253;
		}
		else //按键完全松开
		{
			key_value = 0;
			key_state = 0;
		}
	}
}

/* 大功率外设状态 */
uint8_t xdata high_power_state = 0;

/* 虚拟定时器 */
uint8_t xdata Timer10ms_cnt = 0;
uint8_t xdata Timer50ms_cnt = 0;
uint8_t xdata Timer100ms_cnt = 0;
uint16_t xdata Timer500ms_cnt = 0;
uint16_t xdata Timer1s_cnt = 0;
uint16_t xdata Timern_cnt = 0;
uint16_t xdata operate_timer_cnt = 0; //操作周期定时器，用来错开每个时序操作防止时序混乱，需要根据操作的芯片数量来决定定时器周期

/* NE555频率 */
uint16_t xdata ne555_out = 0;

void Timer1Isr(void) interrupt 3 //100us一周期
{
	static uint8_t Timer1ms_cnt = 10;
	static uint16_t ne555_cnt = 100; //ne555专用的定时器，可以调整采样周期

	{ //LED_PWM
		if (led_pwm_cnt <= led_pwm_duty) //有效电平
		{
			CHANNLE(4, ~led_buf);
			//led_output(led_buf);
		}
		else //无效电平
		{
			CHANNLE(4, 0xFF); //关闭LED
			//led_output(0x00);
		}
		if (++led_pwm_cnt >= led_pwm_period)
			led_pwm_cnt = 0;
	}
	
	if (++Timer1ms_cnt >= 10) //1ms
	{
		Timer1ms_cnt = 0;
		
		Timer10ms_cnt++;
		Timer50ms_cnt++;
		operate_timer_cnt++;
		Timer100ms_cnt++;
		Timer500ms_cnt++;
		Timer1s_cnt++;
		Timern_cnt++;
		
		{ //数码管
			CHANNLE(6, 0x00);
			CHANNLE(7, ~t_display[digs[dig_com]]); //关键步骤
			CHANNLE(6, T_COM[dig_com] & dig_switch); //关键步骤 根据这一位数码管开关来决定是否显示
			if (++dig_com >= 8)
				dig_com = 0;
		}
		//dig_output();
		if (++ne555_cnt >= 100)
		{
			ne555_cnt = 0;
			
			TR0 = 0;		//定时器0暂停计数
			ne555_out = TH0 << 8 | TL0;
			TL0 = 0x00;	//复位初始值
			TH0 = 0x00;	//复位初始值
			TR0 = 1;		//定时器0开始计数
//			ne555_out *= 10;
		}
		
		//大功率外设
		CHANNLE(5, high_power_state);
	}
}


