#include "bh1750.h"
#include "dwt.h"
/***************************************************************
* 文件名称: BH1750
* 作    者: 物联网俱乐部
* 版    本: V1.0
* 编写日期: 2018-12-01
* 功    能: 光照强度采集
****************************************************************
* 硬件平台: 本例程配套物联网俱乐部EVB_L1开发板使用
* 淘    宝: iot-club.taobao.com、https://shop449662000.taobao.com
* 论    坛: www.iot-club.cn 、www.iotclub.net
****************************************************************/

static float result_lx=0;
static uint8_t BUF[2]={0};
static uint16_t result=0;

/***************************************************************
* 函数名称: I2C_Start
* 说    明: CPU发起I2C总线启动信号
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void I2C_Start(void)
{
	/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
	SDA_OUT();
	BH1750_SDIN_Set(); 
	BH1750_SCLK_Set(); 
	delay_us(4);
	BH1750_SDIN_Clr(); 
	delay_us(4);
	BH1750_SCLK_Clr(); 
}

/***************************************************************
* 函数名称: I2C_Stop
* 说    明: CPU发起I2C总线停止信号
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void I2C_Stop(void)
{
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	SDA_OUT();
	BH1750_SCLK_Clr(); 
	BH1750_SDIN_Clr();  
	delay_us(4);
	BH1750_SCLK_Set(); 	
	BH1750_SDIN_Set();  
	delay_us(4);
}

/***************************************************************
* 函数名称: I2C_WaitAck
* 说    明: CPU产生一个时钟，并读取器件的ACK应答信号
* 参    数: 无
* 返 回 值: 无
***************************************************************/
uint8_t I2C_WaitAck(void)
{
  uint8_t ucErrTime=0;
	SDA_IN(); 
	BH1750_SDIN_Set();  	/* CPU释放SDA总线 */
	delay_us(4);
	BH1750_SCLK_Set(); 	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	delay_us(4);
	while (BH1750_SDA_READ())	/* CPU读取SDA口线状态 */
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			I2C_Stop();
			return 1;
		}
	}
	
	BH1750_SCLK_Clr(); 
	return 0;
}

/***************************************************************
* 函数名称: I2C_Ack
* 说    明: CPU产生一个ACK信号
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void I2C_Ack(void)
{

	BH1750_SCLK_Clr(); 
	SDA_OUT(); 
	BH1750_SDIN_Clr();  	/* CPU驱动SDA = 0 */
	delay_us(2);
	BH1750_SCLK_Set(); 	/* CPU产生1个时钟 */
	delay_us(2);
	BH1750_SCLK_Clr(); 
}

/***************************************************************
* 函数名称: I2C_NAck
* 说    明: CPU产生1个NACK信号
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void I2C_NAck(void)
{
	
	BH1750_SCLK_Clr(); 
	SDA_OUT();
	BH1750_SDIN_Set();  	/* CPU驱动SDA = 1 */
	delay_us(2);
	BH1750_SCLK_Set(); 	/* CPU产生1个时钟 */
	delay_us(2);
	BH1750_SCLK_Clr(); 
}

/***************************************************************
* 函数名称: I2C_SendByte
* 说    明: CPU向I2C总线设备发送8bit数据
* 参    数: Byte,等待发送的字节
* 返 回 值: 无
***************************************************************/
void I2C_SendByte(uint8_t Byte)
{
  uint8_t t;  
      
    SDA_OUT();                                                      //sda???  
    BH1750_SCLK_Clr();                                                   //??????????  
    for (t = 0; t < 8; t++)  
    {  
        if((Byte & 0x80) >> 7)
		{
			BH1750_SDIN_Set();
		}
		else 
			BH1750_SDIN_Clr();
        Byte <<= 1;  
        delay_us(2);  
        BH1750_SCLK_Set();  
        delay_us(2);  
        BH1750_SCLK_Clr();  
        delay_us(2);  
    } 
}

/***************************************************************
* 函数名称: I2C_SendByte
* 说    明: CPU向I2C总线设备发送8bit数据
* 参    数: Byte,等待发送的字节
* 返 回 值: 无
***************************************************************/
uint8_t I2C_ReadByte(uint8_t ack)
{
	uint8_t i;
	uint8_t value;
	SDA_IN();
	/* 读到第1个bit为数据的bit7 */
	value = 0;
	for (i = 0; i < 8; i++)  
	{  
		BH1750_SCLK_Clr();  
		delay_us(2);  
		BH1750_SCLK_Set();  
		value <<= 1;  
		if (BH1750_SDA_READ() ) 
			value++;  
		delay_us(1);  
	}  
	if (!ack)
		I2C_NAck();//发送nACK
	else
		I2C_Ack(); //发送ACK  
	return value;
}

/***************************************************************
* 函数名称: I2C_CheckDevice
* 说    明: 检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
*			在访问I2C设备前，请先调用 I2C_CheckDevice() 检测I2C设备是否正常，该函数会配置GPIO
* 参    数: Address,设备的I2C总线地址
* 返 回 值: 0 表示正确
*			1 表示未探测到
***************************************************************/
uint8_t I2C_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;
	I2C_Start();		/* 发送启动信号 */
	/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
	I2C_SendByte(_Address | I2C_WR);
	ucAck = I2C_WaitAck();	/* 检测设备的ACK应答 */
	I2C_Stop();			/* 发送停止信号 */
	return ucAck;
}

/***************************************************************
* 函数名称: Cmd_Write_BH1750
* 说    明: 写命令
* 参    数: cmd,想发送的命令
* 返 回 值: 无
***************************************************************/
void Cmd_Write_BH1750(uint8_t cmd)
{
    I2C_Start();                  //起始信号
    I2C_SendByte(BH1750_Addr+0);   //发送设备地址+写信号
	while(I2C_WaitAck());
	delay_us(100);
    I2C_SendByte(cmd);    //内部寄存器地址
	while(I2C_WaitAck());
	delay_us(100);
    I2C_Stop();                   //发送停止信号
	delay_ms(5);
}

/***************************************************************
* 函数名称: Init_BH1750
* 说    明: 写命令初始化BH1750
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Init_BH1750(void)
{
	dwt_delay_init(SystemCoreClock);
	Cmd_Write_BH1750(0x01);
}

/***************************************************************
* 函数名称: Start_BH1750
* 说    明: 启动BH1750
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Start_BH1750(void)
{
	Cmd_Write_BH1750(BH1750_ON);	 //power on
	Cmd_Write_BH1750(BH1750_CON);
}

/***************************************************************
* 函数名称: Read_BH1750
* 说    明: 读BH1750采集的数据
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Read_BH1750(void)
{   
	uint8_t i;	
	I2C_Start();                          //起始信号
	I2C_SendByte(BH1750_Addr+1);         //发送设备地址+读信号
	while(I2C_WaitAck());
	for (i=0; i<3; i++)                      //á?D??áè?6??μ??·êy?Y￡?′?′￠?DBUF
	{     
		if (i ==3)
		{
			BUF[i]=I2C_ReadByte(0); 
		}
		else
		{	
			BUF[i]=I2C_ReadByte(1);  //发送ACK	
		}
	}
	//发送NACK
	I2C_Stop();                          //停止信号
	delay_ms(5);
}

/***************************************************************
* 函数名称: Convert_BH1750
* 说    明: 数值转换
* 参    数: 无
* 返 回 值: 光强值
***************************************************************/
float Convert_BH1750(void)
{
	Start_BH1750();
	delay_ms(180);
	Read_BH1750();
	result=BUF[0];
	result=(result<<8)+BUF[1];  //合成数据，即光照数据	
	result_lx=(float)(result/1.2);
  return result_lx;
}


