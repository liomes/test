void Ds1302Write(uchar addr, uchar dat)
{
	uchar n;
	RST = 0;
	_nop_();

	SCLK = 0;//先将SCLK置低电平。
	_nop_();
	RST = 1; //然后将RST(CE)置高电平。
	_nop_();

	for (n=0; n<8; n++)//开始传送八位地址命令
	{
		DSIO = addr & 0x01;//数据从低位开始传送
		addr >>= 1;
		SCLK = 1;//数据在上升沿时，DS1302读取数据
		_nop_();
		SCLK = 0;
		_nop_();
	}
	for (n=0; n<8; n++)//写入8位数据
	{
		DSIO = dat & 0x01;
		dat >>= 1;
		SCLK = 1;//数据在上升沿时，DS1302读取数据
		_nop_();
		SCLK = 0;
		_nop_();	
	}	
		 
	RST = 0;//传送数据结束
	_nop_();
}

/*******************************************************************************
* 函 数 名         : Ds1302Read
* 函数功能		   : 读取一个地址的数据
* 输    入         : addr
* 输    出         : dat
*******************************************************************************/

uchar Ds1302Read(uchar addr)
{
	uchar n,dat,dat1;
	RST = 0;
	_nop_();

	SCLK = 0;//先将SCLK置低电平。
	_nop_();
	RST = 1;//然后将RST(CE)置高电平。
	_nop_();

	for(n=0; n<8; n++)//开始传送八位地址命令
	{
		DSIO = addr & 0x01;//数据从低位开始传送
		addr >>= 1;
		SCLK = 1;//数据在上升沿时，DS1302读取数据
		_nop_();
		SCLK = 0;//DS1302下降沿时，放置数据
		_nop_();
	}
	_nop_();
	for(n=0; n<8; n++)//读取8位数据
	{
		dat1 = DSIO;//从最低位开始接收
		dat = (dat>>1) | (dat1<<7);
		SCLK = 1;
		_nop_();
		SCLK = 0;//DS1302下降沿时，放置数据
		_nop_();
	}

	RST = 0;
	_nop_();	//以下为DS1302复位的稳定时间,必须的。
	SCLK = 1;
	_nop_();
	DSIO = 0;
	_nop_();
	DSIO = 1;
	_nop_();
	return dat;	
}

/*******************************************************************************
* 函 数 名         : Ds1302Init
* 函数功能		   : 初始化DS1302.
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

void Ds1302Init()
{
	uchar n;
	Ds1302Write(0x8E,0X00);		 //禁止写保护，就是关闭写保护功能
	for (n=0; n<7; n++)//写入7个字节的时钟信号：分秒时日月周年
	{
		Ds1302Write(WRITE_RTC_ADDR[n],TIME[n]);	
	}
	Ds1302Write(0x8E,0x80);		 //打开写保护功能
}

/*******************************************************************************
* 函 数 名         : Ds1302ReadTime
* 函数功能		   : 读取时钟信息
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

void Ds1302ReadTime()
{
	uchar n;
	for (n=0; n<7; n++)//读取7个字节的时钟信号：分秒时日月周年
	{
		TIME[n] = Ds1302Read(READ_RTC_ADDR[n]);
	}
		
}

/*******************************************************************************
* 函 数 名         : delay
* 函数功能		   : 延时函数，i=1时，大约延时10us
*******************************************************************************/
void delay(u16 i)
{
	while(i--);	
}



/*******************************************************************************
* 函 数 名         : datapros()
* 函数功能		   : 时间读取处理转换函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

void datapros() 	 
{
   	Ds1302ReadTime();
	DisplayData[0] = Disp[TIME[2]/16];				//时
	DisplayData[1] = Disp[TIME[2]&0x0f];				 
	DisplayData[2] = 0x3a;
	DisplayData[3] = Disp[TIME[1]/16];				//分
	DisplayData[4] = Disp[TIME[1]&0x0f];	
	DisplayData[5] = 0x3a;
	DisplayData[6] = Disp[TIME[0]/16];				//秒
	DisplayData[7] = Disp[TIME[0]&0x0f];
}
void datapros1() 	 
{
   	Ds1302ReadTime();
	DisplayData[0] = Disp[TIME[6]/16];				//年
	DisplayData[1] = Disp[TIME[6]&0x0f];				 
	DisplayData[2] = 0x3a;
	DisplayData[3] = Disp[TIME[4]/16];				//月
	DisplayData[4] = Disp[TIME[4]&0x0f];	
	DisplayData[5] = 0x3a;
	DisplayData[6] = Disp[TIME[3]/16];				//日
	DisplayData[7] = Disp[TIME[3]&0x0f];
}
void LcdWriteCom(uchar com)	  //写入命令
{
	LCD1602_E = 0;     //使能
	LCD1602_RS = 0;	   //选择发送命令
	LCD1602_RW = 0;	   //选择写入
	
	LCD1602_DATAPINS = com;     //放入命令
	delay(5);		//等待数据稳定

	LCD1602_E = 1;	          //写入时序
	delay(5);	  //保持时间
	LCD1602_E = 0;
}
void LcdWriteData(uchar dat)			//写入数据
{
	LCD1602_E = 0;	//使能清零
	LCD1602_RS = 1;	//选择输入数据
	LCD1602_RW = 0;	//选择写入

	LCD1602_DATAPINS = dat; //写入数据
	delay(5);

	LCD1602_E = 1;   //写入时序
	delay(5);   //保持时间
	LCD1602_E = 0;
}
void LcdInit()						  //LCD初始化子程序
{
 	LcdWriteCom(0x38);  //开显示
	LcdWriteCom(0x0c);  //开显示不显示光标
	LcdWriteCom(0x06);  //写一个指针加1
	LcdWriteCom(0x01);  //清屏
	LcdWriteCom(0x80);  //设置数据指针起点
}


/*******************************************************************************
* 函数名         :DigDisplay()
* 函数功能		 :数码管显示函数
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void DigDisplay()
{
	u8 i;
	for(i=0;i<8;i++)
	{
		switch(i)	 //位选，选择点亮的数码管，
		{
			case(0):
				LcdWriteCom(0x80); break;//显示第0位
			case(1):
				LcdWriteCom(0x81); break;//显示第1位
			case(2):
				LcdWriteCom(0x82); break;//显示第2位
			case(3):
				LcdWriteCom(0x83); break;//显示第3位
			case(4):
				LcdWriteCom(0x84); break;//显示第4位
			case(5):
				LcdWriteCom(0x85); break;//显示第5位
			case(6):
				LcdWriteCom(0x86); break;//显示第6位
			case(7):
				LcdWriteCom(0x87); break;//显示第7位
		}
		LcdWriteData(DisplayData[i]);//发送数据
		//delay(100); //间隔一段时间扫描	
		
	}		
}
void DigDisplay1()
{
	u8 i;
	for(i=0;i<8;i++)
	{
		switch(i)	 //位选，选择点亮的数码管，
		{
			case(0):
				LcdWriteCom(0xc0); break;//显示第0位
			case(1):
				LcdWriteCom(0xc1); break;//显示第1位
			case(2):
				LcdWriteCom(0xc2); break;//显示第2位
			case(3):
				LcdWriteCom(0xc3); break;//显示第3位
			case(4):
				LcdWriteCom(0xc4); break;//显示第4位
			case(5):
				LcdWriteCom(0xc5); break;//显示第5位
			case(6):
				LcdWriteCom(0xc6); break;//显示第6位
			case(7):
				LcdWriteCom(0xc7); break;//显示第7位
		}
		LcdWriteData(DisplayData[i]);//发送数据
		//delay(100); //间隔一段时间扫描	
		
	}		
}
void DigDisplay3()
{
	u8 i;
	for(i=0;i<12;i++)
	{
		switch(i)	 //位选，选择点亮的数码管，
		{
			case(0):
				LcdWriteCom(0x80); break;//显示第0位
			case(1):
				LcdWriteCom(0x81); break;//显示第1位
			case(2):
				LcdWriteCom(0x82); break;//显示第2位
			case(3):
				LcdWriteCom(0x83); break;//显示第3位
			case(4):
				LcdWriteCom(0x84); break;//显示第4位
			case(5):
				LcdWriteCom(0x85); break;//显示第5位
			case(6):
				LcdWriteCom(0x86); break;//显示第6位
			case(7):
				LcdWriteCom(0x87); break;//显示第7位
			case(8):
				LcdWriteCom(0x88); break;//显示第0位
			case(9):
				LcdWriteCom(0x89); break;//显示第1位
			case(10):
				LcdWriteCom(0x8A); break;//显示第2位
			case(11):
				LcdWriteCom(0x8B); break;//显示第3位
		}
		LcdWriteData(xingming[i]);//发送数据
		//delay(100); //间隔一段时间扫描	
		
	}		
}
void delay1(unsigned int i)   //1ms
{
    unsigned char k;
    while(i--)
    for(k=0;k<120;k++);
}

char scan_key()
{
	char m,n;
	KEY=0x0f;
	if(KEY!=0x0f)//判断按钮是否按下
	{
		delay1(5);	 //消抖
		if(KEY!=0x0f)//判断按钮是否按下
		{
		   switch(KEY)//转换为00001111，按下置零，检测低四位：列
	       {
		      case 0x07:n=0;break;
		      case 0x0b:n=1;break;
		      case 0x0d:n=2;break;
			  case 0x0e:n=3;break;
			  default:break;
	       }
		   KEY=0xf0;//转换11110000
		   switch(KEY)//转换为11110000，按下置零，检测高四位：行
		   {
		   	  case 0x70:m=0;break;
			  case 0xb0:m=1;break;
			  case 0xd0:m=2;break;
			  case 0xe0:m=3;break;
			  default:break;
		   }
		}
	}
	  
	if(KEY==0x0f)return -1;//没有按钮动作返回-1
	else return(m*4+n);	//用行列数求按钮位置返回主函数
}
