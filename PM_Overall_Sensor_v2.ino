#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include "DHT.h"
#include <pm2008_i2c.h>
#include <SPI.h>
#include <SD.h>

#define DHTPIN 7
#define RXPIN 6
#define TXPIN 5
#define GPSBAUD 9600
#define DHTTYPE DHT11

File myFile;
TinyGPS gps;
SoftwareSerial uart_gps(RXPIN,TXPIN);
DHT dht(DHTPIN, DHTTYPE);
PM2008_I2C pm2008_i2c;


void getgps(TinyGPS &gps);

void setup()
{
  pm2008_i2c.begin();
  Serial.begin(9600);
  uart_gps.begin(GPSBAUD);
  dht.begin();
  pm2008_i2c.command();
  

  Serial.println("");
  Serial.println(F("PM Overall Sensor Working"));
  Serial.println(F("      waiting for lock..    "));
  Serial.println("");

  Serial.print(F("Initializing SD card..."));

  if(!SD.begin(4))
  {
    Serial.println(F("Initialization failed!"));
    return;
  }
  Serial.println(F("initialization done"));
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
  int year;
  byte month, day, hour, minute, second, hundredths;
  gps.crack_datetime(&year,&month,&day,&hour,&minute,&second,&hundredths);

}

void getgps(TinyGPS &gps)
{
  float latitude, longitude;
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  uint8_t ret = pm2008_i2c.read();
  
  gps.f_get_position(&latitude, &longitude);
  
  if (isnan(h) || isnan(t) || isnan(f))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);

  
  Serial.print(latitude,5);
  Serial.print(",");
  Serial.print(longitude,5);
  Serial.print(",");
  Serial.print(" ");
  Serial.print(gps.f_altitude(),0); Serial.print(", ");
  Serial.print(gps.f_course(),2); Serial.print(", ");
  Serial.print(gps.f_speed_kmph(),1); Serial.print(", "); 
  
  checktime();
  
  Serial.print(h);
  Serial.print(",");
  Serial.print(hic);
  Serial.print(", ");
  Serial.print(hif);
  Serial.print(", ");
  Serial.println();

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
  
  delay(5000);  
}
void checktime()
{
  int year;
  byte month, day, hour, minute, second, hundredths;
  gps.crack_datetime(&year,&month,&day,&hour,&minute,&second,&hundredths);

  Serial.print(" "); Serial.print(month, DEC); Serial.print(".");
  Serial.print(day,DEC); Serial.print("."); Serial.print(year);
  Serial.print("."); Serial.print(hour+9,DEC); Serial.print(".");
  Serial.print(minute,DEC); Serial.print("."); Serial.print(second, DEC);
  Serial.print(",");
}

void writegps(float lati, float longi)
{
  int year;
  byte month, day, hour, minute, second, hundredths;
  gps.crack_datetime(&year,&month,&day,&hour,&minute,&second,&hundredths);

  Serial.print(F("lati/longi: ")); 
  Serial.print(lati,5); 
  Serial.print(", "); 
  Serial.println(longi,5);
  Serial.println();
 
  myFile = SD.open("gpstest1.txt", FILE_WRITE);
 
  if (myFile) {
    Serial.print(F("Writing to gpstest1.txt..."));
    myFile.print("      <trkpt lat=\"");
    myFile.print(lati,5);
    myFile.print("\" lon=\"");
    myFile.print(longi,5);
    myFile.println("\">");
    myFile.print("        <ele>");
    myFile.print(gps.f_altitude(),0);
    myFile.println("</ele>");
    myFile.print("        <time>");
    myFile.print(year); myFile.print("-"); myFile.print(month,DEC); myFile.print("-"); myFile.print(day,DEC); myFile.print("T");
    myFile.print(hour+9,DEC); myFile.print(":"); myFile.print(minute,DEC); myFile.print(":"); myFile(second,DEC);
    myFile.println("</time>");
    myFile.println
    myFile.println("      </trkpt>");
    myFile.close();
    Serial.println(F("done."));
  } else {
    Serial.println(F("error opening gpstest1.txt"));
  }
}
