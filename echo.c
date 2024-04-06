#include "echo.h"

//echo ��ʱ��0�汾,�����и��Ķ�ʱ��
#ifdef echo_timer
void Delay14us()		//@12.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 39;
	while (--i);
}

int read_distance()//ֻ��ʱ10000us
{
	int distance = -1;
	unsigned int temp;
	
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0xEF;		//���ö�ʱ��ʼֵ
	TH0 = 0xD8;		//���ö�ʱ��ʼֵ
	TF0 = 0;		//���TF0��־
	
	P10 = 1; Delay14us(); P10 = 0; Delay14us(); 
	P10 = 1; Delay14us(); P10 = 0; Delay14us(); 
	P10 = 1; Delay14us(); P10 = 0; Delay14us(); 
	P10 = 1; Delay14us(); P10 = 0; Delay14us(); 
	P10 = 1; Delay14us(); P10 = 0; Delay14us(); 
	P10 = 1; Delay14us(); P10 = 0; Delay14us(); 
	P10 = 1; Delay14us(); P10 = 0; Delay14us(); 
	P10 = 1; Delay14us(); P10 = 0; Delay14us(); 
	
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	while((P11 == 1) && (TF0 == 0));
	TR0 = 0;
	
	if (TF0 == 0)
	{
		temp = TH0 << 8 | TL0;
		distance = (temp - 55535) * 0.17F;
	}
	
	return distance;
}

#endif

#ifdef echo_pca_1
sbit trig=P1^0;   //���巢�Ͷ�
sbit echo=P1^1;   //������ն�
unsigned int dis;      //��������
void Delay13us()		//@12.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 36;
	while (--i);
}

void Send_Wave()  //����һ��40Khz����
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		trig=1;
		Delay13us();
		trig=0;
		Delay13us();
	}
}

unsigned int Get_Csb()
{
	
	unsigned int dis;
	CMOD&=0x00;   //1mhz
//	CH=0x00;      //�����߰�λ��ʱ12T10001us
//	CL=0x00;      //�����Ͱ�λ
	CH=0x8D;      //�����߰�λ��ʱ12T10001us
	CL=0x1C;      //�����Ͱ�λ
	Send_Wave();  //����40Khz��
	CR=1;     //��ʼ��ʱ
	while((echo==1)&&(CF==0));//�ȴ����ն˽��յ������źŻ������
	CR=0; //�رռ�ʱ
	if(CF==0) //���δ����������Χ���������ݴ���
	{
		//dis=((CH << 8 | CL)-0x8D1C) * 0.017;//cm
		dis=(unsigned int)((CH << 8 | CL)-0x8d1c) * 0.017;//cm
	}
	else  //�������������Χ�������־λ��Ӳ����һ��������Ҫ�ڴ��������
	{
	    CF=0;  //�����־λ����
		dis=0;
	}
	return  dis;
}



#endif

//echo pca�汾
#ifdef echo_pca_2
/* ����㷢�ֲ������̺̣ܶ�����Ӧ�ÿ���
	 ���ص�ʱ��IRCƵ����û�е���12.000MHz��
	 �������������ҵĴ��� (>w<) */
void Delay13us()		//@12.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 36;
	while (--i);
}

void TimerPCAInit()
{
	CCON = 0;	//��ʼ��PCA���ƼĴ���
						//PCA��ʱ��ֹͣ
						//���CF��־
						//���ģ���жϱ�־
	CMOD = 0x01;		//����PCAʱ��Դ 1MHz
									//����PCA��ʱ������ж�
	CCAPM0 = 0x11;	//PCAģ��0Ϊ�½��ش���
									//�������岶����ģ��P11�½����ⲿ�ж�
}

/* ultrasonic_flag ������״̬��־
	 0�����н׶�
	 1�����ͽ׶�
	 2��ȷ�Ͻ��յ����صĳ�����
	 3�����ղ������������򳬳����� */
uint8_t ultrasonic_flag = 0;
uint16_t distance = 0;

/* P11����������RX��һ�������½��أ�
	 ��˵���Ѿ��յ����صĳ�������������ж� */
void TimerPCAIsr() interrupt 7
{
	CR = 0; //���Ƚ�����ʱ
	
	if (ultrasonic_flag == 1) //�����ڷ��ͽ׶�
	{
		if (CCF0) /* ���յ����صĳ����� */
			ultrasonic_flag = 2; //ȷ�Ͻ��յ����صĳ�����
		else if (CF) /* �������� */
			ultrasonic_flag = 3; //���ղ������������򳬳�����
		else
			ultrasonic_flag = 0; //�������
	}
//	else ���ﱾӦ�ü����else������bug������ʱ�Ҳ���
//		ultrasonic_flag = 0; //�������
	
	CF = 0;
	CCF0 = 0;
	CCF1 = 0; //��������־λ��Ȼû���õ�
	CCF2 = 0; //�����Է���һ�������жϿ�ס
}

/* ���������� */
void sand_ultrasonic()
{ //��ʱ��ǰ���Ƿ�����ǰ�����ԣ����ҵİ������ҷ��ּ�ʱ��ǰ�����ݱȽ�׼ȷ
	/* ������ʱ */
	CH = 0x8D;		//���ö�ʱ��ʼֵ�����ֵ����ô���ģ�0x8D1C �� 65535 - 500�����ף� / 0.017������/΢�룩��������̿����Լ���
	CL = 0x1C;		//���ö�ʱ��ʼֵ
	/* �������ԣ������ܲ���������4�ף�����������������������
		 ����ʱ������������Զ��4�ף��������ó�ֵ������������������
		 �����������5�ף����Դ������ٲ���Ҫ�Ĳ���ʱ�䣬
		 ���Գ�����������ʱ�������ֵ����ֵ */
	CF = 0;		//���CF��־
	
	/* �������������� */
	EA = 0; //�ر��жϣ���ֹ��Ϸ���
	CR = 1;	//��ʱ����ʼ��ʱ
	P10 = 1; Delay13us(); P10 = 0; Delay13us(); //�ߵ�ƽ��ǰ�͵�ƽ�ں�
	P10 = 1; Delay13us(); P10 = 0; Delay13us();
	P10 = 1; Delay13us(); P10 = 0; Delay13us();
	P10 = 1; Delay13us(); P10 = 0; Delay13us();
	P10 = 1; Delay13us(); P10 = 0; Delay13us();
	P10 = 1; Delay13us(); P10 = 0; Delay13us();
	P10 = 1; Delay13us(); P10 = 0; Delay13us();
	P10 = 1; Delay13us(); P10 = 0; //��ʱ������Ҫֱ��һ�㣨ֱָ�Ӹ���8�Σ�
	EA = 1; //���´��ж�
}

/* ������� */
void calculate_distance()
{
	if (ultrasonic_flag == 2) //ȷ�Ͻ��յ����صĳ�����
	{
//		distance = CCAP0H;
//		distance <<= 8;
//		distance |= CCAP0L;
//		distance -= 0x8D1C; //������ʱ����ֵ����ȥ��ֵԭ�������ó�ֵ�ĵط���ע��
//		distance = (float)distance * 0.017;
		distance = ((CCAP0H << 8 | CCAP0L) - 0x8D1C) * 0.017;
	}
	else //���ղ������������򳬳�����
	{
		distance = DISTANCE_OUTRANG;
	}
}

/* ��ȡ���뺯��
	 ��һ����Ҫ��ôд
	 ���Ը��������������������޸� */
void read_distance()
{ //�����������������д�Ĳ��ã�����Ӧ������򵥵�д����
	if (ultrasonic_flag > 1)
	{
		calculate_distance();
		ultrasonic_flag = 0; //���뷢�ͽ׶�
	}
	
	if (ultrasonic_flag == 0)
	{
		sand_ultrasonic();
		ultrasonic_flag = 1; //���뷢�ͽ׶�
	}
}

#endif

