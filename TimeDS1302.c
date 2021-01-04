void Ds1302Write(uchar addr, uchar dat)
{
	uchar n;
	RST = 0;
	_nop_();

	SCLK = 0;//�Ƚ�SCLK�õ͵�ƽ��
	_nop_();
	RST = 1; //Ȼ��RST(CE)�øߵ�ƽ��
	_nop_();

	for (n=0; n<8; n++)//��ʼ���Ͱ�λ��ַ����
	{
		DSIO = addr & 0x01;//���ݴӵ�λ��ʼ����
		addr >>= 1;
		SCLK = 1;//������������ʱ��DS1302��ȡ����
		_nop_();
		SCLK = 0;
		_nop_();
	}
	for (n=0; n<8; n++)//д��8λ����
	{
		DSIO = dat & 0x01;
		dat >>= 1;
		SCLK = 1;//������������ʱ��DS1302��ȡ����
		_nop_();
		SCLK = 0;
		_nop_();	
	}	
		 
	RST = 0;//�������ݽ���
	_nop_();
}

/*******************************************************************************
* �� �� ��         : Ds1302Read
* ��������		   : ��ȡһ����ַ������
* ��    ��         : addr
* ��    ��         : dat
*******************************************************************************/

uchar Ds1302Read(uchar addr)
{
	uchar n,dat,dat1;
	RST = 0;
	_nop_();

	SCLK = 0;//�Ƚ�SCLK�õ͵�ƽ��
	_nop_();
	RST = 1;//Ȼ��RST(CE)�øߵ�ƽ��
	_nop_();

	for(n=0; n<8; n++)//��ʼ���Ͱ�λ��ַ����
	{
		DSIO = addr & 0x01;//���ݴӵ�λ��ʼ����
		addr >>= 1;
		SCLK = 1;//������������ʱ��DS1302��ȡ����
		_nop_();
		SCLK = 0;//DS1302�½���ʱ����������
		_nop_();
	}
	_nop_();
	for(n=0; n<8; n++)//��ȡ8λ����
	{
		dat1 = DSIO;//�����λ��ʼ����
		dat = (dat>>1) | (dat1<<7);
		SCLK = 1;
		_nop_();
		SCLK = 0;//DS1302�½���ʱ����������
		_nop_();
	}

	RST = 0;
	_nop_();	//����ΪDS1302��λ���ȶ�ʱ��,����ġ�
	SCLK = 1;
	_nop_();
	DSIO = 0;
	_nop_();
	DSIO = 1;
	_nop_();
	return dat;	
}

/*******************************************************************************
* �� �� ��         : Ds1302Init
* ��������		   : ��ʼ��DS1302.
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/

void Ds1302Init()
{
	uchar n;
	Ds1302Write(0x8E,0X00);		 //��ֹд���������ǹر�д��������
	for (n=0; n<7; n++)//д��7���ֽڵ�ʱ���źţ�����ʱ��������
	{
		Ds1302Write(WRITE_RTC_ADDR[n],TIME[n]);	
	}
	Ds1302Write(0x8E,0x80);		 //��д��������
}

/*******************************************************************************
* �� �� ��         : Ds1302ReadTime
* ��������		   : ��ȡʱ����Ϣ
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/

void Ds1302ReadTime()
{
	uchar n;
	for (n=0; n<7; n++)//��ȡ7���ֽڵ�ʱ���źţ�����ʱ��������
	{
		TIME[n] = Ds1302Read(READ_RTC_ADDR[n]);
	}
		
}

/*******************************************************************************
* �� �� ��         : delay
* ��������		   : ��ʱ������i=1ʱ����Լ��ʱ10us
*******************************************************************************/
void delay(u16 i)
{
	while(i--);	
}



/*******************************************************************************
* �� �� ��         : datapros()
* ��������		   : ʱ���ȡ����ת������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/

void datapros() 	 
{
   	Ds1302ReadTime();
	DisplayData[0] = Disp[TIME[2]/16];				//ʱ
	DisplayData[1] = Disp[TIME[2]&0x0f];				 
	DisplayData[2] = 0x3a;
	DisplayData[3] = Disp[TIME[1]/16];				//��
	DisplayData[4] = Disp[TIME[1]&0x0f];	
	DisplayData[5] = 0x3a;
	DisplayData[6] = Disp[TIME[0]/16];				//��
	DisplayData[7] = Disp[TIME[0]&0x0f];
}
void datapros1() 	 
{
   	Ds1302ReadTime();
	DisplayData[0] = Disp[TIME[6]/16];				//��
	DisplayData[1] = Disp[TIME[6]&0x0f];				 
	DisplayData[2] = 0x3a;
	DisplayData[3] = Disp[TIME[4]/16];				//��
	DisplayData[4] = Disp[TIME[4]&0x0f];	
	DisplayData[5] = 0x3a;
	DisplayData[6] = Disp[TIME[3]/16];				//��
	DisplayData[7] = Disp[TIME[3]&0x0f];
}
void LcdWriteCom(uchar com)	  //д������
{
	LCD1602_E = 0;     //ʹ��
	LCD1602_RS = 0;	   //ѡ��������
	LCD1602_RW = 0;	   //ѡ��д��
	
	LCD1602_DATAPINS = com;     //��������
	delay(5);		//�ȴ������ȶ�

	LCD1602_E = 1;	          //д��ʱ��
	delay(5);	  //����ʱ��
	LCD1602_E = 0;
}
void LcdWriteData(uchar dat)			//д������
{
	LCD1602_E = 0;	//ʹ������
	LCD1602_RS = 1;	//ѡ����������
	LCD1602_RW = 0;	//ѡ��д��

	LCD1602_DATAPINS = dat; //д������
	delay(5);

	LCD1602_E = 1;   //д��ʱ��
	delay(5);   //����ʱ��
	LCD1602_E = 0;
}
void LcdInit()						  //LCD��ʼ���ӳ���
{
 	LcdWriteCom(0x38);  //����ʾ
	LcdWriteCom(0x0c);  //����ʾ����ʾ���
	LcdWriteCom(0x06);  //дһ��ָ���1
	LcdWriteCom(0x01);  //����
	LcdWriteCom(0x80);  //��������ָ�����
}


/*******************************************************************************
* ������         :DigDisplay()
* ��������		 :�������ʾ����
* ����           : ��
* ���         	 : ��
*******************************************************************************/
void DigDisplay()
{
	u8 i;
	for(i=0;i<8;i++)
	{
		switch(i)	 //λѡ��ѡ�����������ܣ�
		{
			case(0):
				LcdWriteCom(0x80); break;//��ʾ��0λ
			case(1):
				LcdWriteCom(0x81); break;//��ʾ��1λ
			case(2):
				LcdWriteCom(0x82); break;//��ʾ��2λ
			case(3):
				LcdWriteCom(0x83); break;//��ʾ��3λ
			case(4):
				LcdWriteCom(0x84); break;//��ʾ��4λ
			case(5):
				LcdWriteCom(0x85); break;//��ʾ��5λ
			case(6):
				LcdWriteCom(0x86); break;//��ʾ��6λ
			case(7):
				LcdWriteCom(0x87); break;//��ʾ��7λ
		}
		LcdWriteData(DisplayData[i]);//��������
		//delay(100); //���һ��ʱ��ɨ��	
		
	}		
}
void DigDisplay1()
{
	u8 i;
	for(i=0;i<8;i++)
	{
		switch(i)	 //λѡ��ѡ�����������ܣ�
		{
			case(0):
				LcdWriteCom(0xc0); break;//��ʾ��0λ
			case(1):
				LcdWriteCom(0xc1); break;//��ʾ��1λ
			case(2):
				LcdWriteCom(0xc2); break;//��ʾ��2λ
			case(3):
				LcdWriteCom(0xc3); break;//��ʾ��3λ
			case(4):
				LcdWriteCom(0xc4); break;//��ʾ��4λ
			case(5):
				LcdWriteCom(0xc5); break;//��ʾ��5λ
			case(6):
				LcdWriteCom(0xc6); break;//��ʾ��6λ
			case(7):
				LcdWriteCom(0xc7); break;//��ʾ��7λ
		}
		LcdWriteData(DisplayData[i]);//��������
		//delay(100); //���һ��ʱ��ɨ��	
		
	}		
}
void DigDisplay3()
{
	u8 i;
	for(i=0;i<12;i++)
	{
		switch(i)	 //λѡ��ѡ�����������ܣ�
		{
			case(0):
				LcdWriteCom(0x80); break;//��ʾ��0λ
			case(1):
				LcdWriteCom(0x81); break;//��ʾ��1λ
			case(2):
				LcdWriteCom(0x82); break;//��ʾ��2λ
			case(3):
				LcdWriteCom(0x83); break;//��ʾ��3λ
			case(4):
				LcdWriteCom(0x84); break;//��ʾ��4λ
			case(5):
				LcdWriteCom(0x85); break;//��ʾ��5λ
			case(6):
				LcdWriteCom(0x86); break;//��ʾ��6λ
			case(7):
				LcdWriteCom(0x87); break;//��ʾ��7λ
			case(8):
				LcdWriteCom(0x88); break;//��ʾ��0λ
			case(9):
				LcdWriteCom(0x89); break;//��ʾ��1λ
			case(10):
				LcdWriteCom(0x8A); break;//��ʾ��2λ
			case(11):
				LcdWriteCom(0x8B); break;//��ʾ��3λ
		}
		LcdWriteData(xingming[i]);//��������
		//delay(100); //���һ��ʱ��ɨ��	
		
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
	if(KEY!=0x0f)//�жϰ�ť�Ƿ���
	{
		delay1(5);	 //����
		if(KEY!=0x0f)//�жϰ�ť�Ƿ���
		{
		   switch(KEY)//ת��Ϊ00001111���������㣬������λ����
	       {
		      case 0x07:n=0;break;
		      case 0x0b:n=1;break;
		      case 0x0d:n=2;break;
			  case 0x0e:n=3;break;
			  default:break;
	       }
		   KEY=0xf0;//ת��11110000
		   switch(KEY)//ת��Ϊ11110000���������㣬������λ����
		   {
		   	  case 0x70:m=0;break;
			  case 0xb0:m=1;break;
			  case 0xd0:m=2;break;
			  case 0xe0:m=3;break;
			  default:break;
		   }
		}
	}
	  
	if(KEY==0x0f)return -1;//û�а�ť��������-1
	else return(m*4+n);	//����������ťλ�÷���������
}
