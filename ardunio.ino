#include <RTClib.h>
PCF8563 rtc;
#define music_num_MAX 9 //最多存放9首歌曲
#include <SoftwareSerial.h>
//用户自定义部分-----------------
#include <Wire.h>
//EEPROM---------------------
#include <EEPROM.h>
#define EEPROM_write(address, p) {int i = 0; byte *pp = (byte*)&(p);for(; i < sizeof(p); i++) EEPROM.write(address+i, pp[i]);}
#define EEPROM_read(address, p)  {int i = 0; byte *pp = (byte*)&(p);for(; i < sizeof(p); i++) pp[i]=EEPROM.read(address+i);}
//#define MODE_loopAll 0  //一直循环
#define SSID        "iPhone X" //改为你的Wi-Fi名称
#define PASSWORD    "12345678"//Wi-Fi密码
#define HOST_NAME   "api.heclouds.com"
#define DEVICEID   "503152785" //OneNet上的设备ID
#define PROJECTID "184270" //OneNet上的产品ID
#define HOST_PORT   (80)
String apiKey="kPtZsRKG5Mh2YZZnK29G4r=FVQg="; //与你的设备绑定的APIKey
static const byte  GETDATA[]  PROGMEM = {
  "GET https://api.heclouds.com/devices/503152785/datapoints?datastream_id=id,status,vol&limit=1 HTTP/1.1\r\nHost:api.heclouds.com\r\napi-key:kPtZsRKG5Mh2YZZnK29G4r=FVQg=\r\nConnection: close\r\n\r\n"
};//关键头文件
 /*"GET https://api.heclouds.com/devices/23334240/datapoints?datastream_id=id,status,precent&limit=1 //url+  【 datapoints？datastream id=】 你的几个数据名称【&limit=】（几个数据1个表示最近）
 *  HTTP/1.1\r\n
 *  Host:api.heclouds.com//域名
 *  \r\n
 *  api-key:sCCE3BRbnvyyKsX1iWY7=dSFlME=///API
 *  \r\nConnection: close\r\n\r\n"
*/
#define INTERVAL_SENSOR   17000             //定义传感器采样时间间隔  597000
#define INTERVAL_NET      17000             //定义发送时间
//传感器部分================================   
#include <Wire.h>                                  //调用库  
#include <ESP8266.h>
#include <I2Cdev.h>                                //调用库  
/*******温湿度*******/
#include <Microduino_SHT2x.h>
/*******光照*******/
#define  sensorPin_1  A0
#define IDLE_TIMEOUT_MS  3000      // Amount of time to wait (in milliseconds) with no data 
                                   // received before closing the connection.  If you know the server
                                   // you're accessing is quick to respond, you can reduce this value.

//WEBSITE     
char buf[10];

#define INTERVAL_sensor 2000
unsigned long sensorlastTime = millis();

float tempOLED, humiOLED, lightnessOLED;

#define INTERVAL_OLED 1000

String mCottenData;

//3,传感器值的设置 
float sensor_tem, sensor_hum, sensor_lux;                    //传感器温度、湿度、光照   
char  sensor_tem_c[7], sensor_hum_c[7], sensor_lux_c[7] ;    //换成char数组传输
#include <SoftwareSerial.h>
#define EspSerial wifiSerial
#define UARTSPEED  9600
SoftwareSerial wifiSerial(2, 3); /* RX:D3, TX:D2 */
ESP8266 wifi(EspSerial);
//ESP8266 wifi(Serial1);                                      //定义一个ESP8266（wifi）的对象
unsigned long net_time1 = millis();                          //数据上传服务器时间
unsigned long sensor_time = millis();                        //传感器采样时间计时器

int SensorData;                                   //用于存储传感器数据
String postString;                                //用于存储发送数据的字符串
String jsonToSend;                                //用于存储发送的json格式参数

Tem_Hum_S2 TempMonitor;

struct config_type
{
  int EEPROM_music_num;       //歌曲的数目
  int EEPROM_music_vol;       //歌曲的音量
};

//用户自定义部分------------------------
#include "audio.h"   //"audio.h"是控制音频文件

#include "U8glib.h"
//-------字体设置，大、中、小
#define setFont_L u8g.setFont(u8g_font_7x13)
#define setFont_M u8g.setFont(u8g_font_fixed_v0r)
#define setFont_S u8g.setFont(u8g_font_fixed_v0r)
/*
font:
 u8g_font_7x13
 u8g_font_fixed_v0r
 u8g_font_chikitar
 u8g_font_osb21
 u8g_font_courB14r
 u8g_font_courB24n
 u8g_font_9x18Br
 */

//屏幕类型--------
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);

#define init_draw 500  //主界面刷新时间
unsigned long timer_draw;

int MENU_FONT = 1;  //初始化字体大小 0：小，1：中，2：大
int x=0;
boolean music_status = false; //歌曲播放状态
int music_num;
int music_vol;
int temp=0;
String dateStr, ret;
boolean key, key_cache;         //定义mp3的开关
unsigned long music_vol_time_cache = 0;
unsigned long music_vol_time = 0;
boolean music_vol_sta = false;

int uiStep()         //切歌
{
  if (analogRead(A0) < 100)  //按下
  {
    delay(50);
    if (analogRead(A0) < 100) return 1; 
  }
  if (analogRead(A0) > 800 && analogRead(A0) < 900)    //向右拨动
  {
    delay(50);
    if (analogRead(A0) > 800 && analogRead(A0) < 900) return 2;
  }
  if (analogRead(A0) > 500 && analogRead(A0) < 600)    //向左拨动
  {
    delay(50);
    if (analogRead(A0) > 500 && analogRead(A0) < 600) return 3;
  }
  if (analogRead(A0) > 600 && analogRead(A0) < 800)    //向上拨动
  {
    delay(50);
    if (analogRead(A0) > 600 && analogRead(A0) < 800) return 4;
  }
  if (analogRead(A0) > 300 && analogRead(A0) < 400)    //向下拨动
  {
    delay(50);
    if (analogRead(A0) > 300 && analogRead(A0) < 400) return 5;
  }
  return 0;
}

void networkHandle() {
  //do something with net work ,include handle response message.
  wifi.createTCP(HOST_NAME, HOST_PORT); 
  wifi.sendFromFlash(GETDATA, sizeof(GETDATA));
  uint8_t buffer[415]={0};
  uint32_t len = wifi.recv(buffer, sizeof(buffer), 2000);
  Serial.println(len);
  /*if (len > 0) {
    for (uint32_t i = 0; i < len; i++) {
      //Serial.println((char)buffer[i]);
    }
  }
  //the ram of the device is too limited,so i enhered the length of response message,at specific index,there are nessicity value.
//ಥ_ಥ They are json,but i handle them as string......
//  272，273 vol
//344 id
//414 status*/
  music_vol=((int)buffer[272]-48)*10+((int)buffer[273]-48)-10;
  //audio_vol(music_vol);
  music_num=(int)buffer[344]-47;
  if(music_num==10) music_num=1;
  if(music_num==-2) music_num=9;
  //audio_choose(music_num);
  //audio_choose(music_num);
  //temp=music_num;
  music_status=(int)buffer[414]-48;
  //if(music_status==0) audio_pause();
  //else audio_play();
  ///Serial.println(music_vol);
  ///Serial.println(music_num);
  ///Serial.println(music_status);  
  
  //free(buffer);
  //wifi.releaseTCP();
}

void setup()        //创建无返回值函数
{
    //初始化串口波特率  
    Wire.begin();
    Serial.begin(115200);
    while (!Serial); // wait for Leonardo enumeration, others continue immediately
    Serial.print(F("setup begin\r\n"));
    delay(100);
    pinMode(sensorPin_1, INPUT);

//  WifiInit(EspSerial, UARTSPEED);

  Serial.print(F("FW Version:"));
  Serial.println(wifi.getVersion().c_str());

  if (wifi.setOprToStationSoftAP()) {
    Serial.print(F("to station + softap ok\r\n"));
  } else {
    Serial.print(F("to station + softap err\r\n"));
  }

  if (wifi.joinAP(SSID, PASSWORD)) {
    Serial.print(F("Join AP success\r\n"));

    Serial.print(F("IP:"));
    Serial.println( wifi.getLocalIP().c_str());
  } else {
    Serial.print(F("Join AP failure\r\n"));
  }

  if (wifi.disableMUX()) {
    Serial.print(F("single ok\r\n"));
  } else {
    Serial.print(F("single err\r\n"));
  }
  Serial.print(F("setup end\r\n"));
  //Serial.begin(9600);    //初始化串口通信，并将波特率设置为9600
  //pinMode(A0, INPUT);
  //pinMode(A1, INPUT);
  //EEPROM---------------------
  eeprom_READ();
  audio_init(DEVICE_Flash,MODE_loopOne,music_vol);    //初始化mp3模块
  // u8g.setRot180();  // rotate screen, if required
  music_num=1;
  music_vol=20;
  //wifi.createTCP(HOST_NAME, HOST_PORT);
}

void loop()            //无返回值Loop函数
{
for(int z=0;;z++){
  int vol = uiStep(); //检测输入动作
    Serial.print("0:");
    Serial.print(analogRead(A0));
    Serial.print("  1:");
    Serial.println(analogRead(A1));
    Serial.print("          return:");
    Serial.println(vol);
  if (vol == 1) key = true;     //按一次开关,为动作1
  else key = false;             //否则不做为动作1
  if (!key && key_cache)    //按下松开后
  {
    key_cache = key;    //缓存作判断用
    music_status = !music_status; //播放或暂停
    if (music_status == true) //播放
    {
      Serial.println("play");   //串口输出 “play”（工作
      //      audio_choose(1);
      audio_play();              //音频工作
    }
    else  //暂停
    {
      Serial.println("pause");   //串口输出 “pause”（暂停）
      audio_pause();              //音频暂停工作
    }
  }
  else
  {
    key_cache = key;     //缓存作判断用
  }

  if (vol == 0)                   //如果不对开关操作为动作0
  {
    //    Serial.println("no");
    music_vol_time_cache = millis();
    music_vol_time = music_vol_time_cache;
    music_vol_sta = false; 
  }
  else if (vol == 2)              //向右拨动开关为动作2
  {
    music_vol_time = millis() - music_vol_time_cache;
    delay(500);
    if (uiStep() == 0 && !music_vol_sta)
    {
      music_vol_sta = true;        //音量工作
      music_vol++;                 //音量+1
      if (music_vol > 30) music_vol = 30; //若音量大于30，则音量为30
      audio_vol(music_vol);
      Serial.println("vol+1"); 
      delay(100);  
      eeprom_WRITE();
    }
  }
  else if (vol == 3)      //向左拨动开关为动作3
  {
    music_vol_time = millis() - music_vol_time_cache;
    delay(500);
    if (uiStep() == 0 && !music_vol_sta)
    {
      music_vol_sta = true;          //音频工作
      music_vol--;                   //音量减1
      if (music_vol < 1) music_vol = 1; //如果音量小于1，音量为1
      audio_vol(music_vol);
      Serial.println("vol-1"); 
      delay(100);
      eeprom_WRITE();
    }
  }
  else if (vol == 4)              //向上拨动开关为动作4
  {
    music_vol_time = millis() - music_vol_time_cache;
    delay(500);
    if (uiStep() == 0 && !music_vol_sta)
    {
      Serial.println("next");
      music_num++;
      if (music_num > music_num_MAX) music_num = 1;
      audio_choose(music_num);
      audio_play();
      music_status = true;       //音频状态为工作
      eeprom_WRITE();
    }
  }
  else if (vol == 5)      //向下拨动开关为动作5
  {
    music_vol_time = millis() - music_vol_time_cache;
    delay(500);
    if (uiStep() == 0 && !music_vol_sta)
    {
      Serial.println("perv");
      music_num--;
      if (music_num < 1) music_num = music_num_MAX;
      audio_choose(music_num);       //音频选择歌曲序号
      audio_play();                  //音频工作
      music_status = true;           //音频工作
      eeprom_WRITE();
    }
  }
  u8g.firstPage();
  do {
    draw();
  }
  while ( u8g.nextPage() );
  timer_draw = millis();
  if(z%10==0) 
  {networkHandle();
  audio_vol(music_vol);
  audio_choose(music_num);
  if(temp!=music_num) audio_choose(music_num);
  temp=music_num;
  if(music_status==0) audio_pause();
  else audio_play();
  Serial.println(music_vol);
  Serial.println(music_num);
  Serial.println(music_status);  
  }
  x++;
  Serial.println(x);
}}

void eeprom_WRITE()
{
  config_type config;     // 定义结构变量config，并定义config的内容
  config.EEPROM_music_num = music_num;
  config.EEPROM_music_vol = music_vol;

  EEPROM_write(0, config);  // 变量config存储到EEPROM，地址0写入
}

void eeprom_READ()
{
  config_type config_readback;
  EEPROM_read(0, config_readback);
  music_num = config_readback.EEPROM_music_num;
  music_vol = config_readback.EEPROM_music_vol;
}

//主界面，可自由定义
void draw()
{
  setFont_L;
  u8g.setPrintPos(4, 16);
  u8g.print("Music_sta:");
  u8g.print(music_status ? "play" : "pause");
  u8g.setPrintPos(4, 16 * 2);
  u8g.print("Music_vol:");
  u8g.print(music_vol);
  u8g.print("/30");
  u8g.setPrintPos(4, 16 * 3);
  u8g.print("Music_num:");
  u8g.print(music_num);
  u8g.print("/");
  u8g.print(music_num_MAX);
  u8g.setPrintPos(4, 16 * 4);
  u8g.print("C422  LOOP:");
  u8g.print(x);
  //u8g.print(rtc.formatTime(RTCC_TIME_HMS));
}
