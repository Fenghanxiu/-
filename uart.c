#include "uart.h"

void UartInit(void)		//4800bps@12.000MHz
{
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x01;		//����1ѡ��ʱ��2Ϊ�����ʷ�����
	AUXR |= 0x04;		//��ʱ��ʱ��1Tģʽ
	T2L = 0x8F;		//���ö�ʱ��ʼֵ
	T2H = 0xFD;		//���ö�ʱ��ʼֵ
	AUXR |= 0x10;		//��ʱ��2��ʼ��ʱ
	ES = 1;
	EA = 1;
}

bit busy = 0;
unsigned char xdata uart_rx_buf[25] = {0};
unsigned char xdata uart_rx_cnt = 0;
unsigned char xdata uart_rx_flag = 0; //Ϊ1����Ϊ���յ���һ��ָ��
void UartIsr() interrupt 4
{
	if (TI) //�������
	{
		TI = 0;
		
		busy = 0;
	}
	
	if (RI) //�յ���
	{
		RI = 0;
		
		uart_rx_buf[uart_rx_cnt] = SBUF; //�ȱ������������
		
		if (uart_rx_buf[uart_rx_cnt] == '\n' && 
			uart_rx_buf[uart_rx_cnt - 1] == '\r') //�ж�֡β
		{
			uart_rx_flag = 1;									//�Ѿ�������һ��ָ��
			uart_rx_buf[++uart_rx_cnt] = 0;		//�ַ�������λ
		}
		
		if (++uart_rx_cnt >= 25)	//����һ�½��մ�С
		{
			uart_rx_cnt = uart_rx_flag = uart_rx_buf[0] = 0;
		}
	}
}

/*----------------------------
���ʹ�������
----------------------------*/
void SendData(uint8_t dat)
{
	while (busy);               //�ȴ�ǰ������ݷ������
	busy = 1;
	SBUF = dat;                 //д���ݵ�UART���ݼĴ���
}

/*----------------------------
�����ַ���
----------------------------*/
void SendString(char *s)
{
    while (*s)                  //����ַ���������־
    {
        SendData(*s++);         //���͵�ǰ�ַ�
    }
}