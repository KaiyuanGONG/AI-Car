
#include <PinChangeInt.h>

//int testPIN[4] = {5, 6, 7, 8};
int testPIN = 5;
//float Velocity[4] = {0, 0, 0, 0};
float Velocity = 0;
int MotorCount = 0;

volatile unsigned int MortorCount = 0; // 最大到 65535,  接着会归零从头算起

void ReadMotor() {   // 中断处理程序
  MotorCount++;
}

float Read_Motor_V()
{

  MotorCount = 0;
  unsigned long nowtime = 0;
  nowtime = millis() + 50;
  PCattachInterrupt(testPIN, ReadMotor, RISING);
  while (millis() < nowtime)
    ;
  PCdetachInterrupt(testPIN);
  //Velocity = ((MotorCount / (11 * 56)) * 8.5 * PI) / 0.05;
  Velocity = ((MotorCount * 6.5 * PI * 20) / 11) / 56;
  return Velocity;
}


// Attach the interrupt in setup()
void setup() {
  pinMode(testPIN,  INPUT);
  Serial.begin(9600);
  //Serial.println("start ---------");
}

void loop() {
  //for(int i = 0; i < 4; i++) {
  Read_Motor_V();
  while (!Serial);
  //Serial.print("Velocity");
  //Serial.print(i);
  //Serial.print(": ");
  Serial.println(Velocity);
  //Serial.println("cm/s ");
  //delay(100);
  //}
  //Serial.println("-----------");
  delay(10);
}
