#include <Wire.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
//#include <Adafruit_BMP085.h>
#include <SFE_BMP180.h>
#include <SimpleKalmanFilter.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>

TinyGPS gps;
SoftwareSerial mySerial(5,6); // RX, TX


//#define seaLevelPressure_hPa 1013.25
#define LED1 4
#define MPU 0X68

SimpleKalmanFilter pressureKalmanFilter(1, 1, 0.01);

const uint64_t my_radio_pipe = 0xE8E8F0F0E1LL;  //Remember that this code should be the same for the receiver

//Adafruit_BMP085 bmp;
SFE_BMP180 pressure;
RF24 radio(9, 10);  //Set CE and CSN pins

struct Data_to_be_sent {
  int ch1;
  int ch2;
  int ch3;
  int ch4;
  int ch5;
  int ch6;
  int ch7;
  int ch8;
};

Data_to_be_sent sent_data;

Servo channel_x;
Servo channel_y;

int gyro_x, gyro_y, gyro_z;
int acc_x, acc_y, acc_z;
//float presure,ps_ground;
float baseline; // baseline pressure

long gyro_x_cal, gyro_y_cal, gyro_z_cal;

float accAngleX, accAngleY, gyroAngleX, gyroAngleY;
float angleX, angleY;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY;
float elapsedTime, currentTime, previousTime;



void setup() {
  pinMode(LED1, OUTPUT); 
  digitalWrite(LED1, HIGH);
  channel_x.attach(5);
  channel_y.attach(6);
  mySerial.begin(9600);
  Serial.begin(9600);
  Wire.begin();
  
  setup_mpu_6050_registers();
 // calculate_IMU_error();
  

  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(my_radio_pipe);

   if (!pressure.begin()) {
      Serial.println("BMP180 Pressure Sensor Error!");
      while(1); // Pause forever.
    }
    baseline = getPressure();

   digitalWrite(LED1, LOW);
  
}

void loop(){
  unsigned long chars;
  unsigned short sentences, failed;
 
  bool newData = false;
  read_mpu_6050_data();                                                //Read the raw acc and gyro data from the MPU-6050
  int p = getPressure();
  int altitude = pressure.altitude(p,baseline);
  int estimated_altitude = pressureKalmanFilter.updateEstimate(altitude);
 
  sent_data.ch1 = acc_x;
  sent_data.ch2 = acc_y;
  sent_data.ch3 = acc_z;
  sent_data.ch4 = gyro_x;
  sent_data.ch5 = gyro_y;
  sent_data.ch6 = gyro_z;
  sent_data.ch7 = estimated_altitude;
  sent_data.ch8 = 0;

  radio.write(&sent_data, sizeof(Data_to_be_sent));


  while (mySerial.available())
  {
    char c = mySerial.read();
    // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
    if (gps.encode(c)) // Did a new valid sentence come in?
      newData = true;
  }
  if (newData)
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    Serial.print("LAT=");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    Serial.print(" LON=");
    Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    Serial.print(" SAT=");
    Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    Serial.print(" PREC=");
    Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
  }
  gps.stats(&chars, &sentences, &failed);
  Serial.print(" CHARS=");
  Serial.print(chars);
  Serial.print(" SENTENCES=");
  Serial.print(sentences);
  Serial.print(" CSUM ERR=");
  Serial.println(failed);
  if (chars == 0)
    Serial.println("** No characters received from GPS: check wiring **");

/*  
  Serial.print(acc_x);
  Serial.print("," );
  Serial.print(acc_y);
  Serial.print("," );
  Serial.print(acc_z);
  Serial.print("," );
  Serial.print(gyro_x);
  Serial.print("," );
  Serial.print(gyro_y);
  Serial.print("," );
  Serial.print(gyro_z);
  Serial.print("," );
  Serial.print(estimated_altitude);
  Serial.println(",0" );
  */
}



double getPressure() {
  char status;
  double T,P;
  status = pressure.startTemperature();
  if (status != 0) {
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0) {
      status = pressure.startPressure(3);
      if (status != 0) {
        delay(status);
        status = pressure.getPressure(P,T);
        if (status != 0) {
          return(P);
        }
      } 
    }  
  } 
}



void read_mpu_6050_data(){                                             //Subroutine for reading the raw gyro and accelerometer data

  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  acc_x = (Wire.read() << 8 | Wire.read()); // X-axis value
  acc_y = (Wire.read() << 8 | Wire.read()); // Y-axis value
  acc_z = (Wire.read() << 8 | Wire.read()); // Z-axis value

  // Calculating angle values using
  //accAngleX = (atan(acc_y / sqrt(pow(acc_x, 2) + pow(acc_z, 2))) * 180 / PI) + 1.15; // AccErrorX ~(-1.15) See the calculate_IMU_error()custom function for more details
  //accAngleY = (atan(-1 * acc_x / sqrt(pow(acc_y, 2) + pow(acc_z, 2))) * 180 / PI) - 0.52; // AccErrorX ~(0.5)
  
  
  // === Read gyro data === //
  //previousTime = currentTime;        // Previous time is stored before the actual time read
  //currentTime = millis();            // Current time actual time read
  //elapsedTime = (currentTime - previousTime) / 1000;   // Divide by 1000 to get seconds
 
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  gyro_x = (Wire.read() << 8 | Wire.read()); /// 32.8; // For a 1000dps range we have to divide first the raw value by 32.8, according to the datasheet
  gyro_y = (Wire.read() << 8 | Wire.read());// / 32.8;
  gyro_z = (Wire.read() << 8 | Wire.read());
  
  //gyro_x = gyro_x + 1.85; //// GyroErrorX ~(-1.85)
  //gyro_y = gyro_y - 0.15; // GyroErrorY ~(0.15)
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  //gyroAngleX = gyro_x * elapsedTime;
  //gyroAngleY = gyro_y * elapsedTime;

  // Complementary filter - combine acceleromter and gyro angle values
  //angleX = 0.98 * (angleX + gyroAngleX) + 0.02 * accAngleX;
  //angleY = 0.98 * (angleY + gyroAngleY) + 0.02 * accAngleY;
  // Map the angle values from -90deg to +90 deg into values from 0 to 255, like the values we are getting from the Joystick
  //data.j1PotX = map(angleX, -90, +90, 255, 0);
  //data.j1PotY = map(angleY, -90, +90, 0, 255);
}

void setup_mpu_6050_registers()
{
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x6B);                                                    //Send the requested starting register
  Wire.write(0x00);                                                    //Set the requested starting register
  Wire.endTransmission();                                              //End the transmission
  //Configure the accelerometer (+/-8g)
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x1C);                                                    //Send the requested starting register
  Wire.write(0x10);                                                    //Set the requested starting register
  Wire.endTransmission();                                              //End the transmission
  //Configure the gyro (500dps full scale)
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x1B);                                                    //Send the requested starting register
  Wire.write(0x10);                                                    //0x08  Set the requested starting register
  Wire.endTransmission();                                              //End the transmission
}

void mpu_cal()
{
    for (int cal_int = 0; cal_int < 2000 ; cal_int ++){                  //Run this code 2000 times
      read_mpu_6050_data();                                              //Read the raw acc and gyro data from the MPU-6050
      gyro_x_cal += gyro_x;                                              //Add the gyro x-axis offset to the gyro_x_cal variable
      gyro_y_cal += gyro_y;                                              //Add the gyro y-axis offset to the gyro_y_cal variable
      gyro_z_cal += gyro_z;                                              //Add the gyro z-axis offset to the gyro_z_cal variable
      delay(3);                                                          //Delay 3us to simulate the 250Hz program loop
  }
  gyro_x_cal /= 2000;                                                  //Divide the gyro_x_cal variable by 2000 to get the avarage offset
  gyro_y_cal /= 2000;                                                  //Divide the gyro_y_cal variable by 2000 to get the avarage offset
  gyro_z_cal /= 2000;                                                  //Divide the gyro_z_cal variable by 2000 to get the avarage offset
}




void read_IMU() {
  Wire.beginTransmission(0x68);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  //For a range of +-8g, we need to divide the raw values by 4096, according to the datasheet
  acc_x = (Wire.read() << 8 | Wire.read());// / 4096.0; // X-axis value
  acc_y = (Wire.read() << 8 | Wire.read());// / 4096.0; // Y-axis value
  acc_z = (Wire.read() << 8 | Wire.read());// / 4096.0; // Z-axis value

  // Calculating angle values using
  accAngleX = (atan(acc_y / sqrt(pow(acc_x, 2) + pow(acc_z, 2))) * 180 / PI) + 1.15; // AccErrorX ~(-1.15) See the calculate_IMU_error()custom function for more details
  accAngleY = (atan(-1 * acc_x / sqrt(pow(acc_y, 2) + pow(acc_z, 2))) * 180 / PI) - 0.52; // AccErrorX ~(0.5)

  // === Read gyro data === //
  previousTime = currentTime;        // Previous time is stored before the actual time read
  currentTime = millis();            // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000;   // Divide by 1000 to get seconds
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 4, true); // Read 4 registers total, each axis value is stored in 2 registers
  gyro_x = (Wire.read() << 8 | Wire.read());// / 32.8; // For a 1000dps range we have to divide first the raw value by 32.8, according to the datasheet
  gyro_y = (Wire.read() << 8 | Wire.read()); /// 32.8;
  gyro_x = gyro_x + 1.85; //// GyroErrorX ~(-1.85)
  gyro_y = gyro_y - 0.15; // GyroErrorY ~(0.15)
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  gyroAngleX = gyro_x * elapsedTime;
  gyroAngleY = gyro_y * elapsedTime;

  // Complementary filter - combine acceleromter and gyro angle values
  angleX = 0.98 * (angleX + gyroAngleX) + 0.02 * accAngleX;
  angleY = 0.98 * (angleY + gyroAngleY) + 0.02 * accAngleY;
  // Map the angle values from -90deg to +90 deg into values from 0 to 255, like the values we are getting from the Joystick
//  data.j1PotX = map(angleX, -90, +90, 255, 0);
 // data.j1PotY = map(angleY, -90, +90, 0, 255);
}



void calculate_IMU_error() {
  // We can call this funtion in the setup section to calculate the accelerometer and gury data error. From here we will get the error values used in the above equations printed on the Serial Monitor.
  // Note that we should place the IMU flat in order to get the proper values, so that we then can the correct values
  // Read accelerometer values 200 times
  byte c = 0;
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    acc_x = (Wire.read() << 8 | Wire.read()) / 4096.0 ;
    acc_y = (Wire.read() << 8 | Wire.read()) / 4096.0 ;
    acc_z = (Wire.read() << 8 | Wire.read()) / 4096.0 ;
    // Sum all readings
    AccErrorX = AccErrorX + ((atan((acc_y) / sqrt(pow((acc_x), 2) + pow((acc_z), 2))) * 180 / PI));
    AccErrorY = AccErrorY + ((atan(-1 * (acc_x) / sqrt(pow((acc_y), 2) + pow((acc_z), 2))) * 180 / PI));
    c++;
  }
  //Divide the sum by 200 to get the error value
  AccErrorX = AccErrorX / 200;
  AccErrorY = AccErrorY / 200;
  c = 0;
  // Read gyro values 200 times
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 4, true);
    gyro_x = Wire.read() << 8 | Wire.read();
    gyro_y = Wire.read() << 8 | Wire.read();
    // Sum all readings
    GyroErrorX = GyroErrorX + (gyro_x / 32.8);
    GyroErrorY = GyroErrorY + (gyro_y / 32.8);
    c++;
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX = GyroErrorX / 200;
  GyroErrorY = GyroErrorY / 200;
  // Print the error values on the Serial Monitor
  Serial.print("AccErrorX: ");
  Serial.println(AccErrorX);
  Serial.print("AccErrorY: ");
  Serial.println(AccErrorY);
  Serial.print("GyroErrorX: ");
  Serial.println(GyroErrorX);
  Serial.print("GyroErrorY: ");
  Serial.println(GyroErrorY);
}
