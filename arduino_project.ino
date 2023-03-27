#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Stepper.h>
#include <Servo.h>


 
#define DHTPIN 13        // SDA 핀의 설정
#define DHTTYPE DHT22   // DHT22  센서종류 설정


int joy_x = A2;

int joy_p = A3;

int SVPin = 12;
Servo SV;

hd44780_I2Cexp lcd ; // LCD 객체 
DHT dht(DHTPIN, DHTTYPE);
 
int fan = 3;
int motor_pin = 11;
int Dust = 0;
char str1[10];    //10칸짜리 문자배열 생성
int RLED = 7;     //빨간색 led 연결
int GLED = 8;     //초록생 led 연결

//미세먼지 원리: 미세먼지에 적외선을 쏘아 돌아오는 양을 측정->전압으로 신호
int Vo = A0;      // 미세먼지 전압신호 측정 핀 연결
int V_LED = 5;

float Vo_value =0;   
float Voltage=0;
float a = 0;


int gas = A1;
int Buz = 10;

void setup(){
  pinMode(13, INPUT);
  pinMode(fan, OUTPUT);
  pinMode(joy_p , INPUT);
  
  pinMode(9, OUTPUT);


  SV.attach(SVPin);
  Serial.begin(9600); 
  
  pinMode(V_LED, OUTPUT);
  pinMode(Vo, INPUT);
  pinMode(motor_pin, OUTPUT);
  pinMode(RLED, OUTPUT);
  pinMode(GLED, OUTPUT);
  
  pinMode(gas, INPUT);
  pinMode(Buz, OUTPUT);
  
  Wire.begin();
  Serial.begin(9600); 
  lcd.begin(16, 2); // lcd 통신 사용
  dht.begin();
  
}
void loop(){
  //온습도 값 측정후 변수 저장
  float h = dht.readHumidity();
  float t = dht.readTemperature();
if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
    return;
} 
  int x = analogRead(joy_x);
 
  int p = digitalRead(joy_p);
  int angle1 = map(x, 513, 1023, 90, 180);
  int angle2 = map(x, 0, 512, 0, 90);
  Serial.print("X: ");
  Serial.print(x);

  Serial.print(" P: ");
  Serial.println(p);
  
  if (x < 400) {    
  SV.write(angle1);
  delay(500);
  }
  
  else if (x > 600) {    
  SV.write(angle2);
  delay(500);
  }
//출력  
lcd.clear (); // LCD출력내용 지우기
lcd.backlight();
lcd.setCursor (0, 0); //커서 위치(좌표) 설정
lcd.print("T:");
lcd.print(t);
lcd.setCursor (8, 0);
lcd.print("H:");
lcd.print(h);


//온도에 따른 선풍기 온오프
if (t >=  22 ) {
  digitalWrite(fan ,LOW);
}
else {
  digitalWrite(fan, HIGH);
  
}


//미세먼지 & 공기청정 모터
//280,40,9680 마이크로 세컨드설정은 센서 설명서에 나와있음
digitalWrite(V_LED, LOW);    //적외선 LED OFF
delayMicroseconds(280);      //샘플링 시간, 0.28마이크로세컨드
Vo_value = analogRead(Vo);   //적외선을 발사하지 않았을때의 값을 읽어오라는 뜻
delayMicroseconds(40);       //값을 읽고 대기시간,
digitalWrite(V_LED,HIGH);    //적외선 LED ON
delayMicroseconds(9680);     //동작 유지 시간, 9680마이크로 세컨드

Voltage = Vo_value*5.0/1023.0; //0~1023센서값을 0~5V로 변환

//센서의 특성에 기인한 코드
if( Voltage < 0.1){
  Dust = 0;
}
else if (Voltage < 0.8){
  Dust = 10*(5*(Voltage - 0.1));
}
else if (Voltage < 1){
  Dust = 30 + 5*((Voltage - 0.8)*10);
}else if (Voltage >1){
  Dust = 40 +16*((Voltage -1)*10);
}

lcd.setCursor(0, 1);
lcd.print("D:");
sprintf(str1, "%03d", Dust); //03은 두자리 숫자 앞에는 0을 붙인다는 뜻, d는 정수를 뜻함.
lcd.print(str1);


lcd.print("g/m");

//미세먼지값에 따른 LED와 공기청정 모터 제어
if(Dust > 80){
  digitalWrite(RLED,HIGH);
  digitalWrite(GLED,LOW);

}
else{
  digitalWrite(RLED,LOW);
  digitalWrite(GLED,HIGH);

}

// 가스센서 & 버져
int value = analogRead(gas);
lcd.print(" G:");
lcd.print(value);
delay(1000);
//가스값에 따른 LED와 버저 제어
if( value <= 820){

  noTone(Buz);                        
}

else{

  tone(Buz,730);
}
 delay(500);


}
