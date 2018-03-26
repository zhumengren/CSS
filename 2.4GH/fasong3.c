                                                                                                         //+++++++++++++++++++++++++
//NRF2401模块测试
//20170712
//+++++++++++++++++++++++++
#include<reg52.h>
#include<intrins.h>
#define uchar unsigned char
#define uint unsigned int
sbit lcden = P3^4;
sbit lcdrs = P3^5;
sbit dula  = P2^6;
sbit wela  = P2^7;

sbit cs1=P3^6;
sbit cs2=P3^7;

uchar table[]="123456";
//++++++++++++++++++++++++++
//SPI所接管口
//++++++++++++++++++++++++++
sbit CE=P3^0;
sbit IRQ=P2^5;
sbit CSN=P2^1;
sbit MOSI=P3^1;
sbit MISO=P2^4;
sbit SCK=P2^2;
uchar str[4];//作为存储上面的uchartostr函数转换的字符

//+++++++++++++++++++++
//定义各个寄存器的地址，指令，状态标志，等等的定义
//+++++++++++++++++++++
//============ 状态标志 =============
extern uchar idata nrf_sta;
#define RX_DR 		((nrf_sta >> 6) & 0x01)
#define TX_DS  		((nrf_sta >> 5) & 0x01)
#define MAX_RT 	((nrf_sta >> 4) & 0x01)
//================== NRF24L01的接收和发送地址 ===================
#define TX_ADR_WIDTH    5   // 5个字节的TX地址长度
#define RX_ADR_WIDTH    5   // 5个字节的RX地址长度
#define TX_PLOAD_WIDTH  32  // ?个字节的TX数据长度
#define RX_PLOAD_WIDTH  32  // ?个字节的RX数据长度
//================== NRF24L01寄存器指令 =======================
#define READ_REG       	 	0x00  	// 读寄存器
#define WRITE_REG       		0x20 	// 写寄存器
#define RD_RX_PLOAD     	0x61  	// 读取接收数据
#define WR_TX_PLOAD     0xA0  	// 写待发数据
#define FLUSH_TX        		0xE1 	// 冲洗发送 FIFO
#define FLUSH_RX        		0xE2  	// 冲洗接收 FIFO
#define REUSE_TX_PL     	0xE3  	// 定义重复装载数据
#define NOP             			0xFF  	// 保留
//================== SPI(nRF24L01)寄存器地址 =====================
#define NRF_CONFIG      0x00  // 配置收发状态，CRC校验模式以及收发状态响应方式
#define EN_AA           		0x01  // 自动应答功能设置
#define EN_RXADDR       0x02  // 可用信道设置
#define SETUP_AW        	0x03  // 收发地址宽度设置
#define SETUP_RETR      0x04  // 自动重发功能设置
#define RF_CH           		0x05  // 工作频率设置
#define RF_SETUP        	0x06  // 发射速率、功耗功能设置
#define STATUS          	0x07  // 状态寄存器
#define OBSERVE_TX      0x08  // 发送监测功能
#define CD              			0x09  // 地址检测           
#define RX_ADDR_P0      0x0A  // 频道0接收数据地址
#define RX_ADDR_P1      0x0B  // 频道1接收数据地址 
#define RX_ADDR_P2      0x0C  // 频道2接收数据地址 
#define RX_ADDR_P3      0x0D  // 频道3接收数据地址
#define RX_ADDR_P4      0x0E  // 频道4接收数据地址
#define RX_ADDR_P5      0x0F  // 频道5接收数据地址
#define TX_ADDR         	0x10  // 发送地址寄存器
#define RX_PW_P0        	0x11  // 接收频道0接收数据长度
#define RX_PW_P1        	0x12  // 接收频道0接收数据长度
#define RX_PW_P2       	0x13  // 接收频道0接收数据长度
#define RX_PW_P3        	0x14  // 接收频道0接收数据长度
#define RX_PW_P4        	0x15  // 接收频道0接收数据长度
#define RX_PW_P5        	0x16  // 接收频道0接收数据长度
#define FIFO_STATUS     0x17  // FIFO栈入栈出状态寄存器设置
//==============================
//收发缓存数组 
//==============================
extern uchar idata RxBuf[32]={1,2,3,4,5,6,7,8,9,10,11} ;	//接收缓存 存入idata区
extern uchar idata TxBuf[32]={1,2,3,4,5,6,7,8,9,10,11};	//发送缓存

//+++++++++++++++++++++
//本地地址和接收地址
//+++++++++++++++++++++
char const TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01};//本地地址
char const RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01};//接收地址

//+++++++++++++++++++++
//延时程序，单位毫秒
//+++++++++++++++++++++
void delay_ms(uint z)
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=110;y>0;y--);
} 

//+++++++++++++++++++++++++++++
//将char值转成字符串函数 具体有什么用我也不知道
//+++++++++++++++++++++++++++++
uchar *uchartostr(uchar unm)
{
	uchar x00,xx,x0,x,n;
	x00=unm/100;
	xx=unm%100;
	x0=xx/10;
	x=xx%10;
	n=0;
	if(x00!=0)
	{
		str[n]=x00+48;
		n++;
	}
	if(!(x00==0&x0==0))
	{
	str[n]=x0+48;
	n++;
	}
	str[n]=x+48;
	n++;
	str[n]='\0';
	return str;
}

//+++++++++++++++++++++++++++++
//函数：SPI_RW(uchar num)
//往NRF2401写入数据num，并从中读回数据
//+++++++++++++++++++++++++++++
uchar SPI_RW(uchar num)
{
	uchar bit_ctr;
   	for(bit_ctr=0;bit_ctr<8;bit_ctr++)  
   	{
		MOSI = (num & 0x80);   
		delay_ms(1);         
		num = (num << 1);               
		SCK = 1;                        
		num |= MISO;  
    		        
		SCK = 0;            		    
   	}
    return(num);           		        
}
/*
uchar SPI_RW(uchar byte)
{
uchar i;
for(i=0;i<8;i++)//一字节8位循环8次写入
{
if(byte&0x80)//如果数据最高位是1
//delay_ms(1);
MOSI=1;//向NRF24L01写1
else //否则写0
MOSI=0;
delay_ms(1);
byte<<=1;//低一位移到最高位
SCK=1;//SCK拉高，写入一位数据，同时读取一位数据
if(MISO)
byte|=0x01;
SCK=0;//SCK拉低
}
return byte;//返回读取一字节
}*/



//+++++++++++++++++++++++++++++
//函数uchar SPI_Read_Reg(uchar reg)
//作用就是写入寄存器的地址reg，然后
//读取该地址的一个字节的数据返回给该函数
//+++++++++++++++++++++++++++++
uchar SPI_Read_Reg(uchar reg)
{
	uchar reg_val;

	CSN = 0;                
	SPI_RW(reg);
//	delay_ms(1);            
	reg_val = SPI_RW(0);  
//	delay_ms(1);
	CSN = 1;                	
	return (reg_val);        
}

//+++++++++++++++++++++++++++++
//函数SPI_Write_Reg(uchar reg,uchar value)
//写一个地址reg定位寄存器，同时返回该寄存器的值
//接着往该寄存器里面写入数据value
//+++++++++++++++++++++++++++++
uchar SPI_Write_Reg(uchar reg, uchar value)
{
	uchar status;
	CSN = 0;               
	status = SPI_RW(reg);      
	SPI_RW(value);            
	CSN = 1;                  
	return(status);            
}

//+++++++++++++++++++++++++++++
//读多字节寄存器的函数 
//功能用于读数据，reg:寄存器的地址 
//				  pBuF;待读出数据地址 
//				  uchars;读出数据的个数 
//就是从NRF24中读取数据到单片机中
//+++++++++++++++++++++++++++++
uchar SPI_Read_Buf(uchar reg, uchar *pBuf, uchar nBytes)
{
	uchar status;
	uchar i;
	CSN = 0; 
	status = SPI_RW(reg); //定位到寄存器reg并读取该寄存器的数据
	for(i = 0;i < nBytes;i++)
		pBuf[i] = SPI_RW(0); //通过MISO从NRF24读入数据
	CSN = 1; 
	return(status); 
}

//+++++++++++++++++++++++++++++
//写多字节寄存器的函数
//功能用于写数据，reg:寄存器的地址
//				  pBuF;待写进数据地址
//				  uchars;写进数据的个数 
//就是从单片机向NRF24中写入数据
//+++++++++++++++++++++++++++++
uchar SPI_Write_Buf(uchar reg, uchar *pBuf, uchar nBytes)
{
	uchar status;
	uchar i;
	CSN = 0;             
	status = SPI_RW(reg);   
	for(i = 0; i < nBytes;i++)
		SPI_RW(pBuf[i]);
	CSN = 1;           
	return(status);     
}

//+++++++++++++++++++++++++++++
//LED显示程序
//+++++++++++++++++++++++++++++
void write_com(uchar com)
{
lcdrs = 0;
lcden = 0;
P0 = com;
delay_ms(5);
lcden = 1;
delay_ms(5);
lcden = 0;
}

void write_date(uchar date)
{
lcdrs = 1;
lcden = 0;
P0 = date;
delay_ms(5);
lcden = 1;
delay_ms(5);
lcden =0;
}

void init_yejin()
{
dula = 0;
wela = 0;
write_com(0x38);
write_com(0x0c);
write_com(0x06);
write_com(0x01);
}

void shownews(char *a,char*b)
{
uint num ;
char *string,*string1;//设置指针
char i=0;
string=a;//指针指向status[0]
//char *string1;//分开定义不行，要把string和string1放在一起才行
string1=b;
init_yejin();
write_com(0x80+0x02);
for(num = 0,string=a;num<4;num++,string++)//写第二行的8个数据
	{
	write_date(*string);
	delay_ms(5);
	}
write_com(0x80+0x42);
		for(num=9,string1=b;num<15;num++,string1++)
		{
		write_date(*string1);
		delay_ms(100);
		}		
}


//++++++++++++++++++++++++++
//头函数简化程序，把所有的发射要准备的程 
//序都写到这里了 
//++++++++++++++++++++++++++
void init_NRF24L01(void)
{
//	delay_ms(1);
	CE = 0;//CE置0,射频电路停止工作
	CSN=1;//CSN置1，停止寄存器读写
	SCK=0;//SCK置0，时钟信号停止读写
	IRQ=1;//IRQ置1，中断复位，等待产生中断信号

	SPI_Write_Reg(WRITE_REG+EN_AA,0x00);//频道0到5自动ACK应答禁止
	SPI_Write_Reg(WRITE_REG+SETUP_RETR,0x00);//禁止自动重发
	SPI_Write_Reg(WRITE_REG+EN_RXADDR,0x01);//允许接收地址只有频道0，+++++其实我不大能理解0x01为什么是只有频道0
	SPI_Write_Reg(WRITE_REG+RF_CH,1);//设置信道工作频率，收发要一致
	SPI_Write_Reg(WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//设置接收数据长度，这里为32字节
	SPI_Write_Reg(WRITE_REG+RF_SETUP,0x0f);//发射速率为2MHZ，发射功率为最大值0db，+++++++这一步教程里似乎有错误（）,知道有错误还不检查，TM害我花了那么多时间

}										//这里特别反省自己，原资料是使用的0x07,使用的明明是1MHZ但是却说是2MHZ，也许，这个错误对别人的电路没有影响，但是对自己的影响却是成败的关键
										//而且自己明明检查出来了这个错误，但是没有引起重视，这里再说一点，就是机器的配置结构不同，会对时序影响很大，这个在以后的工作中一定要引起重视
/*

//++++++++++++++++++++++++++
//头函数简化程序，把所有的发射要准备的程 
//序都写到这里了 
//++++++++++++++++++++++++++
void init_NRF24L01(void)
{
	delay_ms(1);
	CE = 0;//CE置0,射频电路停止工作
	CSN=1;//CSN置1，停止寄存器读写
	SCK=0;//SCK置0，时钟信号停止读写
	SPI_Write_Buf(WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);//写本地地址
	SPI_Write_Buf(WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);//写接收端地址
	SPI_Write_Reg(WRITE_REG+EN_AA,0x00);//频道0到5自动ACK应答禁止
//	SPI_Write_Reg(WRITE_REG+SETUP_RETR,0x0a);//禁止自动重发
	SPI_Write_Reg(WRITE_REG+EN_RXADDR,0x01);//允许接收地址只有频道0，+++++其实我不大能理解0x01为什么是只有频道0
	SPI_Write_Reg(WRITE_REG+RF_CH,1);//设置信道工作频率，收发要一致
	SPI_Write_Reg(WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//设置接收数据长度，这里为32字节
	SPI_Write_Reg(WRITE_REG+RF_SETUP,0x0f);//发射速率为2MHZ，发射功率为最大值0db，+++++++这一步教程里似乎有错误（）,知道有错误还不检查，TM害我花了那么多时间
}										//这里特别反省自己，原资料是使用的0x07,使用的明明是1MHZ但是却说是2MHZ，也许，这个错误对别人的电路没有影响，但是对自己的影响却是成败的关键
										//而且自己明明检查出来了这个错误，但是没有引起重视，这里再说一点，就是机器的配置结构不同，会对时序影响很大，这个在以后的工作中一定要引起重视
*/

/*
//++++++++++++++++++++++++++
//发送tx_buf中的数据1
//++++++++++++++++++++++++++
void NRF24L01_TxPacket(unsigned char * tx_buf)
{	
//delay_ms(10);
	CE=0;//停止射频电路工作
	SPI_Write_Reg(WRITE_REG+STATUS,0xff);//清除中断标志
	SPI_Write_Reg(FLUSH_TX,0x00);	//清除TX_FIFO寄存器
	SPI_Write_Reg(WRITE_REG+NRF_CONFIG,0x0e);//IRQ收发完成中断响应，16位CEC,发送模式，可以试用7e
	CE=1;//置高CE，激发数据发送?????不确定是不是发送   
	delay_ms(120);//延时微秒，不知道够不够
//以上为发送模式
	CE=0;//停止射频电路工作
	SPI_Write_Reg(WRITE_REG+STATUS,0xff);//清除中断标志
	SPI_Write_Reg(FLUSH_TX,0x00);	//清除TX_FIFO寄存器
//	SPI_Write_Buf(WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);//写本地地址
//delay_ms(10);
	SPI_Write_Buf(WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);//写接收端地址	
//delay_ms(10);
	SPI_Write_Buf(WR_TX_PLOAD,tx_buf,TX_PLOAD_WIDTH);//装载要发送的数据，这里的数据
	CE=1;//置高CE，激发数据发送
	delay_ms(180);
}
*/ 

/*
//++++++++++++++++++++++++++
//发送tx_buf中的数据1
//++++++++++++++++++++++++++
void NRF24L01_TxPacket(unsigned char * tx_buf)
{	
//delay_ms(10);
	CE=0;//停止射频电路工作
	SPI_Write_Buf(WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);//写接收端地址
	SPI_Write_Buf(WR_TX_PLOAD,tx_buf,TX_PLOAD_WIDTH);//装载要发送的数据，这里的数据
	SPI_Write_Reg(WRITE_REG+NRF_CONFIG,0x7e);//IRQ收发完成中断响应，16位CEC,发送模式，可以试用7e
	CE=1;//置高CE，激发数据发送
	delay_ms(180);
}

*/


//++++++++++++++++++++++++++
//发送tx_buf中的数据
//++++++++++++++++++++++++++
void NRF24L01_TxPacket(unsigned char * tx_buf)
{	

	CE=0;//停止射频电路工作
	SPI_Write_Buf(WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);//写本地地址
	SPI_Write_Buf(WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);//写接收端地址
	SPI_Write_Buf(WR_TX_PLOAD,tx_buf,TX_PLOAD_WIDTH);//装载要发送的数据，这里的数据从形参来
//	SPI_Write_Reg(WRITE_REG+STATUS,0xff);//清除中断标志
//	SPI_Write_Reg(FLUSH_TX,0x00);	//清除TX_FIFO寄存器
	SPI_Write_Reg(WRITE_REG+NRF_CONFIG,0x0e);//IRQ收发完成中断响应，16位CEC,发送模式，可以试用7e
//	CE=1;//置高CE，激发数据发送?????不确定是不是发送   
//	delay_ms(120);//延时微秒，不知道够不够
//以上为发送模式
//	CE=0;//停止射频电路工作
//	SPI_Write_Reg(WRITE_REG+STATUS,0xff);//清除中断标志
//	SPI_Write_Reg(FLUSH_TX,0x00);	//清除TX_FIFO寄存器
	
//delay_ms(10);
		
//delay_ms(10);
	
	CE=1;//置高CE，激发数据发送
	delay_ms(120);
}


//++++++++++++++++++++++++++
//主函数
//++++++++++++++++++++++++++
void main()
{	
//	uchar z=0;//测试用	
//	uchar addr[5]={0,0,0,0,0};//定义5个数据用来装载读取到的本机地址


	uchar status;//用来装读取到的status的数值
    uchar fifo_s;//定义一个变量用来存储读取到的FIFO的状态
	delay_ms(100);



//	status=0;//status置0,用于保存后面读取的数据
	
/*	status=SPI_Read_Reg(0x07);
	delay_ms(100);
	shownews("fs3",uchartostr(status));//用以显示测试
	delay_ms(1000);//延时测试LED
*/
	init_NRF24L01();//NRF24L01初始化init_NRF24L01()
//	status=SPI_Read_Reg(0x07);//读取寄存器0x07的值
/*
//用来测试SPI_Write_Buf和SPI_Read_Buf功能
	uchar const TX_ADDRESS[5]={0x05,0x04,0x03,0x02,0x01};//要写入的地址
	uchar addr[5]={0,0,0,0,0};//定义5个数据用来装载读取到的本机地址
	SPI_Write_Buf(WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);//把五个地址写入到发送地址寄存器TX_ADDR中
*/
//	SPI_Write_Buf(WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);//把五个地址写入到发送地址寄存器TX_ADDR中
//	SPI_Read_Buf(TX_ADDR,addr,TX_ADR_WIDTH);//读取本机发送寄存器TX_ADDR(就是地址0x10)的5个值写到addr中


	while(1)
	{	
//	status=SPI_Read_Reg(0x07);//读取寄存器0x07的值

/***********
	SPI_Write_Reg(WRITE_REG+STATUS,0xFF);//清理状态寄存器STATUS
	SPI_Write_Buf(WR_TX_PLOAD,TxBuf,TX_PLOAD_WIDTH);//装载数据TxBuf（共32字节）,这里没有对TxBuf进行赋值，使用的是默认值
	status=SPI_Read_Reg(STATUS);//读取模块状态
	fifo_s=SPI_Read_Reg(FIFO_STATUS);//读取FIFO的状态	
	shownews("123456",uchartostr(fifo_s));//分别显示STATUS和FIFO_STATUS
//	shownews(uchartostr(fifo_s),uchartostr(fifo_s));
delay_ms(1000);
*/  //***************


/*
if(status==14)
{
shownews(uchartostr(fifo_s),uchartostr(status));
cs1=1;
cs2=0;
//delay_ms(1000);
//delay_ms(1000);
}
else
{
cs1=0;
cs2=0;
delay_ms(1000);
delay_ms(1000);
}
*/

//	delay_ms(1000);//延时，用这段时间来记录

	NRF24L01_TxPacket(TxBuf);//进行一次数据（TxBuf）的发送
//	delay_ms(1000);

/*********
	status=SPI_Read_Reg(STATUS);//重新读取模块状态
	fifo_s=SPI_Read_Reg(FIFO_STATUS);//重新读取FIFO的状态	
	shownews("123456",uchartostr(fifo_s));//分别显示STATUS和FIFO_STATUS
	delay_ms(1000);//延时，用这段时间来记录
*/ //************
/*
if(status==46)
{
shownews(uchartostr(fifo_s),uchartostr(status));
cs1=1;
cs2=0;
//delay_ms(1000);
//delay_ms(1000);
}
else
{
cs1=0;
cs2=0;
delay_ms(1000);
delay_ms(1000);
}
*/
//delay_ms(1000);

//	shownews(uchartostr(status),"123456");//显示status的值
//	delay_ms(1000);
//	shownews("123456",status);

/*	status=SPI_Read_Reg(0x07);
	delay_ms(100);
	shownews("fs3",uchartostr(addr[1]));//用以显示测试
	delay_ms(1000);//延时测试LED
*/
	}
	
}
