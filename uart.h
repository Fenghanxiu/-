#ifndef __UART_H__
#define __UART_H__

#include <STC15F2K60S2.H>
#include "intrins.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include "hardware.h"

extern bit busy;
extern unsigned char xdata uart_rx_buf[25];
extern unsigned char xdata uart_rx_cnt;
extern unsigned char xdata uart_rx_flag; //为1定义为接收到了一个指令

void UartInit(void);
void SendData(uint8_t dat);
void SendString(char *s);

#endif
