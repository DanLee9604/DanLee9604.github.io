#include <NewPing.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

 LiquidCrystal_I2C lcd(0x27, 16, 2);
  
int TRIGGER_PIN = 10  ;
int ECHO_PIN    = 11  ;
int MAX_DISTANCE = 200 ;
   
int LAZER_L = 3;
int LAZER_R = 4;
int DETECT_L = A5;
int DETECT_R = A4;

int LED1_R = 50;
int LED1_G = 51;
int LED1_B = 49;

int LED2_R = 35;
int LED2_G = 37;
int LED2_B = 39;

int LED3_R = 44;
int LED3_G = 43;
int LED3_B = 45;

int LED4_R = 41;
int LED4_G = 40;
int LED4_B = 42;

  
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); 
 
void setup() {
  Serial.begin(9600);
  pinMode(LED1_R, OUTPUT);
  pinMode(LED1_G, OUTPUT);
  pinMode(LED1_B, OUTPUT);
  
  pinMode(LED2_R, OUTPUT);
  pinMode(LED2_G, OUTPUT);
  pinMode(LED2_B, OUTPUT);
  
  pinMode(LED3_R, OUTPUT);
  pinMode(LED3_G, OUTPUT);
  pinMode(LED3_B, OUTPUT);
  
  pinMode(LED4_R, OUTPUT);
  pinMode(LED4_G, OUTPUT);
  pinMode(LED4_B, OUTPUT);
  
  pinMode(LAZER_L, OUTPUT);
  pinMode(LAZER_R, OUTPUT);
  
  pinMode(DETECT_L, INPUT);
  pinMode(DETECT_R, INPUT);

  lcd.init();
  lcd.backlight();
  
  digitalWrite(LAZER_L, HIGH);
  digitalWrite(LAZER_R, HIGH);
}
void loop() {
  int L_detected = digitalRead(DETECT_L);
  int R_detected = digitalRead(DETECT_R);
  

  
  Serial.print("Distance : ");
  Serial.print(sonar.ping_cm());
  Serial.print("cm  ");
  Serial.print("LEFT: ");
  Serial.print(L_detected);
  Serial.print(" RIGHT: ");
  Serial.println(R_detected);

  
  if (sonar.ping_cm() < 8 ) {
    if (L_detected == HIGH && R_detected == HIGH){
   analogWrite(LED1_R, 0);
   analogWrite(LED1_G, 0);
   analogWrite(LED1_B, 0);
   analogWrite(LED2_R, 0);
   analogWrite(LED2_G, 0);
   analogWrite(LED2_B, 0);
   analogWrite(LED3_R, 0);
   analogWrite(LED3_G, 0);
   analogWrite(LED3_B, 0);
   analogWrite(LED4_R, 0);
   analogWrite(LED4_G, 0);
   analogWrite(LED4_B, 0);
  }  
  else if (L_detected == LOW && R_detected == HIGH){
   analogWrite(LED1_R, 0);
   analogWrite(LED1_G, 140);
   analogWrite(LED1_B, 0);
   analogWrite(LED2_R, 0);
   analogWrite(LED2_G, 140);
   analogWrite(LED2_B, 0);
   analogWrite(LED3_R, 140);
   analogWrite(LED3_G, 0);
   analogWrite(LED3_B, 0);
   analogWrite(LED4_R, 0); 
   analogWrite(LED4_G, 140);
   analogWrite(LED4_B, 0);
   
   lcd.setCursor(0,0);          
   lcd.print("  Check Right  "); 
  }
  else if (R_detected == LOW && L_detected==HIGH){
   analogWrite(LED1_R, 0);
   analogWrite(LED1_G, 140);
   analogWrite(LED1_B, 0);
   analogWrite(LED2_R, 140);
   analogWrite(LED2_G, 0);
   analogWrite(LED2_B, 0);
   analogWrite(LED3_R, 0);
   analogWrite(LED3_G, 140);
   analogWrite(LED3_B, 0);
   analogWrite(LED4_R, 0);
   analogWrite(LED4_G, 140);
   analogWrite(LED4_B, 0);
   
   lcd.setCursor(0,0);          
   lcd.print("  Check Left  ");
  }

    else {
   analogWrite(LED1_R, 0);
   analogWrite(LED1_G, 140);
   analogWrite(LED1_B, 0);
   analogWrite(LED2_R, 0);
   analogWrite(LED2_G, 140);
   analogWrite(LED2_B, 0);
   analogWrite(LED3_R, 0);
   analogWrite(LED3_G, 140);
   analogWrite(LED3_B, 0);
   analogWrite(LED4_R, 0);
   analogWrite(LED4_G, 140);
   analogWrite(LED4_B, 0);
   
   lcd.setCursor(0,0);          
   lcd.print("   WELL DONE   ");
   }
}
else{
  analogWrite(LED1_R, 0);
}
   analogWrite(LED1_G, 0);
   analogWrite(LED1_B, 0);
   analogWrite(LED2_R, 0);
   analogWrite(LED2_G, 0);
   analogWrite(LED2_B, 0);
   analogWrite(LED3_R, 0);
   analogWrite(LED3_G, 0);
   analogWrite(LED3_B, 0);
   analogWrite(LED4_R, 0);
   analogWrite(LED4_G, 0);
   analogWrite(LED4_B, 0);
lcd.clear() ;
  
    }
