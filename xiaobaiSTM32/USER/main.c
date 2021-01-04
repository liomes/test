#include "led.h"
#include "key.h"
#include "delay.h"
#include "beep.h"
#include "sys.h"
#include "timer.h"
#include "usart.h"
#include "dht11.h"
#include "bh1750.h"
#include "exti.h"
#include "oled.h"
#include "esp8266.h"
#include "onenet.h"
#include "stdio.h"

u8 alarmFlag = 0;//�Ƿ񱨾���flag��־
u8 alarm_is_free = 10; //�������Ƿ��ֶ�������������ֶ�����������Ϊ0;

u8 humidityH; //ʪ����������
u8 humidityL; //ʪ��С������
u8 temperatureH; //�¶���������
u8 temperatureL; //�¶�С������

float Light = 0; //���ն�

char PUB_BUF[256];//�ϴ����ݵ�

const char *devSubTopic[] = {"/mysmarthome/sub"};
const char devPubTopic[] = "/mysmarthome/pub";

//ALIENTEK miniSTM32������ʵ��1
//�����ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾
 int main(void)
 {	
	unsigned short timeCount = 0;	//���ͼ������
	unsigned char *dataPtr = NULL;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2 
	LED_Init();		  	//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();
	EXTIX_Init();		  //�ⲿ�жϳ�ʼ��(��������)
	BEEP_Init(); 
	DHT11_Init();
	BH1750_Init();
	Usart1_Init(115200); //debug����
	Usart2_Init(115200); //stm32-8266ͨѶ����
	 
	OLED_Init();
	OLED_ColorTurn(0);//0������ʾ��1 ��ɫ��ʾ
  OLED_DisplayTurn(0);//0������ʾ 1 ��Ļ��ת��ʾ
	OLED_Clear();
	
	TIM2_Int_Init(4999,7199);
	TIM3_Int_Init(2499,7199);
	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
	ESP8266_Init();					//��ʼ��ESP8266
	
	while(OneNet_DevLink())			//����OneNET
	delay_ms(500);
	
	BEEP = 0; //���б�ʾ����ɹ�
	delay_ms(250); //
	BEEP=1;
	
	OneNet_Subscribe(devSubTopic, 1);
	if(humidityH > 80 || temperatureH > 30) alarmFlag=1;
	
	while(1)
	{
	
		if(timeCount % 40 == 0)//1000ms/25=40 1sִ��һ��
		{
			/********** ��ʪ�ȴ�������ȡ����**************/
			DHT11_Read_Data(&humidityH,&humidityL,&temperatureH,&temperatureL);
			UsartPrintf(USART_DEBUG, "ʪ��:%d.%d  �¶�:%d.%d",humidityH,humidityL,temperatureH,temperatureL);
			/********** ���նȴ�������ȡ����**************/
			if (!i2c_CheckDevice(BH1750_Addr))
			{
				Light = LIght_Intensity();
				UsartPrintf(USART_DEBUG,"��ǰ���ն�Ϊ��%.1f lx\r\n", Light);	
			}	
			if(alarm_is_free == 10)//��������Ȩ�����Ƿ����alarm_is_free>0��ʼֵΪ��10
			{
				if(humidityH < 80 && temperatureH < 30 && Light < 1000) alarmFlag=0;
				else alarmFlag = 1;
			}
			if(alarm_is_free<10)alarm_is_free++;
			//UsartPrintf(USART_DEBUG,"alarm_is_free=%d\n", alarm_is_free);
			//UsartPrintf(USART_DEBUG,"alarmFlag=%d\n", alarmFlag);
		}
		if(++timeCount >= 200)	//5000ms / 25 = 200���ͼ��5s
		{
			UsartPrintf(USART_DEBUG, "OneNet_Publish\r\n");
			sprintf(PUB_BUF,"{\"Hum\":%d.%d,\"Temp\":%d.%d,\"Light\":%.1f}",
			humidityH,humidityL,temperatureH,temperatureL,Light);
			OneNet_Publish(devPubTopic, PUB_BUF);
			
			timeCount = 0;
			ESP8266_Clear();
		}
		
		dataPtr = ESP8266_GetIPD(3);  //���ݽ���
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);
		
		delay_ms(10);
	}
 }

