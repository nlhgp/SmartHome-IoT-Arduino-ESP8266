#include <SoftwareSerial.h>
#include <LedControl.h>//7219驱动点阵
#include "DHT.h"//温湿度传感器
#define GREEN 4//灯泡，输出接口
#define pinBuzzer 7//蜂鸣器，mvp
//烟雾传感器
#define AOUT A0
#define Sensor_DO 13
//驱动点阵
// #define DIN 13//SPI模式，MOSI主设备输出，从设备输入
// #define CS 14//SPI模式，CS片选
// #define CLK 15//SPI模式，CLK
//温湿度传感器
#define DHTPIN  8
#define DHTTYPE DHT11
//火焰传感器
#define KEY 12

//网络模块
SoftwareSerial mySerial(10, 11); // RX=10, TX=11（根据实际接线修改引脚号）

//温湿度传感器
DHT dht(DHTPIN, DHTTYPE);

//超声波模块
float distance;
const int echo=3;                          //echO接D3脚
const int trig=2; 

//8*8矩阵
//LedControl lc = LedControl(DIN, CLK, CS, 0);//新建一个类对象

//面部表情
byte smile[8] =   {0x3C, 0x42, 0xA5, 0x81, 0xA5, 0x99, 0x42, 0x3C};//微笑
byte neutral[8] = {0x3C, 0x42, 0xA5, 0x81, 0xBD, 0x81, 0x42, 0x3C};//中性
byte sad[8] =     {0x3C, 0x42, 0xA5, 0x81, 0x99, 0xA5, 0x42, 0x3C};//悲伤

//信号量
char sign=0;

//角度
int pos = 0;

//定时器
int timer(int ms, int id)
{
	static unsigned long start[7]={millis(),millis(),millis(),millis(),millis(),millis(),millis()};//0温度
  unsigned long end = millis();
	if (end - start[id] >= ms)
	{
		start[id] = end;
		return 1;
	}
	return 0;
}

void bee()
{
   digitalWrite(pinBuzzer, HIGH);//输出HIGH电平,停止发声
   delay(500); //等待1000毫秒
   digitalWrite(pinBuzzer, LOW);//输出LOW电平,发声
   delay(500); //等待1000毫秒
}

//打印图形
// void printByte(byte character [])
// {
//   int i = 0;
//   for (i = 0; i < 8; i++)
//   {
//     lc.setRow(0, i, character[i]);//设置点阵单行8个LED状态，每行数据用16进制表示
//   }
// }

void setup() {
  Serial.begin(9600);//波特率

  pinMode(pinBuzzer, OUTPUT); //蜂鸣器

  //超声波
  pinMode(echo,INPUT);                       //设置echo为输入脚
  pinMode(trig,OUTPUT);                      //设置trig为输出脚

  //烟雾感应器
  pinMode(AOUT, INPUT);//定义A0为INPUT模式

  //火焰传感器
  pinMode(KEY,INPUT);//定义火焰传感器为输出接口

  //网络模块    
  mySerial.begin(9600);

  //温湿度传感器
  dht.begin();

  //灯泡
  pinMode(GREEN, OUTPUT);
  digitalWrite(GREEN, LOW);

  //8*8矩阵模块
}

void servopulse(int angle)//舵机脉冲函数
{
  int pulsewidth=(angle*11)+500;  //将角度转化为500-2480的脉宽值，每多转1度，对应高电平多11us
  digitalWrite(A1,HIGH);    //将舵机接口电平至高
  delayMicroseconds(pulsewidth);  //延时脉宽值的微秒数
  digitalWrite(A1,LOW);     //将舵机接口电平至低
  delayMicroseconds(20000-pulsewidth);   
}

void doorpulse(int angle)
{
  int pulsewidth=(angle*11)+500;  //将角度转化为500-2480的脉宽值，每多转1度，对应高电平多11us
  digitalWrite(A2,HIGH);    //将舵机接口电平至高
  delayMicroseconds(pulsewidth);  //延时脉宽值的微秒数
  digitalWrite(A2,LOW);     //将舵机接口电平至低
  delayMicroseconds(20000-pulsewidth);   
}

void loop() {
  // 网络模块，舵机控制逻辑
  if (mySerial.available()) {
    String s = mySerial.readStringUntil('\n');
    s.trim();
    if (s == "open#window") {
      for(int i=0;i<50;i++)  //发送50个脉冲   //为什么是50个？
      {
       servopulse(90);   //引用脉冲函数
      }
    }
    if(s == "close#window") 
    {
      for(int i=0;i<50;i++)  //发送50个脉冲   //为什么是50个？
      {
       servopulse(0);   //引用脉冲函数
      }
    }
    if(s == "open#door")
    {
      for(int i=0;i<50;i++)  //发送50个脉冲   //为什么是50个？
      {
       doorpulse(30);   //引用脉冲函数
      }
    }
    if(s == "close#door")
    {
      for(int i=0;i<50;i++)  //发送50个脉冲   //为什么是50个？
      {
       doorpulse(70);   //引用脉冲函数
      }
    }
  }

  //超声波模块
  float distance;
  if(timer(500,3))
  {
    digitalWrite(trig,LOW);
    delayMicroseconds(20);
    digitalWrite(trig,HIGH);
    delayMicroseconds(20);
    digitalWrite(trig,LOW);                     //发一个20US的高脉冲去触发Trig
    distance  = pulseIn(echo,HIGH);             //计数接收高电平时间
    distance  = distance*340/2/10000;           //计算距离 1：声速：340M/S  2：实际距离1/2声速距离 3：计数时钟为1US
    // Serial.print("距离: ");
    // Serial.print(distance);
    // Serial.println("cm");
    delay(20);                                   //单次测离完成后加20mS的延时再进行下次测量。防止近距离测量时，测量到上次余波，导致测量不准确。
    
    if(distance < 10.0)
    {
      digitalWrite(GREEN, HIGH);//高电平，亮灯
      String s = "#worning#thing_move";
      if(sign==0)
      {
        sign=1;
        mySerial.print(s);//有物体经过警告
        delay(20);
        sign=0;
      }
      bee();
    }

    else
    {
      digitalWrite(GREEN, LOW); //将管脚设置为低电平, 则LED灯灭
    }
  }

  //8*8矩阵模块
  //printByte(smile);

  //烟雾传感器
  int temp;
  temp = analogRead(AOUT); //读取A0的模拟数据
  //Serial.println(temp); //串口输出temp的数据
  //蜂鸣器，烟雾传感器参数到八百蜂鸣器就叫
  if(temp>800)
  {
    String s = "worning#yan";
    Serial.print("检测到火灾风险");
    servopulse(90);//打开窗户
    doorpulse(30);//开门
    if(sign==0)
    {
      sign=1;
      mySerial.print(s);

      delay(50);
      sign=0;
    }
    Serial.print(s);
    bee();
  }

  //火焰传感器
  // if (digitalRead(KEY) == HIGH && timer(500,6))
  // {
  //   Serial.println("明火警告！！");
  //   mySerial.print(F("worning#fire"));
  //   servopulse(90);//打开窗户
  //   digitalWrite(GREEN, HIGH);
  // }

  // else
  // {
  //   digitalWrite(GREEN, LOW);
  // }

  //温湿度传感器
  if(timer(1000,5))
  {
    float h = dht.readHumidity();//读湿度
    float t = dht.readTemperature();//读温度(摄氏度)
    // Serial.print("Humidity:");
    // Serial.print(h);
    // Serial.print("% Temperature:");
    // Serial.print(t);
    // Serial.println("℃");
    if(sign==0)
    {
      sign=1;
      mySerial.print(F("oc#"));
      mySerial.print(t);
      mySerial.print(F("#"));
      mySerial.print(h);
      delay(50);
      sign=0;
    }
  }
}  
