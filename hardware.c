#include "hardware.h"

/* NE555Ƶ�ʼ����� */
void NE555Init(void)
{
	TMOD &= 0xF0;	//��ʱ��0ģʽ��λ
	TMOD |= 0x04;	//����Ϊ����ģʽ����������ΪP34
	TL0 = 0x00;		//��λ��ʼֵ
	TH0 = 0x00;		//��λ��ʼֵ
	TF0 = 0;			//���TF0��־
	TR0 = 1;			//��ʱ��0��ʼ��ʱ
}

/* ledpwm������ܡ��̵��������������Լ�����������ʱ��ʹ�������ʱ�� */
void Timer1Init(void)		//100΢��@12.000MHz
{
	AUXR |= 0x40;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0x50;		//���ö�ʱ��ʼֵ
	TH1 = 0xFB;		//���ö�ʱ��ʼֵ
	TF1 = 0;		//���TF1��־
	TR1 = 1;		//��ʱ��1��ʼ��ʱ
	ET1 = 1;
}
/*led*/
uint8_t xdata led_buf = 0x00;//ÿ��led��״̬
//uint8_t xdata led_state = 0; 
uint8_t xdata led_pwm_cnt = 0;	//led��pwm������
uint8_t xdata led_pwm_duty = 10; //led��pwmռ�ձȣ�Ĭ����ռ�ձȣ�ռ�ձȲ��ܳ�������
uint8_t xdata led_pwm_period = 10; //led��pwm���ڣ���λ���Ƕ�ʱ�������ڣ�������100us��

void led_output(uint8_t led_buf)
{
	P0 = ~led_buf;
	P2 |= 0x80;
	P2 &= 0x1f;
}

/*dig*/
/*************  ���س�������    **************/
uint8_t code t_display[]={                       //��׼�ֿ�
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
//	 0.   1.   2.   3.   4.   5.   6.   7.   8.   9.   -1
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};

uint8_t code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //λ��

uint8_t xdata dig_com = 0; //��ǰ��ʾ�������λ
uint8_t xdata dig_buf[SCREEN_NUM][8] = 
{
	{ 16, 16, 16, 16, 16, 16, 16, 16, },	//��������
	{ 25, 1, 16, 16, 16, 16, 16, 16, },	//����1
	{ 25, 2, 16, 16, 16, 16, 16, 16, },	//����2
	{ 25, 3, 16, 16, 16, 16, 16, 16, },	//����3
	{ 25, 4, 16, 16, 16, 16, 16, 16, },	//����4
	{ 25, 5, 16, 16, 16, 16, 16, 16, },	//����5
	{ 25, 6, 16, 16, 16, 16, 16, 16, },	//����6
};
uint8_t xdata *digs = dig_buf[0]; //��ǰ��ʾ����
uint8_t xdata dig_switch = 0xFF; //�����ÿһλ�Ŀ���
void dig_output()
{
	static unsigned char dig_com = 0;
	
	P0 = 0x00; //��Ӱ
	P2 |= 0xC0;
	P2 &= ~0xE0;
	
	P0 = ~t_display[digs[dig_com]]; //��ѡ
	P2 |= 0xE0;
	P2 &= ~0xE0;
	
	P0 = T_COM[dig_com]; //λѡ
	P2 |= 0xC0;
	P2 &= ~0xE0;
	
	if (++dig_com >= 8)
	{
		dig_com = 0;
	}
}

/*key*/
unsigned char key_value = 0; //������ֵ
unsigned char key_state = 0; //����״̬
/* ÿ���10ms����һ�� */
void key_scan_v2()
{
	P3 |= 0x0F;
	if (~P3 & 0x0F) // ����а�������
	{
		/* ����״̬���� */
		if (key_state < 255)
			key_state++;
		
		/* ������ֵ�ж� */
		if (P33 == 0)
			key_value = 4;
		if (P32 == 0)
			key_value = 5;
		if (P31 == 0)
			key_value = 6;
		if (P30 == 0)
			key_value = 7;
	}
	else //���û�а�������
	{
		key_value = 0;
		key_state = 0;
	}
}
//���󰴼�͵��������ʽ
unsigned char xdata ReadData=0;
unsigned char xdata trg=0;
unsigned char xdata cnt=0;
void KeyAction()  //���Գ�����    ����ʽ���� S4--S7
{
		static unsigned char cont_count = 0;	
		unsigned char xdata ReadData;
		ReadData = P3^0xff;
		trg = ReadData&(ReadData^cnt);//���δ����ж�
		cnt = ReadData;//�����ж�

		if(trg & 0x01)          	//����S7
		{
//			freg += 1;
		}
		else if(trg & 0x02)     	//����S6
		{
	//		freg -= 1;
		}		
		
		else if(trg & 0x04)     	//����S5
		{
	//		freg -= 1;
		}		
		
		else if(trg & 0x08)     	//����S4
		{
	//		freg -= 1;
		}		
		
		
		else if(cnt & 0x01)          //����S7  1S
		{
			if(++cont_count == 100)
			{
					cont_count = 0;
	//				freg += 10;
			}
		}
}
/* һ�㶼��10ms���ҵ���һ�Σ�
	 10msΪ����ʱ�䣬���Ը�����Ҫ���� */
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
		/* ����״̬���� */
		if (key_state < 255)
			key_state++;
		
		/* ������ֵ�ж� */
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

void key_pad_scan_v2()//ʹ�ô���ʱ����ɨ�贮��P30,P31||ʹ��Ne555ʱ����ɨ��P34
{
	unsigned char R = 255, C = 255;
	
//	P3 = 0x0F;
	P3 |= 0x08;
	P3 &= ~0xF0;
	P4 = 0x00;
	
	if (~P3 & 0x0F)
	{
		/* ����״̬���� */
		if (key_state < 255)
			key_state++;
		
		/* ������ֵ�ж� */
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
void key_pad_scan_v3()//���̰�
{
	unsigned char R = 255, C = 255;
	
//	P3 = 0x0F;
	P3 |= 0x08;
	P3 &= ~0xF0;
	P4 = 0x00;
	
	if (~P3 & 0x0F)
	{
		/* ����״̬���� */
		if (key_state < 255)
			key_state++;
		
		/* ������ֵ�ж� */
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
		if (key_value && key_state <= 250) //����������ɿ�
		{
			if (key_state < 100) //�����µ�����ʱ��û�г���1��
				key_state = 255;
			else if (key_state < 200) //�����µ�����ʱ�䳬��1���û����2��
				key_state = 254;
			else //�����µ�����ʱ�䳬��2��
				key_state = 253;
		}
		else //������ȫ�ɿ�
		{
			key_value = 0;
			key_state = 0;
		}
	}
}

/* ��������״̬ */
uint8_t xdata high_power_state = 0;

/* ���ⶨʱ�� */
uint8_t xdata Timer10ms_cnt = 0;
uint8_t xdata Timer50ms_cnt = 0;
uint8_t xdata Timer100ms_cnt = 0;
uint16_t xdata Timer500ms_cnt = 0;
uint16_t xdata Timer1s_cnt = 0;
uint16_t xdata Timern_cnt = 0;
uint16_t xdata operate_timer_cnt = 0; //�������ڶ�ʱ����������ÿ��ʱ�������ֹʱ����ң���Ҫ���ݲ�����оƬ������������ʱ������

/* NE555Ƶ�� */
uint16_t xdata ne555_out = 0;

void Timer1Isr(void) interrupt 3 //100usһ����
{
	static uint8_t Timer1ms_cnt = 10;
	static uint16_t ne555_cnt = 100; //ne555ר�õĶ�ʱ�������Ե�����������

	{ //LED_PWM
		if (led_pwm_cnt <= led_pwm_duty) //��Ч��ƽ
		{
			CHANNLE(4, ~led_buf);
			//led_output(led_buf);
		}
		else //��Ч��ƽ
		{
			CHANNLE(4, 0xFF); //�ر�LED
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
		
		{ //�����
			CHANNLE(6, 0x00);
			CHANNLE(7, ~t_display[digs[dig_com]]); //�ؼ�����
			CHANNLE(6, T_COM[dig_com] & dig_switch); //�ؼ����� ������һλ����ܿ����������Ƿ���ʾ
			if (++dig_com >= 8)
				dig_com = 0;
		}
		//dig_output();
		if (++ne555_cnt >= 100)
		{
			ne555_cnt = 0;
			
			TR0 = 0;		//��ʱ��0��ͣ����
			ne555_out = TH0 << 8 | TL0;
			TL0 = 0x00;	//��λ��ʼֵ
			TH0 = 0x00;	//��λ��ʼֵ
			TR0 = 1;		//��ʱ��0��ʼ����
//			ne555_out *= 10;
		}
		
		//��������
		CHANNLE(5, high_power_state);
	}
}


