/*	#   I2C����Ƭ��˵��
	1. 	���ļ������ṩ���������빩����ѡ����ɳ�����Ʋο���
	2. 	����ѡ�ֿ������б�д��ش�����Ըô���Ϊ������������ѡ��Ƭ�����͡������ٶȺ�����
		�жԵ�Ƭ��ʱ��Ƶ�ʵ�Ҫ�󣬽��д�����Ժ��޸ġ�
*/

#include "iic.h"

#define DELAY_TIME	5

sbit scl = P2^0;
sbit sda = P2^1;

//
static void I2C_Delay(unsigned char n)
{
    do
    {
        _nop_();_nop_();_nop_();_nop_();_nop_();
        _nop_();_nop_();_nop_();_nop_();_nop_();
        _nop_();_nop_();_nop_();_nop_();_nop_();		
    }
    while(n--);      	
}

//
void I2CStart(void)
{
    sda = 1;
    scl = 1;
	I2C_Delay(DELAY_TIME);
    sda = 0;
	I2C_Delay(DELAY_TIME);
    scl = 0;    
}

//
void I2CStop(void)
{
    sda = 0;
    scl = 1;
	I2C_Delay(DELAY_TIME);
    sda = 1;
	I2C_Delay(DELAY_TIME);
}

// �����ֽ�
void I2CSendByte(unsigned char byt)
{
    unsigned char i;
	
    for(i=0; i<8; i++){
        scl = 0;
		I2C_Delay(DELAY_TIME);
        if(byt & 0x80){
            sda = 1;
        }
        else{
            sda = 0;
        }
		I2C_Delay(DELAY_TIME);
        scl = 1;
        byt <<= 1;
		I2C_Delay(DELAY_TIME);
    }
	
    scl = 0;  
}

// �����ֽ�
unsigned char I2CReceiveByte(void)
{
	unsigned char da;
	unsigned char i;
	for(i=0;i<8;i++){   
		scl = 1;
		I2C_Delay(DELAY_TIME);
		da <<= 1;
		if(sda) 
			da |= 0x01;
		scl = 0;
		I2C_Delay(DELAY_TIME);
	}
	return da;    
}

// �ȴ���Ӧ
unsigned char I2CWaitAck(void)
{
	unsigned char ackbit;
	
    scl = 1;
	I2C_Delay(DELAY_TIME);
    ackbit = sda; 
    scl = 0;
	I2C_Delay(DELAY_TIME);
	
	return ackbit;
}

// ���ͻ�Ӧ
void I2CSendAck(unsigned char ackbit)
{
    scl = 0;
    sda = ackbit; 
	I2C_Delay(DELAY_TIME);
    scl = 1;
	I2C_Delay(DELAY_TIME);
    scl = 0; 
	sda = 1;
	I2C_Delay(DELAY_TIME);
}

/* ------------------at24c02----------------- */
void e2prom_write(uint8_t addr, uint8_t *Data, uint8_t len)
{
	I2CStart();
	I2CSendByte(0xA0);
	I2CWaitAck();
	I2CSendByte(addr);
	I2CWaitAck();
	while(len--)
	{
		I2CSendByte(*Data);
		I2CWaitAck();
		Data++;
	}
	I2CStop();
}

void e2prom_read(uint8_t addr, uint8_t *Data, uint8_t len)
{
	I2CStart();
	I2CSendByte(0xA0);
	I2CWaitAck();
	I2CSendByte(addr);
	I2CWaitAck();
	
	I2CStart();
	I2CSendByte(0xA1);
	I2CWaitAck();
	
	loop:
	*Data = I2CReceiveByte();
	if (--len)
	{
		I2CSendAck(0); //����Ӧ��
		Data++;
		goto loop;
	}
	I2CSendAck(1); //���ͷ�Ӧ��
	I2CStop();
}

/* ------------------pcf8591----------------- */
uint8_t control_byte = 0x04;
uint8_t dac_level = 0; //��ǰdac����ĵȼ������ǰ���޸����ֵ�ٵ����������
uint8_t adc_level[4] = { 0 };

void dac_out(void)
{
	control_byte |= 0x40; //����dac���
	
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(control_byte);
	I2CWaitAck();
	I2CSendByte(dac_level);
	I2CWaitAck();
	I2CStop();
}

void dac_stop(void)
{
	control_byte &= ~0x40; //�ر�dac���
	
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(control_byte);
	I2CWaitAck();
	I2CStop();
}

void adc_read(void)
{
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(control_byte);
	I2CWaitAck();
	
	I2CStart();
	I2CSendByte(0x91);
	I2CWaitAck();
	
	I2CReceiveByte(); //��һ�ζ�ȡ�����ݲ�Ҫʹ��
	I2CSendAck(0); //����Ӧ��
	adc_level[0] = I2CReceiveByte();
	I2CSendAck(0); //����Ӧ��
	adc_level[1] = I2CReceiveByte();
	I2CSendAck(0); //����Ӧ��
	adc_level[2] = I2CReceiveByte();
	I2CSendAck(0); //����Ӧ��
	adc_level[3] = I2CReceiveByte();
	I2CSendAck(1); //���ͷ�Ӧ��
	I2CStop();
}

unsigned char read_pcf8591(unsigned char channle)
{
	unsigned char buf;
	
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(channle | 0x40);
	I2CWaitAck();
	
	I2CStart();
	I2CSendByte(0x91);
	I2CWaitAck();
	I2CReceiveByte();
	I2CSendByte(0); //Ӧ��
	buf = I2CReceiveByte();
	I2CSendByte(1); //��Ӧ��
	I2CStop();
	
	return buf;
}

void write_pcf8591(unsigned char level)
{
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(0x40);
	I2CWaitAck();
	I2CSendByte(level);
	I2CWaitAck();
	I2CStop();
}

void stop_pcf8591(void)
{
	
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(0x00);
	I2CWaitAck();
	I2CStop();
}