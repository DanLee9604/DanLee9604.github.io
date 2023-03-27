#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include "DHT.h"
#include <FastLED.h>

#define NUM_LEDS 4
#define RGB_PIN 4
#define DHTPIN 7
#define RXPIN 6
#define TXPIN 5
#define GPSBAUD 9600
#define DHTTYPE DHT11

/*Harry Potter BGM tone setting*/
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST 0

int tempo = 144;

int buzzer = 8;
int gLP = 1;
int bLP = 2;
int rLP = 3;
const int gP = A0;
int lP = A1;
int aV = 0;

CRGB leds [NUM_LEDS];
TinyGPS gps;
SoftwareSerial uart_gps(RXPIN,TXPIN);
DHT dht(DHTPIN, DHTTYPE);

void getgps(TinyGPS &gps);

int melody[] = {


  // Hedwig's theme fromn the Harry Potter Movies
  // Socre from https://musescore.com/user/3811306/scores/4906610
  
  REST, 2, NOTE_D4, 4,
  NOTE_G4, -4, NOTE_AS4, 8, NOTE_A4, 4,
  NOTE_G4, 2, NOTE_D5, 4,
  NOTE_C5, -2, 
  NOTE_A4, -2,
  NOTE_G4, -4, NOTE_AS4, 8, NOTE_A4, 4,
  NOTE_F4, 2, NOTE_GS4, 4,
  NOTE_D4, -1, 
  NOTE_D4, 4,

  NOTE_G4, -4, NOTE_AS4, 8, NOTE_A4, 4, //10
  NOTE_G4, 2, NOTE_D5, 4,
  NOTE_F5, 2, NOTE_E5, 4,
  NOTE_DS5, 2, NOTE_B4, 4,
  NOTE_DS5, -4, NOTE_D5, 8, NOTE_CS5, 4,
  NOTE_CS4, 2, NOTE_B4, 4,
  NOTE_G4, -1,
  NOTE_AS4, 4,
     
  NOTE_D5, 2, NOTE_AS4, 4,//18
  NOTE_D5, 2, NOTE_AS4, 4,
  NOTE_DS5, 2, NOTE_D5, 4,
  NOTE_CS5, 2, NOTE_A4, 4,
  NOTE_AS4, -4, NOTE_D5, 8, NOTE_CS5, 4,
  NOTE_CS4, 2, NOTE_D4, 4,
  NOTE_D5, -1, 
  REST,4, NOTE_AS4,4,  

  NOTE_D5, 2, NOTE_AS4, 4,//26
  NOTE_D5, 2, NOTE_AS4, 4,
  NOTE_F5, 2, NOTE_E5, 4,
  NOTE_DS5, 2, NOTE_B4, 4,
  NOTE_DS5, -4, NOTE_D5, 8, NOTE_CS5, 4,
  NOTE_CS4, 2, NOTE_AS4, 4,
  NOTE_G4, -1, 
  
};

// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
// there are two values per note (pitch and duration), so for each note there are four bytes
int notes = sizeof(melody) / sizeof(melody[0]) / 2;

// this calculates the duration of a whole note in ms (60s/tempo)*4 beats
int wholenote = (60000 * 4) / tempo;

int divider = 0, noteDuration = 0;

void setup()
{
  pinMode(gLP,OUTPUT);
  pinMode(bLP,OUTPUT);
  pinMode(rLP,OUTPUT);
  FastLED.addLeds<WS2812, RGB_PIN, GRB>(leds, NUM_LEDS);
  Serial.begin(9600);
  uart_gps.begin(GPSBAUD);
  dht.begin();

  Serial.println("");
  Serial.println("GPS Shield QuickStart Example Sketch v12");
  Serial.println("      waiting for lock..    ");
  Serial.println("");
}

void loop()
{
  aV = analogRead(lP);
  if(aV < 50)
  {            
    digitalWrite(rLP, HIGH);
  }
  else if(aV >= 50 && aV <= 100)
  {
    digitalWrite(bLP, HIGH);
  }
  else{
    digitalWrite(gLP, HIGH);
  }
  delay(200);
  digitalWrite(gLP, LOW);
  digitalWrite(bLP, LOW);
  digitalWrite(rLP, LOW);
   
  float h = dht.readHumidity();
 if(h>=80)
 {
   for(int dot = 0; dot < NUM_LEDS; dot++) 
   { 
            leds[dot] = CRGB::Purple;
            FastLED.show();
            // clear this led for the next time around the loop
            leds[dot] = CRGB::Black;
            delay(30);
   }
    // iterate over the notes of the melody. 
  // Remember, the array is twice the number of notes (notes + durations)
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) 
  {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) 
    {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) 
    {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(buzzer, melody[thisNote], noteDuration*0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);
    
    // stop the waveform generation before the next note.
    noTone(buzzer);
  }
 }
  
  while(uart_gps.available())
  {
    int c = uart_gps.read();
    if(gps.encode(c))
    {
      getgps(gps);
    }
  }
}

void getgps(TinyGPS &gps)
{
  aV = analogRead(lP);
  float latitude, longitude;
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  gps.f_get_position(&latitude, &longitude);
  
  if (isnan(h) || isnan(t) || isnan(f))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.println(h);
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.println(" *F\t ");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F ");
  Serial.print("Gas : ");
  Serial.println(analogRead(gP));
  Serial.print("Light : ");
  Serial.println(lP);
  Serial.print("Lat/Long: ");
  Serial.print(latitude,5);
  Serial.print(",");
  Serial.println(longitude,5);
  

  int year;
  byte month, day, hour, minute, second, hundredths;
  gps.crack_datetime(&year,&month,&day,&hour,&minute,&second,&hundredths);

  Serial.print("Date: "); Serial.print(month, DEC); Serial.print("/");
  Serial.print(day,DEC); Serial.print("/"); Serial.print(year);
  Serial.print("  Time: "); Serial.print(hour,DEC); Serial.print(":");
  Serial.print(minute,DEC); Serial.print(":"); Serial.print(second, DEC);
  Serial.print("."); Serial.println(hundredths,DEC);

  Serial.print("Altitude (meters): "); Serial.println(gps.f_altitude());
  Serial.print("Course (degrees): "); Serial.println(gps.f_course());
  Serial.print("Speed(kmph): "); Serial.println(gps.f_speed_kmph());
  Serial.println();

  unsigned long chars;
  unsigned short sentences, failed_checksum;
  gps.stats(&chars, &sentences, &failed_checksum);
  delay(1000);  
}
