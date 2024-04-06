#include "echo.h"

//echo 定时器0版本,可自行更改定时器
#ifdef echo_timer
void Delay14us()		//@12.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 39;
	while (--i);
}

int read_distance()//只计时10000us
{
	int distance = -1;
	unsigned int temp;
	
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xEF;		//设置定时初始值
	TH0 = 0xD8;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
	
	P10 = 1; Delay14us(); P10 = 0; Delay14us(); 
	P10 = 1; Delay14us(); P10 = 0; Delay14us(); 
	P10 = 1; Delay14us(); P10 = 0; Delay14us(); 
	P10 = 1; Delay14us(); P10 = 0; Delay14us(); 
	P10 = 1; Delay14us(); P10 = 0; Delay14us(); 
	P10 = 1; Delay14us(); P10 = 0; Delay14us(); 
	P10 = 1; Delay14us(); P10 = 0; Delay14us(); 
	P10 = 1; Delay14us(); P10 = 0; Delay14us(); 
	
	TR0 = 1;		//定时器0开始计时
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
sbit trig=P1^0;   //定义发送端
sbit echo=P1^1;   //定义接收端
unsigned int dis;      //距离数据
void Delay13us()		//@12.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 36;
	while (--i);
}

void Send_Wave()  //产生一个40Khz方波
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
//	CH=0x00;      //计数高八位定时12T10001us
//	CL=0x00;      //计数低八位
	CH=0x8D;      //计数高八位定时12T10001us
	CL=0x1C;      //计数低八位
	Send_Wave();  //发送40Khz波
	CR=1;     //开始计时
	while((echo==1)&&(CF==0));//等待接收端接收到返回信号或者溢出
	CR=0; //关闭计时
	if(CF==0) //如果未超出测量范围，进行数据处理
	{
		//dis=((CH << 8 | CL)-0x8D1C) * 0.017;//cm
		dis=(unsigned int)((CH << 8 | CL)-0x8d1c) * 0.017;//cm
	}
	else  //如果超出测量范围，溢出标志位会硬件置一，我们需要在此软件清零
	{
	    CF=0;  //溢出标志位清零
		dis=0;
	}
	return  dis;
}



#endif

//echo pca版本
#ifdef echo_pca_2
/* 如果你发现测距的量程很短，那你应该看看
	 下载的时候IRC频率有没有调成12.000MHz，
	 而不是来质疑我的代码 (>w<) */
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
	CCON = 0;	//初始化PCA控制寄存器
						//PCA定时器停止
						//清除CF标志
						//清除模块中断标志
	CMOD = 0x01;		//设置PCA时钟源 1MHz
									//开启PCA定时器溢出中断
	CCAPM0 = 0x11;	//PCA模块0为下降沿触发
									//利用脉冲捕获来模拟P11下降沿外部中断
}

/* ultrasonic_flag 超声波状态标志
	 0：空闲阶段
	 1：发送阶段
	 2：确认接收到返回的超声波
	 3：接收不到超声波，或超出量程 */
uint8_t ultrasonic_flag = 0;
uint16_t distance = 0;

/* P11，即超声波RX，一旦出现下降沿，
	 则说明已经收到返回的超声波，进入此中断 */
void TimerPCAIsr() interrupt 7
{
	CR = 0; //优先结束计时
	
	if (ultrasonic_flag == 1) //当处于发送阶段
	{
		if (CCF0) /* 接收到返回的超声波 */
			ultrasonic_flag = 2; //确认接收到返回的超声波
		else if (CF) /* 超出量程 */
			ultrasonic_flag = 3; //接收不到超声波，或超出量程
		else
			ultrasonic_flag = 0; //意外情况
	}
//	else 这里本应该加这个else，但有bug，我暂时找不到
//		ultrasonic_flag = 0; //意外情况
	
	CF = 0;
	CCF0 = 0;
	CCF1 = 0; //这两个标志位虽然没有用到
	CCF2 = 0; //但是以防万一，导致中断卡住
}

/* 超声波发送 */
void sand_ultrasonic()
{ //计时在前还是发送在前都可以，在我的板子上我发现计时在前的数据比较准确
	/* 启动计时 */
	CH = 0x8D;		//设置定时初始值，这个值是这么来的：0x8D1C ≈ 65535 - 500（厘米） / 0.017（厘米/微秒），想改量程可以自己算
	CL = 0x1C;		//设置定时初始值
	/* 经过测试，板子能测的最长距离是4米（至少我这块板子是这样），
		 而定时器的理论量程远超4米，所以设置初值的意义在于缩短量程
		 （大概缩减到5米），以此来减少不必要的测量时间，
		 所以超声波传播的时间就是终值减初值 */
	CF = 0;		//清除CF标志
	
	/* 启动超声波发送 */
	EA = 0; //关闭中断，防止打断发送
	CR = 1;	//定时器开始计时
	P10 = 1; Delay13us(); P10 = 0; Delay13us(); //高电平在前低电平在后
	P10 = 1; Delay13us(); P10 = 0; Delay13us();
	P10 = 1; Delay13us(); P10 = 0; Delay13us();
	P10 = 1; Delay13us(); P10 = 0; Delay13us();
	P10 = 1; Delay13us(); P10 = 0; Delay13us();
	P10 = 1; Delay13us(); P10 = 0; Delay13us();
	P10 = 1; Delay13us(); P10 = 0; Delay13us();
	P10 = 1; Delay13us(); P10 = 0; //有时候，做人要直接一点（指直接复制8次）
	EA = 1; //重新打开中断
}

/* 计算距离 */
void calculate_distance()
{
	if (ultrasonic_flag == 2) //确认接收到返回的超声波
	{
//		distance = CCAP0H;
//		distance <<= 8;
//		distance |= CCAP0L;
//		distance -= 0x8D1C; //减掉定时器初值，减去初值原因在设置初值的地方有注解
//		distance = (float)distance * 0.017;
		distance = ((CCAP0H << 8 | CCAP0L) - 0x8D1C) * 0.017;
	}
	else //接收不到超声波，或超出量程
	{
		distance = DISTANCE_OUTRANG;
	}
}

/* 读取距离函数
	 不一定需要这么写
	 可以更据上面两个函数自行修改 */
void read_distance()
{ //可能你会觉得这个函数写的不好，但这应该是最简单的写法了
	if (ultrasonic_flag > 1)
	{
		calculate_distance();
		ultrasonic_flag = 0; //进入发送阶段
	}
	
	if (ultrasonic_flag == 0)
	{
		sand_ultrasonic();
		ultrasonic_flag = 1; //进入发送阶段
	}
}

#endif

