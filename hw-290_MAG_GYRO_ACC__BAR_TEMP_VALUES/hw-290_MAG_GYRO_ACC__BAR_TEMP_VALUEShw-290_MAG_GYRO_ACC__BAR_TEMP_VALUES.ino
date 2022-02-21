#include <Wire.h> //I2C Arduino Library
#include <Adafruit_BMP085.h>

#define addr 0x0D //I2C Address for The HMC5883
#define MPU  0X68 //I2C Address for The MPU6050
#define seaLevelPressure_hPa 1013.25

Adafruit_BMP085 bmp;
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float elapsedTime, currentTime, previousTime;
int c = 0;


void setup() {

  Serial.begin(115200);
      Wire.begin();

  
    if (!bmp.begin()) {
        Serial.println("Could not find a valid BMP085 sensor, check wiring!");
        while (1) {}
     }
  
  //SETTING UP ACC/GYRO   
      Wire.beginTransmission(MPU);
      Wire.write(0x37);
      Wire.write(0x02);
      Wire.endTransmission();

      Wire.beginTransmission(MPU);
      Wire.write(0x6A);
      Wire.write(0x00);
      Wire.endTransmission();

      //Disable Sleep Mode
      Wire.beginTransmission(MPU);
      Wire.write(0x6B);
      Wire.write(0x00);
      Wire.endTransmission();

      Wire.beginTransmission(MPU);
      Wire.write(0x1C);                  //Talk to the ACCEL_CONFIG register (1C hex)
      Wire.write(0x10);                  //Set the register bits as 00010000 (+/- 8g full scale range)
      Wire.endTransmission(true);
      // Configure Gyro Sensitivity - Full Scale Range (default +/- 250deg/s)
      Wire.beginTransmission(MPU);
      Wire.write(0x1B);                   // Talk to the GYRO_CONFIG register (1B hex)
      Wire.write(0x10);                   // Set the register bits as 00010000 (1000deg/s full scale)
      Wire.endTransmission(true);
      calculate_IMU_error();
      delay(20);



//SETTING UP MAG
      Wire.beginTransmission(addr); //start talking
      Wire.write(0x0B); // Tell the HMC5883 to Continuously Measure
      Wire.write(0x01); // Set the Register
      Wire.endTransmission();
      Wire.beginTransmission(addr); //start talking
      Wire.write(0x09); // Tell the HMC5883 to Continuously Measure
      Wire.write(0x1D); // Set the Register
      Wire.endTransmission();
}

void loop() {

  int x, y, z; //triple axis data

  //Tell the HMC what regist to begin writing data into


  Wire.beginTransmission(addr);
  Wire.write(0x00); //start with register 3.
  Wire.endTransmission();

  //Read the data.. 2 bytes for each axis.. 6 total bytes
  Wire.requestFrom(addr, 6);
  if (6 <= Wire.available()) {
    x = Wire.read(); //MSB  x
    x |= Wire.read() << 8; //LSB  x
    z = Wire.read(); //MSB  z
    z |= Wire.read() << 8; //LSB z
    y = Wire.read(); //MSB y
    y |= Wire.read() << 8; //LSB y
  }

  // Show Values


 /// collecting MPU DATA
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
  AccX = (Wire.read() << 8 | Wire.read());// / 16384.0; // X-axis value
  AccY = (Wire.read() << 8 | Wire.read());// / 16384.0; // Y-axis value
  AccZ = (Wire.read() << 8 | Wire.read());// / 16384.0; // Z-axis value
  // Calculating Roll and Pitch from the accelerometer data
  // accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) - 0.58; // AccErrorX ~(0.58) See the calculate_IMU_error()custom function for more details
  // accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) + 1.58; // AccErrorY ~(-1.58)
  // === Read gyroscope data === //
  // previousTime = currentTime;        // Previous time is stored before the actual time read
  //currentTime = millis();            // Current time actual time read
  //elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds

 
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  GyroX = (Wire.read() << 8 | Wire.read());// / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  GyroY = (Wire.read() << 8 | Wire.read());// / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read());// / 131.0;
  Serial.print(x);
  Serial.print("," );
  Serial.print(y);
  Serial.print("," );
  Serial.print(z);
  Serial.print("," );
  Serial.print(AccX);
  Serial.print("," );
  Serial.print(AccY);
  Serial.print("," );
  Serial.print(AccZ);
  Serial.print("," );
  Serial.print(GyroX);
  Serial.print("," );
  Serial.print(GyroY);
  Serial.print("," );
  Serial.print(GyroZ);
  Serial.print(",");
  Serial.print(bmp.readTemperature());
  Serial.print("," );
  Serial.print(bmp.readPressure());  
  Serial.print("," );
  Serial.print(bmp.readAltitude());
  Serial.println(";");


  // Correct the outputs with the calculated error values
  // GyroX = GyroX + 0.56; // GyroErrorX ~(-0.56)
  //GyroY = GyroY - 2; // GyroErrorY ~(2)
  //GyroZ = GyroZ + 0.79; // GyroErrorZ ~ (-0.8)
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  //gyroAngleX = gyroAngleX + GyroX * elapsedTime; // deg/s * s = deg
  //gyroAngleY = gyroAngleY + GyroY * elapsedTime;
  //yaw =  yaw + GyroZ * elapsedTime;
  // Complementary filter - combine acceleromter and gyro angle values
  //roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  // pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
  
  // Print the values on the serial monitor
  //Serial.print("ACC X:" );
  //Serial.print("/");
  //Serial.print(pitch);
  //Serial.print("/");
  //Serial.println(yaw);




  delay(20);
}



void calculate_IMU_error() {
  // We can call this funtion in the setup section to calculate the accelerometer and gyro data error. From here we will get the error values used in the above equations printed on the Serial Monitor.
  // Note that we should place the IMU flat in order to get the proper values, so that we then can the correct values
  // Read accelerometer values 200 times
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    // Sum all readings
    AccErrorX = AccErrorX + ((atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI));
    AccErrorY = AccErrorY + ((atan(-1 * (AccX) / sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI));
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
    Wire.requestFrom(MPU, 6, true);
    GyroX = Wire.read() << 8 | Wire.read();
    GyroY = Wire.read() << 8 | Wire.read();
    GyroZ = Wire.read() << 8 | Wire.read();
    // Sum all readings
    GyroErrorX = GyroErrorX + (GyroX / 131.0);
    GyroErrorY = GyroErrorY + (GyroY / 131.0);
    GyroErrorZ = GyroErrorZ + (GyroZ / 131.0);
    c++;
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX = GyroErrorX / 200;
  GyroErrorY = GyroErrorY / 200;
  GyroErrorZ = GyroErrorZ / 200;
  // Print the error values on the Serial Monitor
  Serial.print("AccErrorX: ");
  Serial.println(AccErrorX);
  Serial.print("AccErrorY: ");
  Serial.println(AccErrorY);
  Serial.print("GyroErrorX: ");
  Serial.println(GyroErrorX);
  Serial.print("GyroErrorY: ");
  Serial.println(GyroErrorY);
  Serial.print("GyroErrorZ: ");
  Serial.println(GyroErrorZ);
}
