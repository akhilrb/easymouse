#include <Wire.h>

#define PI 3.141592

const int MPU_addr=0x68;

int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

void setup()
{
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(57600);
}

void loop()
{
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  // apply trigonometry to get the pitch and roll:
  //float pitch = atan(xAxis/sqrt(pow(yAxis,2) + pow(zAxis,2)));
  //float roll = atan(yAxis/sqrt(pow(xAxis,2) + pow(zAxis,2)));
  //convert radians into degrees
  //pitch = pitch * (180.0/PI);
  //roll = roll * (180.0/PI) ;
  Serial.print("AcX = "); Serial.print(AcX);
  Serial.print("\tAcY = "); Serial.print(AcY);
  Serial.print("\tAcZ = "); Serial.print(AcZ);
  Serial.print("\tTmp = "); Serial.print(Tmp/340.00+36.53);  //equation for temperature in degrees C from datasheet
  Serial.print("\tGyX = "); Serial.print(GyX);
  Serial.print("\tGyY = "); Serial.print(GyY);
  Serial.print("\t\tGyZ = "); Serial.println(GyZ);
  delay(100);
}
/*
void setup()
{
  Serial.begin(115200);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
}

void loop()
{
  if(!digitalRead(4))
  {
    Serial.print("C");
    delay(500);
  }
  if(!digitalRead(5))
  {
    Serial.print("D");
    delay(500);
  }
  if(!digitalRead(6))
  {
    Serial.print("I");
    delay(500);
  }
}*/
