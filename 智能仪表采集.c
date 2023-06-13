#include <reg52.h>

unsigned int _100p0 = 0; //最高位
unsigned int _10p0 = 0; //次高位
unsigned int _1p0 = 0; //最低位

unsigned char table[]={
	0xc0,//0
	0xf9,//1
	0xa4,//2
	0xb0,//3
	0x99,//4
	0x92,//5
	0x82,//6
	0xf8,//7
	0x80,//8
	0x90//9
}; //段码表

sbit point = P1^7; //小数点，显示十进制时显示最高位的小数点

sbit w0 = P2^3; //从左到右第二个数码管位选/最高位
sbit w1 = P2^4; //第三个数码管位选/次高位
sbit w2 = P2^5; //第四个数码管位选/最低位

sbit ADDC = P2^2;
sbit ADDB = P2^1;
sbit ADDA = P2^0; //通道地址

sbit P_ = P2^7; //总控位，为1则其他控制信号失效，为0则有效
sbit ST = P3^6; //开始转换和锁存通道信号
sbit OE = P3^7; //输出使能
sbit EOC = P3^0; //转换完成信号

sbit S0 = P3^1; //按钮0，0通道
sbit S1 = P3^2; //按钮1，1通道
sbit S2 = P3^3; //2通道
sbit S3 = P3^4; //3通道

void delay(unsigned int time){ //软件延时函数
	unsigned int j = 0;
	for(; time > 0; time --){
		for(j = 0; j < 125; j++);
	}
}

void display(){ //数码管显示函数
	unsigned char i = 0;
	for(i = 0;i < 50;i ++){
		if(_100p0 == 0){ //如果最高位为0，熄灭最高位，也可以通过修改段码表来实现
			w0 = 0;
		}
		else{
			P1 = table[_100p0];
			//point = 0; //小数点
			w0 = 1; //打开数码管1位选
			w1 = 0;
			w2 = 0;
			delay(20);
			w0 = 0; //关闭数码管1位选
		}
		if(_100p0 == 0 && _10p0 == 0){ //如果最高位和次高位都为0，都熄灭
			w1 = 0;
		}
		else{
			P1 = table[_10p0];
			w0 = 0;
			w1 = 1;
			w2 = 0;
			delay(20);
			w1 = 0;
		}
		
		P1 = table[_1p0];
		w0 = 0;
		w1 = 0;
		w2 = 1;
		delay(20);
		w2 = 0;
	}
}

void SelectChannel(){ //通道选择
	if(S0 == 0){ //0通道
		ADDC = 0; ADDB = 0; ADDA = 0;
	}
	if(S1 == 0){ //1通道
		ADDC = 0; ADDB = 0; ADDA = 1;
	}
	if(S2 == 0){ //2通道
		ADDC = 0; ADDB = 1; ADDA = 0;
	}
	if(S3 == 0){ //3通道
		ADDC = 0; ADDB = 1; ADDA = 1;
	}
}

void StartADC(){ //A/D转换
	unsigned int temp = 0;
	P_ = 1; //关闭总控位
	SelectChannel(); //选择通道
	P_ = 0; //打开总控位
	ST = 0;
	ST = 1; //脉冲启动转换，脉宽是100ns，单片机执行指令是微妙级
	while(EOC == 0); //等待转换完成
	OE = 0; //打开输出使能
	temp = P0; //转换结果为二进制数字量
	//temp = P0 * 1.0/ 255 * 500; //转换结果转为十进制
	_100p0 = temp / 100;
	_10p0 = temp / 10 % 10;
	_1p0 = temp % 10;
	P_ = 1; //关闭总控位
	OE = 1; //关闭输出使能
}

void main(){
	ADDC = 0; ADDB = 0; ADDA = 0; //默认转换0通道
	while(1){
		StartADC();
		display();
	}
}