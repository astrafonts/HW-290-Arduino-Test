#include <SoftwareSerial.h>

#include <TinyGPS.h>

/* This sample code demonstrates the normal use of a TinyGPS object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/

TinyGPS gps;
SoftwareSerial ss(5, 6);
char c;
unsigned long chars;
unsigned short sentences, failed;
bool newData;
float lat,lon;


void setup()
{
  Serial.begin(115200);
  ss.begin(9600);
  
  Serial.print("Simple TinyGPS library v. "); Serial.println(TinyGPS::library_version());
  Serial.println("by Mikal Hart");
  Serial.println();
}

void loop()
{
  newData = false;

//
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (ss.available())
    {
      c = ss.read();
      //Serial.print(c);
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }

  if (newData)
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    Serial.println("---------------------------------------------------------");
    Serial.print("LAT=");
    Serial.println(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    Serial.print(" LON=");
    Serial.println(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    Serial.print(" SAT=");
    Serial.println(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    Serial.print(" PREC=");
    Serial.println(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
    Serial.println("---------------------------------------------------------");
  }
  
//  gps.stats(&chars, &sentences, &failed);
//  Serial.println("-------------------------------");
//  Serial.print(" CHARS=");  Serial.println(chars);
//  Serial.print(" SENTENCES=");  Serial.println(sentences);  
//  Serial.print(" CSUM ERR=");  Serial.println(failed);
//  Serial.println("-------------------------------");
//  if (chars == 0)
//    Serial.println("** No characters received from GPS: check wiring **");
}
