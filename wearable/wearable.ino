#include <Wire.h>

#define PI 3.141592
#define SHOW_DEBUG false

const int MPU_addr=0x68;

int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
String out = ;
uint8_t rollThresold = 40, pitchThreshold = 20;


void setup()
{
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  // PWR_MGMT_1 register : Power up MPU
  Wire.write(0x6B);
  // wake up the MPU-6050
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(115200);
}

void loop()
{
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);                 // 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  // total 14 registers are to be read
  Wire.requestFrom(MPU_addr,14,true);
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  // apply trigonometry to get the pitch and roll:
  float pitch = atan(GyX/sqrt(pow(GyY,2) + pow(GyZ,2)));
  float roll = atan(GyY/sqrt(pow(GyX,2) + pow(GyZ,2)));
  // convert radians into degrees
  pitch = int(pitch * (180.0/PI));
  roll = int(roll * (-180.0/PI));
  if(! SHOW_DEBUG)
  {
    // adjust threshold
    if(pitch >= 0)
    {
      pitch = max(0,pitch - pitchThreshold);
      int pitchScale = map(pitch,0,90,0,255);
    }
    else if(pitch < 0)
    {
      pitch = min(0,pitch + pitchThreshold);
      int pitchScale = map(pitch,0,-90,0,255);
    }
    if(roll >= 0)
    {
      roll = max(0,roll-rollThreshold);
      int rollScale = map(roll,0,90,0,255);
    }
    else if(roll < 0)
    {
      roll = min(0,roll+rollThreshold);
      int rollScale = map(roll,0,-90,0,255);
    }
    
    // convert to byte
    pitchScale = byte(pitchScale);
    rollScale = byte(rollScale);

    // find direction
    if(pitch < 0 && roll < 0)
      Serial.print("#1");
    else if(pitch < 0 && roll == 0)
      Serial.print("#2");
    else if(pitch < 0 && roll > 0)
      Serial.print("#3");
    else if(pitch == 0 && roll < 0)
      Serial.print("#4");
    else if(pitch == 0 && roll > 0)
      Serial.print("#6");
    else if(pitch > 0 && roll < 0)
      Serial.print("#7");
    else if(pitch > 0 && roll == 0)
      Serial.print("#8");
    else if(pitch > 0 && roll > 0)
      Serial.print("#9");
    
    Serial.print(pitchScale);
    Serial.print(rollScale);
    Serial.print("~");
  }
  
  if(SHOW_DEBUG)
  {
    Serial.print("AcX   "); Serial.print(AcX);
    Serial.print("\tAcY   "); Serial.print(AcY);
    Serial.print("\tAcZ   "); Serial.print(AcZ);
    Serial.print("\tGyX   "); Serial.print(GyX);
    Serial.print("\tGyY   "); Serial.print(GyY);
    Serial.print("\tGyZ   "); Serial.print(GyZ);
    Serial.print("\tpit   "); Serial.print(pitch);
    Serial.print("\trol   "); Serial.println(roll);
  }

  delay(20);
}
