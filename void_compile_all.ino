#include <Servo.h>
#include "DHT.h"
#define DHTPIN 12
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
Servo servo;
const int ENA = 10;            // 모터1사용 가능 제어핀과 동시에 속도제어핀 역할
const int IN1 =  9;               // 방향제어를 위한 핀1
const int IN2 =  8;               // 방향제어를 위한 핀2

int buttonPin = 6;     // 푸시 버튼 핀 설정
int relay =  7;      // 릴레이 핀 설정
boolean current_button =LOW;
boolean last_button =LOW;
boolean relay_on= false;

int servoPin = 11;

int RCPin1 = 2;
int RCPin2 = 3;
int RCPin3 = 4;
int RCPin4 = 5;

int RCValue1;
int RCValue2;
int RCValue3;
int RCValue4;

void setup() { 
  Serial.begin(9600);                       // 시리얼 통신 개시
  dht.begin();
 
  servo.attach(servoPin);                   //서보모터 핀 설정
  servo.write(90);                          //서보모터 각도 초기화
  pinMode(relay, OUTPUT);                   //릴레이 출력 설정
  pinMode(buttonPin, INPUT);                // 버튼 입력 설정
  pinMode(ENA, OUTPUT);                     // 모터1사용 가능 제어핀 출력으로 설정
  pinMode(IN1, OUTPUT);                     // 방향제어를 위한 핀1 출력으로 설정
  pinMode(IN2, OUTPUT);                     // 방향제어를 위한 핀2 출력으로 설정
  pinMode(RCPin1, INPUT);
  pinMode(RCPin2, INPUT);
  pinMode(RCPin3, INPUT);
  pinMode(RCPin4, INPUT);
  digitalWrite(ENA, LOW);                 // 일단 모터가 작동하지 않도록 초기화


}

void loop() {
   
   relaycontrol();
   motor();
   dhttest();
}

void relaycontrol() {
  current_button = digitalRead(buttonPin); // 버튼의 현재 상태 수신

  if (last_button==LOW && current_button==HIGH) { //마지막 버튼 클릭이 LOW고, 현재 입력이 HIGH라면
    
    relay_on =! relay_on; //릴레이의 상태를 반전시킴
  } 
  last_button=current_button; //마지막 버튼 상태를 현재 버튼 상태로 수정
  digitalWrite(relay,relay_on); //릴레이 상태에 따라 릴레이 출력 수정
}

void dhttest(){
  float h = dht.readHumidity();
  float t = dht.readTemperature();
 
  if (isnan(t) || isnan(h)) {
    //값 읽기 실패시 시리얼 모니터 출력
    Serial.println("Failed to read from DHT");
  } else {
    //온도, 습도 표시 시리얼 모니터 출력
    Serial.print("Humidity: "); 
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.println(" *C");

   } 
}
  
void motor(){
       
  RCValue1 = pulseIn(RCPin1,HIGH);
  RCValue2 = pulseIn(RCPin2,HIGH);
  RCValue3 = pulseIn(RCPin3,HIGH);
  RCValue4 = pulseIn(RCPin4,HIGH);
  //타미야 신호값을 PWM신호 범위로 조정
   int motorForward  = map(RCValue3, 1480, 1100,  50,  255);
   int motorBackward = map(RCValue3, 1530, 1900,  50,  255);
   //최소 신호를 50으로 줘서 시그널이 적을때도 작동되게 조정
   int angleRight    = map(RCValue1, 1530, 1900,  91,  170);
   int angleLeft     = map(RCValue1, 1480, 1100,  89,   10);
  
if(RCValue1 > 1470&& RCValue1 < 1540){
  //중립시 조향 중립
  servo.write(90);
  }  
  else if(RCValue1 >=1540){
  // 우향
  servo.write(angleRight);
  } 
  else {
  // 좌향
  servo.write(angleLeft);
   }
if(RCValue3 > 1470&& RCValue3 < 1540){
  //중립시 모터 정지
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, LOW);
  }
else if(RCValue3 >=1540){
  // 후진
  analogWrite(ENA, motorBackward);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);  
  } 
  else {
  // 전진
  analogWrite(ENA, motorForward);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
   }return;
   }

   
