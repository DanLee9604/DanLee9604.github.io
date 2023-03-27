#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <pm2008_i2c.h>
#include <SPI.h>
#include <SD.h>
#include "Arduino.h"

#define RXPIN 6
#define TXPIN 5
#define GPSBAUD 9600

File myFile;
TinyGPS gps;
SoftwareSerial uart_gps(RXPIN,TXPIN);
PM2008_I2C pm2008_i2c;

void getgps(TinyGPS &gps);

char                 databuffer[35];
double               temp;

void getBuffer()                                                                    //Get weather status data
{
  int index;
  for (index = 0;index < 35;index ++)
  {
    if(Serial.available())
    {
      databuffer[index] = Serial.read();
      if (databuffer[0] != 'c')
      {
        index = -1;
      }
    }
    else
    {
      index --;
    }
  }
}

int transCharToInt(char *_buffer,int _start,int _stop)                               //char to int）
{
  int _index;
  int result = 0;
  int num = _stop - _start + 1;
  int _temp[num];
  for (_index = _start;_index <= _stop;_index ++)
  {
    _temp[_index - _start] = _buffer[_index] - '0';
    result = 10*result + _temp[_index - _start];
  }
  return result;
}

int WindDirection()                                                                  //Wind Direction
{
  return transCharToInt(databuffer,1,3);
}

float WindSpeedAverage()                                                             //air Speed (1 minute)
{
  temp = 0.44704 * transCharToInt(databuffer,5,7);
  return temp;
}

float WindSpeedMax()                                                                 //Max air speed (5 minutes)
{
  temp = 0.44704 * transCharToInt(databuffer,9,11);
  return temp;
}

float Temperature()                                                                  //Temperature ("C")
{
  temp = (transCharToInt(databuffer,13,15) - 32.00) * 5.00 / 9.00;
  return temp;
}

float RainfallOneHour()                                                              //Rainfall (1 hour)
{
  temp = transCharToInt(databuffer,17,19) * 25.40 * 0.01;
  return temp;
}

float RainfallOneDay()                                                               //Rainfall (24 hours)
{
  temp = transCharToInt(databuffer,21,23) * 25.40 * 0.01;
  return temp;
}

int Humidity()                                                                       //Humidity
{
  return transCharToInt(databuffer,25,26);
}

float BarPressure()                                                                  //Barometric Pressure
{
  temp = transCharToInt(databuffer,28,32);
  return temp / 10.00;
}


void setup()
{
  pm2008_i2c.begin();
  Serial.begin(9600);
  uart_gps.begin(GPSBAUD);
  pm2008_i2c.command();
  while (!Serial) {
; // wait for serial port to connect. Needed for native USB port only
}

  Serial.println("");
  Serial.println(F("PM Overall Sensor Working"));
  Serial.println(F("      Setting Up...    "));
  Serial.println("");
  Serial.print(F("Initializing SD card..."));

  if(!SD.begin(10))
  {
    Serial.println(F("Initialization failed!"));
    return;
  }
  Serial.println(F("initialization done"));
  myFile = SD.open("wsgptest.txt", FILE_WRITE);
  Serial.println(F("Making Headers..."));
  if (myFile){
    myFile.print("Time, LAT, LNG, ALT, CRS, SPD, TMP, HMD, WDR, WSA, WSM, TMP, RFH, RFD, HMD, BPR, G1.0, G2.5, G10, T1.0, T2.5, T10, 0.3, 0.5, 1, 2.5, 5, 10");
    myFile.close();
  
  }
  Serial.println(F("Finished Setting Up"));
}

void loop()
{
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
  float latitude, longitude;
  uint8_t ret = pm2008_i2c.read();
  
  gps.f_get_position(&latitude, &longitude);
  
  int year;
  byte month, day, hour, minute, second, hundredths;
  gps.crack_datetime(&year,&month,&day,&hour,&minute,&second,&hundredths);

  Serial.print(latitude,5);
  Serial.print(",");
  Serial.print(longitude,5);
  Serial.print(",");
  Serial.print(" ");
  Serial.print(gps.f_altitude(),0); Serial.print(", ");
  Serial.print(gps.f_course(),2); Serial.print(", ");
  Serial.print(gps.f_speed_kmph(),1); Serial.print(", "); 
  
  Serial.print(" "); Serial.print(month, DEC); Serial.print(".");
  Serial.print(day,DEC); Serial.print("."); Serial.print(year);
  Serial.print("."); Serial.print(hour+9,DEC); Serial.print(".");
  Serial.print(minute,DEC); Serial.print("."); Serial.print(second, DEC);
  Serial.print(",");
  
  Serial.print(Temperature());
  Serial.print(",");
  Serial.print(Humidity());
  Serial.print(", ");
  Serial.print(BarPressure());
  Serial.print(", ");
  Serial.print(", ");
  if (ret ==0)
  {
    Serial.print(F("GRI 1.0 2.5 10")); Serial.print(F(" TSI 1.0 2.5 10")); Serial.println(F(" Numb 0.3 0.5 1 2.5 5 10"));
    Serial.print(pm2008_i2c.pm1p0_grimm);   //GRIMM 1.0 측정결과 출력
    Serial.print(", ");
    Serial.print(pm2008_i2c.pm2p5_grimm);   //GRIMM 2.5 측정결과 출력
    Serial.print(", ");
    Serial.print(pm2008_i2c.pm10_grimm);    //GRIMM 10 측정결과 출력
    Serial.print(", ");
    Serial.print(pm2008_i2c.pm1p0_tsi);     //TSI 1.0 측정결과 출력
    Serial.print(", ");
    Serial.print(pm2008_i2c.pm2p5_tsi);     //TSI 2.5 측정결과 출력
    Serial.print(", ");
    Serial.print(pm2008_i2c.pm10_tsi);      //TSI 10 측정결과 출력
    Serial.print(", ");
    Serial.print(pm2008_i2c.number_of_0p3_um);   //0.3um수 출력
    Serial.print(", ");
    Serial.print(pm2008_i2c.number_of_0p5_um);   //0.5um수 출력
    Serial.print(", ");
    Serial.print(pm2008_i2c.number_of_1_um);     //1um수 출력
    Serial.print(", ");
    Serial.print(pm2008_i2c.number_of_2p5_um);   //2.5um수 출력
    Serial.print(", ");
    Serial.print(pm2008_i2c.number_of_5_um);     //5um수 출력
    Serial.print(", ");
    Serial.print(pm2008_i2c.number_of_10_um);    //10um수 출력
    Serial.println(", ");
  }

  
  unsigned long chars;
  unsigned short sentences, failed_checksum;
  gps.stats(&chars, &sentences, &failed_checksum);
  
  writegps(latitude, longitude);
  
  delay(10000);  
}

void writegps(float lati, float longi)
{
  uint8_t ret = pm2008_i2c.read();

  int year;
  byte month, day, hour, minute, second, hundredths;
  gps.crack_datetime(&year,&month,&day,&hour,&minute,&second,&hundredths);
  myFile = SD.open("wsgptest.txt", FILE_WRITE);
 
  if (myFile) {
    Serial.print(F("Writing to file.txt..."));
    myFile.println("");

    myFile.print(" "); myFile.print(year); myFile.print(".");
    if (month <10)
    {
      myFile.print("0"); myFile.print(month,DEC);
    }
    else
    {
      myFile.print(month,DEC);
    }
    myFile.print("."); 
    if (hour+9 >= 24)
    {
      
      if(day+1 <10)
      {
        myFile.print("0");
        myFile.print(day+1,DEC);
      }
      else
      {
      myFile.print(day+1,DEC); 
      }
      myFile.print("."); 
      
      if (hour+9-24 < 10)
      {
        myFile.print("0");
        
      }
      myFile.print(hour+9-24,DEC); myFile.print(".");
    }
    else
    {
    myFile.print(day,DEC); myFile.print(".");
      if (hour+9 < 10)
      {
        myFile.print("0");
      }
    myFile.print(hour+9,DEC); myFile.print(".");
    }
    
    if (minute < 10)
    {
      myFile.print("0");
    }
    myFile.print(minute,DEC); myFile.print("."); 
    if (second < 10)
    {
      myFile.print("0");
    }
    
    myFile.print(second, DEC);
    myFile.print(", ");
    myFile.print(lati,5);
    myFile.print(",");
    myFile.print(longi,5);
    myFile.print(",");
    myFile.print(" ");
    myFile.print(gps.f_altitude(),0); myFile.print(", ");
    myFile.print(gps.f_course(),2); myFile.print(", ");
    myFile.print(gps.f_speed_kmph(),1); myFile.print(", "); 
   
    myFile.print(Temperature());
    myFile.print(", ");
    myFile.print(Humidity());
    myFile.print(", ");
    myFile.print(WindDirection());
    myFile.print(", ");
    myFile.print(WindSpeedAverage());
    myFile.print(", ");
    myFile.print(WindSpeedMax());
    myFile.print(", ");
    myFile.print(RainfallOneHour());
    myFile.print(", ");
    myFile.print(RainfallOneDay());
    myFile.print(", ");
    myFile.print(BarPressure());
    myFile.print(", ");

if (ret ==0)
  {
    myFile.print(pm2008_i2c.pm1p0_grimm);   //GRIMM 1.0 측정결과 출력
    myFile.print(", ");
    myFile.print(pm2008_i2c.pm2p5_grimm);   //GRIMM 2.5 측정결과 출력
    myFile.print(", ");
    myFile.print(pm2008_i2c.pm10_grimm);    //GRIMM 10 측정결과 출력
    myFile.print(", ");
    myFile.print(pm2008_i2c.pm1p0_tsi);     //TSI 1.0 측정결과 출력
    myFile.print(", ");
    myFile.print(pm2008_i2c.pm2p5_tsi);     //TSI 2.5 측정결과 출력
    myFile.print(", ");
    myFile.print(pm2008_i2c.pm10_tsi);      //TSI 10 측정결과 출력
    myFile.print(", ");
    myFile.print(pm2008_i2c.number_of_0p3_um);   //0.3um수 출력
    myFile.print(", ");
    myFile.print(pm2008_i2c.number_of_0p5_um);   //0.5um수 출력
    myFile.print(", ");
    myFile.print(pm2008_i2c.number_of_1_um);     //1um수 출력
    myFile.print(", ");
    myFile.print(pm2008_i2c.number_of_2p5_um);   //2.5um수 출력
    myFile.print(", ");
    myFile.print(pm2008_i2c.number_of_5_um);     //5um수 출력
    myFile.print(", ");
    myFile.print(pm2008_i2c.number_of_10_um);    //10um수 출력
    myFile.print(", ");
  }
    
   
    myFile.close();
    Serial.println(F("done."));
  } 
  else 
  {
    Serial.println(F("error opening file"));
  }
  
}
