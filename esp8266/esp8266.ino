#include <ESP8266WiFi.h>

WiFiClient CLIENT; 
const char *HOST = "192.168.195.56";
const int PORT = 8000;
const char *ssid = "cctv10086";
const char *password = "88888888";


void setupWifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup ( void ) { 
  Serial.begin(9600);//arduino与8266通信串口
  setupWifi();
} 

int loopCount=0; 

void loop ( void ) {

  //断线重连
  while (!CLIENT.connected()){
    Serial.println("not connected");
    if (!CLIENT.connect(HOST, PORT)){
      Serial.println("connect server ....");
      delay(500);
    }
  }

  //透传
  while (CLIENT.available()){
    //8266向arduino发信息
    Serial.write(CLIENT.read());
  }

  //8266接收arduino信息
  while (Serial.available() > 0) {
    CLIENT.write(Serial.read());
  }

  //10s发送一次心跳包
  delay(500);
  loopCount++;
  if(loopCount == 20){
    //向服务器传递信息
    CLIENT.println("ping");
    loopCount=0;
  }
}
