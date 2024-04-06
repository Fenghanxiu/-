#include <STC15F2K60S2.H>
#include "hardware.h"
#include "onewire.h"
#include "iic.h"
#include "ds1302.h"

#include "uart.h"
#include "echo.h"
#include "uart.h"

#include "intrins.h"
#include "string.h"
#include "stdio.h"
void Delay100ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 5;
	j = 144;
	k = 71;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void main()
{
	unsigned char xdata work_flag = 0;
	
	unsigned int xdata temperature; //�¶ȵ�һ�ٱ�
	unsigned char xdata dac_out_flag = 1;
	
	unsigned char xdata screen = 0;
	unsigned char xdata data_disp = 0;
	unsigned char xdata param_disp = 0;
	
	unsigned char xdata key_value_bak = 0;
  unsigned char xdata key_state_bak = 0;
	
	unsigned char xdata temp_param		 = 30;
	unsigned char xdata temp_param_bak = 30;
	unsigned char xdata dist_param		 = 35; 	//��λcm
	unsigned char xdata dist_param_bak = 35; 	//��λcm
	unsigned int xdata param_change	 = 0;
	unsigned int xdata distance=0;
	
	
	char xdata uart_tx_buf[25] = {0};
	
	P0 = 0xFF;
	P2 = 0x80;
	P2 = 0xA0;
	P0 = 0x00;
	P2 = 0x00;
	
	e2prom_read(0x21,&param_change,1);

	Timer1Init();	//������led dig timer��
	
	UartInit();

	while (1)
	{
		if (Timer10ms_cnt >= 10)
		{
			Timer10ms_cnt = 0;

			/* input */
			key_pad_scan_v2();

			/* process */
			/* �߼����� */
			if (Timer50ms_cnt >= 50)
			{
				Timer50ms_cnt = 0;
				
				if (work_flag == 0)
				{
					read_temp();
				}
				else if (work_flag == 1)
				{
					distance=Get_Csb();
				}
				else if (work_flag == 2)
				{
					if (dac_out_flag)
					{
						write_pcf8591((distance / 10) <= dist_param ? 102 : 204);
					}
					else
					{
						write_pcf8591(20);
					}
				}
				else if (work_flag == 3)
				{
					if (uart_rx_flag) //�Ƿ���յ�һ��ָ��
					{
						uart_rx_flag = 0;
						uart_rx_cnt = 0;
						
						if (strcmp(uart_rx_buf, "ST\r\n") == 0) //��ѯ����ָ��
						{
							sprintf(uart_tx_buf, "$%u,%.2f\r\n", distance / 10, (float)temperature / 100.0f);
							SendString(uart_tx_buf);
						}
						else if (strcmp(uart_rx_buf, "PARA\r\n") == 0)
						{
							sprintf(uart_tx_buf, "#%u,%u\r\n", (unsigned int)dist_param, (unsigned int)temp_param);
							SendString(uart_tx_buf);
						}
						else
						{
							strcpy(uart_tx_buf, "ERROR\r\n");
							SendString(uart_tx_buf);
						}
					}
//					UartSandNByte("hello\r\n", 7);
				}
				else if (work_flag == 4)
				{
					e2prom_write(0x21, &param_change,1);
				}
				
				if (++work_flag >= 4)
				{
					work_flag = 0;
				}
			}
			
			/* �����жϣ������жϣ� */
			if (key_value) //��ʱ����δ����
			{
				key_value_bak = key_value;
				key_state_bak = key_state;
			}
			else //��ʱ��������
			{
				if (key_state_bak < 100) //�̰�
				{
					if (key_value_bak == 13) //�л�����
					{
						screen = !screen;
						
						data_disp = 0;
						param_disp = 0;
						
						if (screen == 0) //�˻ص����ݽ���ʱ
						{
							if (temp_param != temp_param_bak || 
								dist_param != dist_param_bak) //������������ı�
							{
								temp_param = temp_param_bak; //��������
								dist_param = dist_param_bak;
								
								param_change++; //�����䶯����++
								
								work_flag = 4;
							}
						}
					}
					
					if (screen == 0) //���ݽ���
					{
						if (key_value_bak == 12) //�л���ʾ
						{
							if (++data_disp >= 3)
							{
								data_disp = 0;
							}
						}
					}
					else //��������
					{
						if (key_value_bak == 12) //�л�����
						{
							param_disp = !param_disp;
						}
						
						if (param_disp == 0) //�¶Ȳ���
						{
							if (key_value_bak == 16)
								temp_param_bak -= 2;
							
							if (key_value_bak == 17)
								temp_param_bak += 2;
						}
						else //�������
						{
							if (key_value_bak == 16)
								dist_param_bak -= 5;
							
							if (key_value_bak == 17)
								dist_param_bak += 5;
						}
					}
				}
				else //����
				{
					if (key_value_bak == 12) //��������
					{
						param_change = 0;
						work_flag = 4;
					}
					else if (key_value_bak == 13) //DAC����ı�
					{
						dac_out_flag = !dac_out_flag;
					}
				}
				
				key_state_bak = key_value_bak = 0; //
			}
			
			/* output */
			if (screen == 0)
			{
				digs = dig_buf[data_disp];
			}
			else
			{
				digs = dig_buf[param_disp + 3];
			}
			
			/* �¶���ʾ */
			dig_buf[0][4] = temperature / 1000 % 10;
			dig_buf[0][5] = temperature / 100 % 10 + 32;
			dig_buf[0][6] = temperature / 10 % 10;
			dig_buf[0][7] = temperature % 10;
			
			/* ������ʾ */
			dig_buf[1][6] = distance / 100 % 10;
			dig_buf[1][7] = distance / 10 % 10;
			
			/* ������� */
			if (param_change >= 10000)
				dig_buf[2][3] = param_change / 10000 % 10;
			else
				dig_buf[2][3] = 16;
			if (param_change >= 1000)
				dig_buf[2][4] = param_change / 1000 % 10;
			else
				dig_buf[2][4] = 16;
			if (param_change >= 100)
				dig_buf[2][5] = param_change / 100 % 10;
			else
				dig_buf[2][5] = 16;
			if (param_change >= 10)
				dig_buf[2][6] = param_change / 10 % 10;
			else
				dig_buf[2][6] = 16;
			dig_buf[2][7] = param_change % 10;
			
			/* �¶Ȳ��� */
			dig_buf[3][6] = temp_param_bak / 10;
			dig_buf[3][7] = temp_param_bak % 10;
			
			/* ������� */
			dig_buf[4][6] = dist_param_bak / 10;
			dig_buf[4][7] = dist_param_bak % 10;
			
			/* led */
			if (temperature > (unsigned int)temp_param * 100)
				led_buf |= 0x01;
			else
				led_buf &= ~0x01;
			
			if ((distance / 10) > dist_param)
				led_buf |= 0x02;
			else
				led_buf &= ~0x02;
			
			if (dac_out_flag)
				led_buf |= 0x04;
			else
				led_buf &= ~0x04;
		}
	}
}
