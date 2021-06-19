/*
板子开关打到3.3V  341/343 365/368 值的范围是0到720  
手柄中间位置的模拟值490/491  523/524，（板子电源选择开关打到5V，值的范围0到1023）
设置一个前进后退转弯的功能
蓝牙模块通信
*/

#include<SoftwareSerial.h>
SoftwareSerial BlueTooth(8, 9); // RX,TX

int Up = 6;    // 上按钮不可用，其他三个正常
int Right = 3; // 按下右边按钮时digitalRead(Right)的值为0，平常默认值为1
int Down = 4;
int Left = 5;
int X, Y = 0;
int style = 0;

void Init()
{
    pinMode(Up, INPUT_PULLUP);
    pinMode(Right, INPUT_PULLUP);
    pinMode(Down, INPUT_PULLUP);
    pinMode(Left, INPUT_PULLUP);
}

// 1，2，3. 4 分别代表前进，右转，后退，左转
void Style()
{

    if (X >= 200 && X <= 500 && Y >= 200 && Y <= 500)
    {
        style = 0;
    }

    if (Y > 500 && X > 200 && X < 500)
    {
        style = 1;
    }

    if (Y > 200 && Y < 500 && X > 500)
    {
        style = 2;
    }

    if (Y < 200 && X > 200 && X < 500)
    {
        style = 3;
    }

    if (Y > 200 && Y < 500 && X < 200)
    {
        style = 4;
    }
}

void setup(void)
{
    Init();
    Serial.begin(9600);
    BlueTooth.begin(9600);
    // Serial.println("init serial port AT");
}

void loop(void)
{
    Style();
    X = analogRead(A0);
    Y = analogRead(A1);

//if(BlueTooth.available()>0) {
    if (digitalRead(Right) == 0){
            BlueTooth.print("U");
        }
    if (digitalRead(Left) == 0){
            BlueTooth.print("C");
    }


    if (style == 0)
    {
        BlueTooth.print("S");
    }
    if (style == 1)
    {
        BlueTooth.print("F");
    }
    if (style == 2)
    {
        BlueTooth.print("R");
    }
    if (style == 3)
    {
        BlueTooth.print("B");
    }
    if (style == 4)
    {
        BlueTooth.print("L");
    }
    


//}

//      if(Serial.available()>0)
//      {
//          char ch=Serial.read();
//          if ( ch == 'S') {
//            Serial.println("Stay");
//          }
//          if (ch == 'F') {
//            Serial.println("GoForward"); ,
//          } 
//      }

    delay(1);
}
