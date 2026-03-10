#include "M5311.h"
#include "string.h"
#include "usart.h"
#include "led.h"

char *strx,*extstrx;
M5311 M5311_Status;
unsigned char socketnum=0;//当前的socket号码
void Clear_Buffer(void)//清空缓存
{
		u8 i;
		for(i=0;i<200;i++)
		Rx_Buffer[i]=0;//缓存
		Rx_Counter=0;
	
}
void M5311_Init(void)
{	
//	LED_R (1);
//    printf("AT\r\n"); 
//    delay_ms(300);
//    strx=strstr((const char*)Rx_Buffer,(const char*)"OK");//返回OK
//    Clear_Buffer();	

//    while(strx==NULL)
//    {
//        Clear_Buffer();	
//        printf("AT\r\n"); 
//        delay_ms(300);
//        strx=strstr((const char*)Rx_Buffer,(const char*)"OK");//返回OK
//    }

//    printf("AT+MIPLCLOSE=0\r\n");//注销登录
//    delay_ms(300);
//    printf("AT+MIPLDELOBJ=0,3200\r\n");//注销资源
//    delay_ms(300);
//    printf("AT+MIPLDELETE=0\r\n");//删除通讯
//    delay_ms(300);
//    printf("AT+CFUN=1\r\n");//
//    delay_ms(1000);
//		Clear_Buffer();	
    printf("AT+CIMI\r\n");//获取卡号，类似是否存在卡的意思，比较重要。IMSI
    delay_ms(1000);
//		Uart3_SendStr(Rx_Buffer);
		delay_ms(500);
		Clear_Buffer();	
    printf("AT+GSN\r\n");//获取IMEI
    delay_ms(1000);
//		Uart3_SendStr(Rx_Buffer);
//    strx=strstr((const char*)Rx_Buffer,(const char*)"460");//返460，表明识别到卡了
//    Clear_Buffer();	
//    while(strx==NULL)
//    {
//        Clear_Buffer();	
//        printf("AT+CIMI\r\n");//获取卡号，类似是否存在卡的意思，比较重要。
//        delay_ms(300);
//        strx=strstr((const char*)Rx_Buffer,(const char*)"460");//返回OK,说明卡是存在的
//    }
//    printf("AT+GSN\r\n");//获取IMEI
//    delay_ms(300);
//    Clear_Buffer();	
//		printf("AT+CGATT=1\r\n");//激活网络，PDP
//		delay_ms(300);
//		strx=strstr((const char*)Rx_Buffer,(const char*)"OK");//返OK
//		Clear_Buffer();	
//		printf("AT+CGATT?\r\n");//查询激活状态
//		delay_ms(300);
//		strx=strstr((const char*)Rx_Buffer,(const char*)"+CGATT: 1");//返1
//		Clear_Buffer();	
//		while(strx==NULL)
//		{
//            Clear_Buffer();	
//            printf("AT+CGATT?\r\n");//获取激活状态
//            delay_ms(300);
//            strx=strstr((const char*)Rx_Buffer,(const char*)"+CGATT: 1");//返回1,表明注网成功
//		}
		  printf("AT+CSQ\r\n");//查看获取CSQ值
        delay_ms(300);
//				Uart3_SendStr(Rx_Buffer);
        Clear_Buffer();
}

//创建对象连接到平台
void M5311_CreateInstance(void)
{    u8 i=0;	
	strx=strstr((const char*)Rx_Buffer,(const char*)"+IP:");//对象创建成功
//	Uart3_SendStr(Rx_Buffer);
	while(strx==NULL)
  {
    strx=strstr((const char*)Rx_Buffer,(const char*)"+IP:");//
  }
    delay_ms(300);	LED_R(1);
	  Clear_Buffer();
    printf("AT+MIPLCREATE\r\n");//设置参数，默认可以直接用代码提供的
    delay_ms(500);
    strx=strstr((const char*)Rx_Buffer,(const char*)"+MIPLCREATE:0");//对象创建成功
   while(strx==NULL)
  {
    strx=strstr((const char*)Rx_Buffer,(const char*)"+MIPLCREATE:0");//
  } 
//	  Uart3_SendStr(Rx_Buffer);
    Clear_Buffer();	;

	  delay_ms(500);
 
    printf("AT+MIPLADDOBJ=0,3200,1,\"1\",0,1\r\n");//订阅 Object//mpu6050
    delay_ms(500);
    strx=strstr((const char*)Rx_Buffer,(const char*)"OK");//对象创建成功
   while(strx==NULL)
  {
        strx=strstr((const char*)Rx_Buffer,(const char*)"OK");//
   } 
//	  Uart3_SendStr(Rx_Buffer);
    Clear_Buffer(); 
	  printf("AT+MIPLDISCOVERRSP=0,3200,1,4,\"5750\"\r\n");//订阅 Resource
    delay_ms(500);
	  strx=strstr((const char*)Rx_Buffer,(const char*)"OK");//对象创建成功
   while(strx==NULL)
  {
        strx=strstr((const char*)Rx_Buffer,(const char*)"OK");//
   } 
//		Uart3_SendStr(Rx_Buffer);
    Clear_Buffer();
	 delay_ms(1000);
    printf("AT+MIPLADDOBJ=0,3200,1,\"1\",0,1\r\n");//订阅 Object//气压计
   delay_ms(500);
    strx=strstr((const char*)Rx_Buffer,(const char*)"OK");//对象创建成功
   while(strx==NULL)
  {
        strx=strstr((const char*)Rx_Buffer,(const char*)"OK");//
   } 
//	  Uart3_SendStr(Rx_Buffer);
    Clear_Buffer();
    printf("AT+MIPLDISCOVERRSP=0,3200,1,4,\"5751\"\r\n");//订阅 Resource
    delay_ms(500);
    strx=strstr((const char*)Rx_Buffer,(const char*)"OK");//对象创建成功
   while(strx==NULL)
  {
        strx=strstr((const char*)Rx_Buffer,(const char*)"OK");//
   } 
//	  Uart3_SendStr(Rx_Buffer);
    Clear_Buffer();
	 delay_ms(1000);
    printf("AT+MIPLADDOBJ=0,3336,1,\"1\",0,1\r\n");//订阅 Object//GPS
    delay_ms(500);
    strx=strstr((const char*)Rx_Buffer,(const char*)"OK");//对象创建成功
   while(strx==NULL)
  {
        strx=strstr((const char*)Rx_Buffer,(const char*)"OK");//
   } 
//	  Uart3_SendStr(Rx_Buffer);
    Clear_Buffer();
    printf("AT+MIPLDISCOVERRSP=0,3336,1,4,\"5514\"\r\n");//订阅 Resource
    delay_ms(500);
    strx=strstr((const char*)Rx_Buffer,(const char*)"OK");//对象创建成功
   while(strx==NULL)
  {
        strx=strstr((const char*)Rx_Buffer,(const char*)"OK");//
   } 
//	  Uart3_SendStr(Rx_Buffer);
    Clear_Buffer();
    printf("AT+MIPLADDOBJ=0,3300,1,\"1\",0,1\r\n");//订阅 Object//GPS
    delay_ms(500);
    strx=strstr((const char*)Rx_Buffer,(const char*)"OK");//对象创建成功
   while(strx==NULL)
  {
        strx=strstr((const char*)Rx_Buffer,(const char*)"OK");//
   } 
//	  Uart3_SendStr(Rx_Buffer);
    Clear_Buffer();
    printf("AT+MIPLDISCOVERRSP=0,3300,1,4,\"5701\"\r\n");//订阅 Resource
    delay_ms(500);
    strx=strstr((const char*)Rx_Buffer,(const char*)"OK");//对象创建成功
   while(strx==NULL)
  {
        strx=strstr((const char*)Rx_Buffer,(const char*)"OK");//
   } 
//	  Uart3_SendStr(Rx_Buffer);
    Clear_Buffer();
	 
    printf("AT+MIPLOPEN=0,3600,60\r\n");//连接平台资源
     delay_ms(300);
    strx=strstr((const char*)Rx_Buffer,(const char*)"+MIPLEVENT:0,6");//返回0,6表明登录成功，可以看到设备在线
   while(strx==NULL)
  {
      strx=strstr((const char*)Rx_Buffer,(const char*)"+MIPLEVENT:0,6");//获取资源号码
   } 
//	  Uart3_SendStr(Rx_Buffer);
	  delay_ms(500);
    Clear_Buffer();
 }
void ONENET_Senddata(double*buffer1,double*buffer2)//往平台主动发送数据
 {
	   Clear_Buffer();
     printf("AT+MIPLNOTIFY=0,0,3200,0,5750,1,20,\"%lf,%lf\",0,0\r\n",*buffer1,*buffer2);//,buffer,ackstr);//发送4个内容的字符串
//     delay_ms(300);
//     strx=strstr((const char*)Rx_Buffer,(const char*)"OK");//判断是否发送成功并响应
//     while(strx==NULL)
//     {
//      strx=strstr((const char*)Rx_Buffer,(const char*)"OK");//
////     } 
//    delay_ms(300);
 }

void ONENET_Readdata(void)//等待服务器的读请求
{
    u8 i=0,count=0;
    strx=strstr((const char*)Rx_Buffer,(const char*)"+MIPLREAD:");//有读请求产生,这里根据不同的请求发送不同的指令需求
    if(strx)//读取到指令请求之后，获取内容数据
    {
          delay_ms(100); 
        while(1)
        {
            if(strx[11+i]==',')
                count++;
            M5311_Status.ReadSource[i]=strx[11+i];
            if(count>=2)//遇到2个逗号了，需要跳出
            {
               M5311_Status.ReadSource[i+1] ='1';
               break;//跳出
            }
            i++;
        }
        while(strx[11+i]!=0x0D)//没有碰到结束符
        { 
            
            M5311_Status.ReadSource[i+2]=strx[11+i];//指针移动往后多移动三个位置，继续接收下面的数据  
            i++;
        }
         strx = strchr(strx+1, ',');  
        Clear_Buffer();
        printf("AT+MIPLREADRSP=%s,4,4,2.35,0,0\r\n",M5311_Status.ReadSource);//回复请求，将数据传输上去
        delay_ms(300); 
        strx=strstr((const char*)Rx_Buffer,(const char*)"OK");//
       while(strx==NULL)
      {
            strx=strstr((const char*)Rx_Buffer,(const char*)"OK");//
       } 
        Clear_Buffer();
    }
      
    
    
}
