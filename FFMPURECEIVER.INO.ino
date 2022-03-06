#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

const uint64_t My_radio_pipeIn = 0xE8E8F0F0E1LL;     //Remember that this code is the same as in the transmitter
RF24 radio(9, 10);  //CSN and CE pins
unsigned long lastRecvTime = 0;
// The sizeof this struct should not exceed 32 bytes
struct Received_data {
  int ch1;
  int ch2;
  int ch3;
  int ch4;
  int ch5;
  int ch6;
  int ch7;
  int ch8;
};

Received_data received_data;

void setup()
{
  Serial.begin(9600);
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);  
  radio.openReadingPipe(1,My_radio_pipeIn);
  radio.startListening();

}
//**************************************************/
void receive_the_data()
{
  while ( radio.available() ) 
  {
      radio.read(&received_data, sizeof(Received_data));
      lastRecvTime = millis(); //Here we receive the data
     
      Serial.print(received_data.ch1);
      Serial.print("\t");
      Serial.print(received_data.ch2);
      Serial.print("\t");
      Serial.print(received_data.ch3);
      Serial.print("\t");
      Serial.print(received_data.ch4);
      Serial.print("\t");
      Serial.print(received_data.ch5);
      Serial.print("\t");
      Serial.print(received_data.ch6);
      Serial.print("\t");
      Serial.print(received_data.ch7);
      Serial.print("\t");
      Serial.println(received_data.ch8);
  }
}
//**************************************************/
void loop()
{
  receive_the_data();
}//Loop end
