/*	# 	�����ߴ���Ƭ��˵��
	1. 	���ļ������ṩ���������빩����ѡ����ɳ�����Ʋο���
	2. 	����ѡ�ֿ������б�д��ش�����Ըô���Ϊ������������ѡ��Ƭ�����͡������ٶȺ�����
		�жԵ�Ƭ��ʱ��Ƶ�ʵ�Ҫ�󣬽��д�����Ժ��޸ġ�
*/

#include "onewire.h"

sbit DQ = P1^4;

//
void Delay_OneWire(unsigned int t)  
{
	unsigned char i;
	while(t--){
		for(i=0;i<6;i++);
	}
}

//
void Write_DS18B20(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		EA = 0; //------------���ж�
		DQ = 0;
		DQ = dat&0x01;
		Delay_OneWire(5);
		DQ = 1;
		dat >>= 1;
		EA = 1; //------------���ж�
	}
	Delay_OneWire(5);
}

//
unsigned char Read_DS18B20(void)
{
	unsigned char i;
	unsigned char dat;
  
	for(i=0;i<8;i++)
	{
		EA = 0; //------------���ж�
		DQ = 0;
		dat >>= 1;
		DQ = 1;
		Delay_OneWire(1); //������������ͼ������(��Щ���Ӳ��ü����)������
		if(DQ)
		{
			dat |= 0x80;
		}	    
		Delay_OneWire(5);
		EA = 1; //------------���ж�
	}
	return dat;
}

//
bit init_ds18b20(void)
{
  	bit initflag = 0;
  	
  	DQ = 1;
  	Delay_OneWire(12);
  	DQ = 0;
  	Delay_OneWire(80);
  	DQ = 1;
		EA = 0; //------------���ж�
  	Delay_OneWire(10); 
    initflag = DQ;    
		EA = 1; //------------���ж�
  	Delay_OneWire(5);
  
  	return initflag;
}

/* -------------------------------------------- */
uint16_t temperature = 0;
void read_temp(void)//�¶�Ҫ�ӳ�700ms���ң���Ȼ����������Ĭ��ֵ
{
	uint16_t temp_H, temp_L;
	
	init_ds18b20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0x44);
	
	init_ds18b20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0xBE);
	
	temp_L = Read_DS18B20();
	temp_H = Read_DS18B20();
	
	temperature = (float)(temp_H << 8 | temp_L) * 6.25; //�¶ȷŴ�100���洢��һ����˵Ϊ0.0625
}
