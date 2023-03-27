#include <CO2Sensor.h>      //MG811 CO2센서 라이브러리 포함
#include <Wire.h>           //BH1750 조도센서 라이브러리 포함
#include <BH1750FVI.h>
#include <DHT.h>            //DHT22 라이브러리 포함

#define esp8266 Serial3   //esp8266선을 RX,TX3으로 연결
#define BAUDRATE 9600

// 핀 설정 및 센서 기본 변수 설정
#define DEBUG   true        //esp 설정
#define DHTPIN A0           //DHT22 온습도 센서 핀 설정
#define DHTTYPE DHT22
#define MG_PIN A1           //MG811 CO2센서 핀 설정
#define VltS A2


//라이브러리별 설정
DHT dht(DHTPIN, DHTTYPE);
BH1750FVI::eDeviceMode_t DEVICEMODE = BH1750FVI::k_DevModeContHighRes;
BH1750FVI LightSensor(DEVICEMODE);
CO2Sensor co2Sensor(MG_PIN, 0.99, 100);//핀 내부계수 측정정도 지정해주는 클래스
//전역변수 설정
float hum;
float temp;
float gas;
float volts;
float vout = 0.0;
float vin = 0.0;
float R1 = 30000.0;
float R2 = 7500.0;

int Vval = 0;
int countTrueCommand;                   //ESP8266 커맨드 카운트 함수
int countTimeCommand;
boolean found = false;

//String형태로 입출력 변수 설정
String AP =     "Ahn";        //Wifi설정
String PASS =   "0000001002";
String WApiKey = "CU8634SH66Q1MLO9";    //ThingSpeak 내 Write API
String RApiKey = "XMP6OYHXR8RYX55A";    //ThingSpeak 내 Read API
String HOST = "api.thingspeak.com";
String PORT = "80";
String fieldTemp = "field1";            //Air Temperature 필드1 설정(최대 8)
String fieldHum  = "field2";            //Air humidity 필드2 설정
String fieldLight = "field3";           //Light Intensity 필드3 설정
String fieldCO2 = "field4";             //CO2 센서 필드 4 설정
String fieldVolt = "field5";

void setup()
{
  unsigned int i = 0;
  // Open Serial1 communications and wait for port to open:
  // 시리얼 1 통신 시작 및 열릴때까지 대기
  esp8266.begin(BAUDRATE);            //ESP8266 보드레이트 설정
  esp8266.setTimeout(5000);
  dht.begin();                        //온습도 시작
  LightSensor.begin();                //조도 센서 시작
  co2Sensor.calibrate();
  pinMode(VltS, INPUT);
  Serial.begin(9600);                 //시리얼 통신 시작
  Serial.println("Thingspeak with ESP-8266");
  //ESPWiFi 셋팅
  sendCommand("AT", 5, "OK");
  sendCommand("AT+CWMODE=1", 5, "OK");
  sendCommand("AT+CWJAP=\"" + AP + "\",\"" + PASS + "\"", 20, "OK");

  //IP획득 및 표시
  sendCommand("AT+CIFSR", 2, "OK");
  delay(2000);
}

//반복 조건
void loop()
{
  hum = dht.readHumidity();                             //습도 읽는 함수
  temp = dht.readTemperature();                         //온도 읽는 함수
  uint16_t lux = LightSensor.GetLightIntensity();       //조도 읽는 함수
  Vval = analogRead(VltS);
  vout = (Vval * 5.0) / 1024.0;
  vin = vout / (R2 / (R1 + R2));                        //전압 센서
  gas = co2Sensor.read();                               //CO2
  
  //시리얼 표시
  Serial.print("Humidity: ");
  Serial.print(hum, 1);           //습도 소수점 첫째 표시
  Serial.print(" %, Temp: ");
  Serial.print(temp, 1);          //온도 소수점 첫째 표시
  Serial.print(" Celsius, ");
  Serial.print(lux);              //조도 표시
  Serial.print(" lux, ");
  Serial.print(vin);            //배터리 볼트 표시
  Serial.print("V, ");
  Serial.print(gas); 
  Serial.println( "ppm " );
  
  //Thing Speak 서버에 업로드
  sendCommand("AT+CIPMUX=1", 5, "OK");
  String getData = "GET /update?api_key=" + WApiKey + "&" + fieldTemp + "=" + String(temp) + "&" + fieldHum + "=" + String(hum) + "&" + fieldLight + "=" + String(lux) + "&" + fieldCO2 + "=" + String(gas) + "&" + fieldVolt + "=" + String(vin);
  sendCommand("AT+CIPSTART=0,\"TCP\",\"" + HOST + "\"," + PORT, 15, "OK");
  sendCommand("AT+CIPSEND=0," + String(getData.length() + 4), 4, ">");
  esp8266.println(getData); countTrueCommand++;
  sendCommand("AT+CIPCLOSE=0", 5, "OK");
}

// ESP 커맨드 입력 및 전송상태 수신
void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while (countTimeCommand < (maxTime * 1))
  {
    esp8266.println(command);//at+cipsend
    if (esp8266.find(readReplay)) //ok
    {
      found = true;
      break;
    }

    countTimeCommand++;
  }

  if (found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }

  if (found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }

  found = false;
}
