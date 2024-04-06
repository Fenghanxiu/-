#include "uart.h"

void UartInit(void)		//4800bps@12.000MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0x8F;		//设置定时初始值
	T2H = 0xFD;		//设置定时初始值
	AUXR |= 0x10;		//定时器2开始计时
	ES = 1;
	EA = 1;
}

bit busy = 0;
unsigned char xdata uart_rx_buf[25] = {0};
unsigned char xdata uart_rx_cnt = 0;
unsigned char xdata uart_rx_flag = 0; //为1定义为接收到了一个指令
void UartIsr() interrupt 4
{
	if (TI) //发送完成
	{
		TI = 0;
		
		busy = 0;
	}
	
	if (RI) //收到了
	{
		RI = 0;
		
		uart_rx_buf[uart_rx_cnt] = SBUF; //先保存读到的数据
		
		if (uart_rx_buf[uart_rx_cnt] == '\n' && 
			uart_rx_buf[uart_rx_cnt - 1] == '\r') //判断帧尾
		{
			uart_rx_flag = 1;									//已经读到了一个指令
			uart_rx_buf[++uart_rx_cnt] = 0;		//字符串截至位
		}
		
		if (++uart_rx_cnt >= 25)	//限制一下接收大小
		{
			uart_rx_cnt = uart_rx_flag = uart_rx_buf[0] = 0;
		}
	}
}

/*----------------------------
发送串口数据
----------------------------*/
void SendData(uint8_t dat)
{
	while (busy);               //等待前面的数据发送完成
	busy = 1;
	SBUF = dat;                 //写数据到UART数据寄存器
}

/*----------------------------
发送字符串
----------------------------*/
void SendString(char *s)
{
    while (*s)                  //检测字符串结束标志
    {
        SendData(*s++);         //发送当前字符
    }
}