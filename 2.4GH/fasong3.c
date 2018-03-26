                                                                                                         //+++++++++++++++++++++++++
//NRF2401ģ�����
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
//SPI���ӹܿ�
//++++++++++++++++++++++++++
sbit CE=P3^0;
sbit IRQ=P2^5;
sbit CSN=P2^1;
sbit MOSI=P3^1;
sbit MISO=P2^4;
sbit SCK=P2^2;
uchar str[4];//��Ϊ�洢�����uchartostr����ת�����ַ�

//+++++++++++++++++++++
//��������Ĵ����ĵ�ַ��ָ�״̬��־���ȵȵĶ���
//+++++++++++++++++++++
//============ ״̬��־ =============
extern uchar idata nrf_sta;
#define RX_DR 		((nrf_sta >> 6) & 0x01)
#define TX_DS  		((nrf_sta >> 5) & 0x01)
#define MAX_RT 	((nrf_sta >> 4) & 0x01)
//================== NRF24L01�Ľ��պͷ��͵�ַ ===================
#define TX_ADR_WIDTH    5   // 5���ֽڵ�TX��ַ����
#define RX_ADR_WIDTH    5   // 5���ֽڵ�RX��ַ����
#define TX_PLOAD_WIDTH  32  // ?���ֽڵ�TX���ݳ���
#define RX_PLOAD_WIDTH  32  // ?���ֽڵ�RX���ݳ���
//================== NRF24L01�Ĵ���ָ�� =======================
#define READ_REG       	 	0x00  	// ���Ĵ���
#define WRITE_REG       		0x20 	// д�Ĵ���
#define RD_RX_PLOAD     	0x61  	// ��ȡ��������
#define WR_TX_PLOAD     0xA0  	// д��������
#define FLUSH_TX        		0xE1 	// ��ϴ���� FIFO
#define FLUSH_RX        		0xE2  	// ��ϴ���� FIFO
#define REUSE_TX_PL     	0xE3  	// �����ظ�װ������
#define NOP             			0xFF  	// ����
//================== SPI(nRF24L01)�Ĵ�����ַ =====================
#define NRF_CONFIG      0x00  // �����շ�״̬��CRCУ��ģʽ�Լ��շ�״̬��Ӧ��ʽ
#define EN_AA           		0x01  // �Զ�Ӧ��������
#define EN_RXADDR       0x02  // �����ŵ�����
#define SETUP_AW        	0x03  // �շ���ַ�������
#define SETUP_RETR      0x04  // �Զ��ط���������
#define RF_CH           		0x05  // ����Ƶ������
#define RF_SETUP        	0x06  // �������ʡ����Ĺ�������
#define STATUS          	0x07  // ״̬�Ĵ���
#define OBSERVE_TX      0x08  // ���ͼ�⹦��
#define CD              			0x09  // ��ַ���           
#define RX_ADDR_P0      0x0A  // Ƶ��0�������ݵ�ַ
#define RX_ADDR_P1      0x0B  // Ƶ��1�������ݵ�ַ 
#define RX_ADDR_P2      0x0C  // Ƶ��2�������ݵ�ַ 
#define RX_ADDR_P3      0x0D  // Ƶ��3�������ݵ�ַ
#define RX_ADDR_P4      0x0E  // Ƶ��4�������ݵ�ַ
#define RX_ADDR_P5      0x0F  // Ƶ��5�������ݵ�ַ
#define TX_ADDR         	0x10  // ���͵�ַ�Ĵ���
#define RX_PW_P0        	0x11  // ����Ƶ��0�������ݳ���
#define RX_PW_P1        	0x12  // ����Ƶ��0�������ݳ���
#define RX_PW_P2       	0x13  // ����Ƶ��0�������ݳ���
#define RX_PW_P3        	0x14  // ����Ƶ��0�������ݳ���
#define RX_PW_P4        	0x15  // ����Ƶ��0�������ݳ���
#define RX_PW_P5        	0x16  // ����Ƶ��0�������ݳ���
#define FIFO_STATUS     0x17  // FIFOջ��ջ��״̬�Ĵ�������
//==============================
//�շ��������� 
//==============================
extern uchar idata RxBuf[32]={1,2,3,4,5,6,7,8,9,10,11} ;	//���ջ��� ����idata��
extern uchar idata TxBuf[32]={1,2,3,4,5,6,7,8,9,10,11};	//���ͻ���

//+++++++++++++++++++++
//���ص�ַ�ͽ��յ�ַ
//+++++++++++++++++++++
char const TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01};//���ص�ַ
char const RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01};//���յ�ַ

//+++++++++++++++++++++
//��ʱ���򣬵�λ����
//+++++++++++++++++++++
void delay_ms(uint z)
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=110;y>0;y--);
} 

//+++++++++++++++++++++++++++++
//��charֵת���ַ������� ������ʲô����Ҳ��֪��
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
//������SPI_RW(uchar num)
//��NRF2401д������num�������ж�������
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
for(i=0;i<8;i++)//һ�ֽ�8λѭ��8��д��
{
if(byte&0x80)//����������λ��1
//delay_ms(1);
MOSI=1;//��NRF24L01д1
else //����д0
MOSI=0;
delay_ms(1);
byte<<=1;//��һλ�Ƶ����λ
SCK=1;//SCK���ߣ�д��һλ���ݣ�ͬʱ��ȡһλ����
if(MISO)
byte|=0x01;
SCK=0;//SCK����
}
return byte;//���ض�ȡһ�ֽ�
}*/



//+++++++++++++++++++++++++++++
//����uchar SPI_Read_Reg(uchar reg)
//���þ���д��Ĵ����ĵ�ַreg��Ȼ��
//��ȡ�õ�ַ��һ���ֽڵ����ݷ��ظ��ú���
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
//����SPI_Write_Reg(uchar reg,uchar value)
//дһ����ַreg��λ�Ĵ�����ͬʱ���ظüĴ�����ֵ
//�������üĴ�������д������value
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
//�����ֽڼĴ����ĺ��� 
//�������ڶ����ݣ�reg:�Ĵ����ĵ�ַ 
//				  pBuF;���������ݵ�ַ 
//				  uchars;�������ݵĸ��� 
//���Ǵ�NRF24�ж�ȡ���ݵ���Ƭ����
//+++++++++++++++++++++++++++++
uchar SPI_Read_Buf(uchar reg, uchar *pBuf, uchar nBytes)
{
	uchar status;
	uchar i;
	CSN = 0; 
	status = SPI_RW(reg); //��λ���Ĵ���reg����ȡ�üĴ���������
	for(i = 0;i < nBytes;i++)
		pBuf[i] = SPI_RW(0); //ͨ��MISO��NRF24��������
	CSN = 1; 
	return(status); 
}

//+++++++++++++++++++++++++++++
//д���ֽڼĴ����ĺ���
//��������д���ݣ�reg:�Ĵ����ĵ�ַ
//				  pBuF;��д�����ݵ�ַ
//				  uchars;д�����ݵĸ��� 
//���Ǵӵ�Ƭ����NRF24��д������
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
//LED��ʾ����
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
char *string,*string1;//����ָ��
char i=0;
string=a;//ָ��ָ��status[0]
//char *string1;//�ֿ����岻�У�Ҫ��string��string1����һ�����
string1=b;
init_yejin();
write_com(0x80+0x02);
for(num = 0,string=a;num<4;num++,string++)//д�ڶ��е�8������
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
//ͷ�����򻯳��򣬰����еķ���Ҫ׼���ĳ� 
//��д�������� 
//++++++++++++++++++++++++++
void init_NRF24L01(void)
{
//	delay_ms(1);
	CE = 0;//CE��0,��Ƶ��·ֹͣ����
	CSN=1;//CSN��1��ֹͣ�Ĵ�����д
	SCK=0;//SCK��0��ʱ���ź�ֹͣ��д
	IRQ=1;//IRQ��1���жϸ�λ���ȴ������ж��ź�

	SPI_Write_Reg(WRITE_REG+EN_AA,0x00);//Ƶ��0��5�Զ�ACKӦ���ֹ
	SPI_Write_Reg(WRITE_REG+SETUP_RETR,0x00);//��ֹ�Զ��ط�
	SPI_Write_Reg(WRITE_REG+EN_RXADDR,0x01);//������յ�ַֻ��Ƶ��0��+++++��ʵ�Ҳ��������0x01Ϊʲô��ֻ��Ƶ��0
	SPI_Write_Reg(WRITE_REG+RF_CH,1);//�����ŵ�����Ƶ�ʣ��շ�Ҫһ��
	SPI_Write_Reg(WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//���ý������ݳ��ȣ�����Ϊ32�ֽ�
	SPI_Write_Reg(WRITE_REG+RF_SETUP,0x0f);//��������Ϊ2MHZ�����书��Ϊ���ֵ0db��+++++++��һ���̳����ƺ��д��󣨣�,֪���д��󻹲���飬TM���һ�����ô��ʱ��

}										//�����ر�ʡ�Լ���ԭ������ʹ�õ�0x07,ʹ�õ�������1MHZ����ȴ˵��2MHZ��Ҳ���������Ա��˵ĵ�·û��Ӱ�죬���Ƕ��Լ���Ӱ��ȴ�ǳɰܵĹؼ�
										//�����Լ�������������������󣬵���û���������ӣ�������˵һ�㣬���ǻ��������ýṹ��ͬ�����ʱ��Ӱ��ܴ�������Ժ�Ĺ�����һ��Ҫ��������
/*

//++++++++++++++++++++++++++
//ͷ�����򻯳��򣬰����еķ���Ҫ׼���ĳ� 
//��д�������� 
//++++++++++++++++++++++++++
void init_NRF24L01(void)
{
	delay_ms(1);
	CE = 0;//CE��0,��Ƶ��·ֹͣ����
	CSN=1;//CSN��1��ֹͣ�Ĵ�����д
	SCK=0;//SCK��0��ʱ���ź�ֹͣ��д
	SPI_Write_Buf(WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);//д���ص�ַ
	SPI_Write_Buf(WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);//д���ն˵�ַ
	SPI_Write_Reg(WRITE_REG+EN_AA,0x00);//Ƶ��0��5�Զ�ACKӦ���ֹ
//	SPI_Write_Reg(WRITE_REG+SETUP_RETR,0x0a);//��ֹ�Զ��ط�
	SPI_Write_Reg(WRITE_REG+EN_RXADDR,0x01);//������յ�ַֻ��Ƶ��0��+++++��ʵ�Ҳ��������0x01Ϊʲô��ֻ��Ƶ��0
	SPI_Write_Reg(WRITE_REG+RF_CH,1);//�����ŵ�����Ƶ�ʣ��շ�Ҫһ��
	SPI_Write_Reg(WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//���ý������ݳ��ȣ�����Ϊ32�ֽ�
	SPI_Write_Reg(WRITE_REG+RF_SETUP,0x0f);//��������Ϊ2MHZ�����书��Ϊ���ֵ0db��+++++++��һ���̳����ƺ��д��󣨣�,֪���д��󻹲���飬TM���һ�����ô��ʱ��
}										//�����ر�ʡ�Լ���ԭ������ʹ�õ�0x07,ʹ�õ�������1MHZ����ȴ˵��2MHZ��Ҳ���������Ա��˵ĵ�·û��Ӱ�죬���Ƕ��Լ���Ӱ��ȴ�ǳɰܵĹؼ�
										//�����Լ�������������������󣬵���û���������ӣ�������˵һ�㣬���ǻ��������ýṹ��ͬ�����ʱ��Ӱ��ܴ�������Ժ�Ĺ�����һ��Ҫ��������
*/

/*
//++++++++++++++++++++++++++
//����tx_buf�е�����1
//++++++++++++++++++++++++++
void NRF24L01_TxPacket(unsigned char * tx_buf)
{	
//delay_ms(10);
	CE=0;//ֹͣ��Ƶ��·����
	SPI_Write_Reg(WRITE_REG+STATUS,0xff);//����жϱ�־
	SPI_Write_Reg(FLUSH_TX,0x00);	//���TX_FIFO�Ĵ���
	SPI_Write_Reg(WRITE_REG+NRF_CONFIG,0x0e);//IRQ�շ�����ж���Ӧ��16λCEC,����ģʽ����������7e
	CE=1;//�ø�CE���������ݷ���?????��ȷ���ǲ��Ƿ���   
	delay_ms(120);//��ʱ΢�룬��֪��������
//����Ϊ����ģʽ
	CE=0;//ֹͣ��Ƶ��·����
	SPI_Write_Reg(WRITE_REG+STATUS,0xff);//����жϱ�־
	SPI_Write_Reg(FLUSH_TX,0x00);	//���TX_FIFO�Ĵ���
//	SPI_Write_Buf(WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);//д���ص�ַ
//delay_ms(10);
	SPI_Write_Buf(WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);//д���ն˵�ַ	
//delay_ms(10);
	SPI_Write_Buf(WR_TX_PLOAD,tx_buf,TX_PLOAD_WIDTH);//װ��Ҫ���͵����ݣ����������
	CE=1;//�ø�CE���������ݷ���
	delay_ms(180);
}
*/ 

/*
//++++++++++++++++++++++++++
//����tx_buf�е�����1
//++++++++++++++++++++++++++
void NRF24L01_TxPacket(unsigned char * tx_buf)
{	
//delay_ms(10);
	CE=0;//ֹͣ��Ƶ��·����
	SPI_Write_Buf(WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);//д���ն˵�ַ
	SPI_Write_Buf(WR_TX_PLOAD,tx_buf,TX_PLOAD_WIDTH);//װ��Ҫ���͵����ݣ����������
	SPI_Write_Reg(WRITE_REG+NRF_CONFIG,0x7e);//IRQ�շ�����ж���Ӧ��16λCEC,����ģʽ����������7e
	CE=1;//�ø�CE���������ݷ���
	delay_ms(180);
}

*/


//++++++++++++++++++++++++++
//����tx_buf�е�����
//++++++++++++++++++++++++++
void NRF24L01_TxPacket(unsigned char * tx_buf)
{	

	CE=0;//ֹͣ��Ƶ��·����
	SPI_Write_Buf(WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);//д���ص�ַ
	SPI_Write_Buf(WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);//д���ն˵�ַ
	SPI_Write_Buf(WR_TX_PLOAD,tx_buf,TX_PLOAD_WIDTH);//װ��Ҫ���͵����ݣ���������ݴ��β���
//	SPI_Write_Reg(WRITE_REG+STATUS,0xff);//����жϱ�־
//	SPI_Write_Reg(FLUSH_TX,0x00);	//���TX_FIFO�Ĵ���
	SPI_Write_Reg(WRITE_REG+NRF_CONFIG,0x0e);//IRQ�շ�����ж���Ӧ��16λCEC,����ģʽ����������7e
//	CE=1;//�ø�CE���������ݷ���?????��ȷ���ǲ��Ƿ���   
//	delay_ms(120);//��ʱ΢�룬��֪��������
//����Ϊ����ģʽ
//	CE=0;//ֹͣ��Ƶ��·����
//	SPI_Write_Reg(WRITE_REG+STATUS,0xff);//����жϱ�־
//	SPI_Write_Reg(FLUSH_TX,0x00);	//���TX_FIFO�Ĵ���
	
//delay_ms(10);
		
//delay_ms(10);
	
	CE=1;//�ø�CE���������ݷ���
	delay_ms(120);
}


//++++++++++++++++++++++++++
//������
//++++++++++++++++++++++++++
void main()
{	
//	uchar z=0;//������	
//	uchar addr[5]={0,0,0,0,0};//����5����������װ�ض�ȡ���ı�����ַ


	uchar status;//����װ��ȡ����status����ֵ
    uchar fifo_s;//����һ�����������洢��ȡ����FIFO��״̬
	delay_ms(100);



//	status=0;//status��0,���ڱ�������ȡ������
	
/*	status=SPI_Read_Reg(0x07);
	delay_ms(100);
	shownews("fs3",uchartostr(status));//������ʾ����
	delay_ms(1000);//��ʱ����LED
*/
	init_NRF24L01();//NRF24L01��ʼ��init_NRF24L01()
//	status=SPI_Read_Reg(0x07);//��ȡ�Ĵ���0x07��ֵ
/*
//��������SPI_Write_Buf��SPI_Read_Buf����
	uchar const TX_ADDRESS[5]={0x05,0x04,0x03,0x02,0x01};//Ҫд��ĵ�ַ
	uchar addr[5]={0,0,0,0,0};//����5����������װ�ض�ȡ���ı�����ַ
	SPI_Write_Buf(WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);//�������ַд�뵽���͵�ַ�Ĵ���TX_ADDR��
*/
//	SPI_Write_Buf(WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);//�������ַд�뵽���͵�ַ�Ĵ���TX_ADDR��
//	SPI_Read_Buf(TX_ADDR,addr,TX_ADR_WIDTH);//��ȡ�������ͼĴ���TX_ADDR(���ǵ�ַ0x10)��5��ֵд��addr��


	while(1)
	{	
//	status=SPI_Read_Reg(0x07);//��ȡ�Ĵ���0x07��ֵ

/***********
	SPI_Write_Reg(WRITE_REG+STATUS,0xFF);//����״̬�Ĵ���STATUS
	SPI_Write_Buf(WR_TX_PLOAD,TxBuf,TX_PLOAD_WIDTH);//װ������TxBuf����32�ֽڣ�,����û�ж�TxBuf���и�ֵ��ʹ�õ���Ĭ��ֵ
	status=SPI_Read_Reg(STATUS);//��ȡģ��״̬
	fifo_s=SPI_Read_Reg(FIFO_STATUS);//��ȡFIFO��״̬	
	shownews("123456",uchartostr(fifo_s));//�ֱ���ʾSTATUS��FIFO_STATUS
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

//	delay_ms(1000);//��ʱ�������ʱ������¼

	NRF24L01_TxPacket(TxBuf);//����һ�����ݣ�TxBuf���ķ���
//	delay_ms(1000);

/*********
	status=SPI_Read_Reg(STATUS);//���¶�ȡģ��״̬
	fifo_s=SPI_Read_Reg(FIFO_STATUS);//���¶�ȡFIFO��״̬	
	shownews("123456",uchartostr(fifo_s));//�ֱ���ʾSTATUS��FIFO_STATUS
	delay_ms(1000);//��ʱ�������ʱ������¼
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

//	shownews(uchartostr(status),"123456");//��ʾstatus��ֵ
//	delay_ms(1000);
//	shownews("123456",status);

/*	status=SPI_Read_Reg(0x07);
	delay_ms(100);
	shownews("fs3",uchartostr(addr[1]));//������ʾ����
	delay_ms(1000);//��ʱ����LED
*/
	}
	
}
