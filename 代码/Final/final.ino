/***********************************
电机序号编号
0号电机（LF左前）   1号电机（RF右前）
2号电机（RB右后）   3号电机（LB左后）
*************************************/
/*****************************************
更新日志2021年5月24日，毕业答辩前一天重构代码
******************************************/

// 包含头文件 软串口、定时中断
#include <SoftwareSerial.h>
#include <FlexiTimer2.h>

// 蓝牙接口定义
SoftwareSerial BlueTooth(50, 51); // RX,TX

// 电机端口初始化极其含义
//IN1 IN2 PWM EncodeA EncodeB
int MotorPins[4][5] = {
    {6, 17, 7, 2, 28},
    {12, 15, 4, 21, 30},
    {11, 16, 5, 20, 26},
    {23, 8, 13, 3, 24},
};

// 一些全局变量的定义
// 数圈数 当前速度 目标速度 目标角度 PWM 等
int MotorCount = 0; //反正每次只能测一个电机的转速，所以不是数组
float Velocity[4] = {0};
float Target[4] = {0};
float Target_V = 0; //输入专用速度
String TargetVelocity;
float TargetA = 0;
String TargetAngle;
float PWMControl[4] = {0};
int PWMStart[4] = {0, 0, 0, 0};
int PWM[4] = {0}; //PWM是整数

// PID 参数定义及设置   四个轮子使用四套PID参数
typedef struct
{
    float Kp; //比例系数Proportional
    float Ki; //积分系数Integral
    float Kd; //微分系数Derivative

    float Ek;  //当前误差
    float Ek1; //前一次误差 e(k-1)
    float Ek2; //再前一次误差 e(k-2)
} PID_IncTypeDef;

PID_IncTypeDef pid0 = {10, 1, 0.1, 0, 0, 0};
PID_IncTypeDef pid1 = {10, 1, 0.1, 0, 0, 0};
PID_IncTypeDef pid2 = {10, 1, 0.1, 0, 0, 0};
PID_IncTypeDef pid3 = {10, 1, 0.1, 0, 0, 0};
PID_IncTypeDef pid[4] = {pid0, pid1, pid2, pid3};

// 程序初始化
void setup()
{
    MotorInit();                   //电机初始化
    Serial.begin(9600);            //硬串口初始化
    BlueTooth.begin(9600);         //蓝牙使用的软串口初始化
    FlexiTimer2::set(5, control);  //时间中断初始化
    FlexiTimer2::start();          //每5ms中断一次，中断函数为control
}

// 程序进入主循环
void loop()
{
    // 计算当前实际速度
    GetV();
    // 蓝牙读取遥控器指令及对应操作判断
    char ch = BlueTooth.read(); // 传递信息
    switch (ch)
    {
    case 'S':
        Stop();
        break;

    case 'F':
        GoForward(1);
        break;

    case 'B':
        GoBackward(1);
        break;
    case 'L':

        TurnLeft(60);
        break;
    case 'R':

        TurnRight(60);
        break;

    // 输入A进入角度调试模式
//    case 'A':
//        //读取串口的输入作为目标角度
//        TargetAngle = Serial.readString();
//        TargetA = TargetAngle.toFloat();
//        break;
    
    // 输入V进入速度调试模式
    //case 'V':
        //读取串口的输入作为目标速度
//        TargetVelocity = Serial.readString();
//        Target_V = TargetVelocity.toFloat();
//        if(Target_V > 0 && Target_V < 50) {
//            Target[0] = Target_V;
//            Target[1] = Target_V;
//            Target[2] = Target_V;
//            Target[3] = Target_V;
//        }
//        break;
//    
    //default:
        //Serial.println("请重新输入");
    }
    // 向串口输出当前的角度和速度和PWM值
    //PrintV();
    //PrintPwm();
    //PrintAngle(); // 角度函数还没有写先欠着
}

// 对电机的各个参数进行初始化
void MotorInit()
{
    for (int i = 0; i < 4; i++)
    {
        pinMode(MotorPins[i][0], OUTPUT);
        pinMode(MotorPins[i][1], OUTPUT);
        pinMode(MotorPins[i][2], OUTPUT);
        pinMode(MotorPins[i][3], INPUT);
        pinMode(MotorPins[i][4], INPUT);

        digitalWrite(MotorPins[i][0], LOW);
        digitalWrite(MotorPins[i][1], LOW);
        digitalWrite(MotorPins[i][2], LOW);
    }
}

// 定时中断的函数
void control()
{
    //计算对应当前目标速度的PWM增量值
    for (int n = 0; n < 4; n++)
    {
        PWMControl[n] = PwmInc(n, Target[n]);
    }
    // 对PWMControl的值进行处理并得到PWM
    SetPWM();
    // 不断更新角度数据
}

//计算当前某序号电机的实际速度
void GetV()
{
    // n是电机的序号,更新并保存四个电机的转速
    for (int n = 0; n < 4; n++)
    {
        MotorCount = 0;
        unsigned long nowtime = 0;
        nowtime = millis() + 50;
        attachInterrupt(digitalPinToInterrupt(MotorPins[n][3]), ReadMotor, RISING); // 2560 的中断2口对应编号0
        while (millis() < nowtime)
            ;
        detachInterrupt(digitalPinToInterrupt(MotorPins[n][3]));
        // 轮组直径8.5cm 电机A相每圈11个脉冲 电机减速比为56：1 计算转速时间间隔为50ms
        Velocity[n] = MotorCount * 8.5 * PI / 11 / 56 / 0.05;
    }
}
void ReadMotor()
{
    MotorCount++;
}

// 不同的运动模式
void DriveMotor(int n, int TargetV)
{
    Target[n] = TargetV;
    if(Target[n] == 0) {
        digitalWrite(MotorPins[n][0], LOW);
        digitalWrite(MotorPins[n][1], LOW);
    }
    if(Target[n] > 0) {
        digitalWrite(MotorPins[n][0], LOW);
        digitalWrite(MotorPins[n][1], HIGH);
    }
    if(Target[n] < 0) {
        digitalWrite(MotorPins[n][0], HIGH);
        digitalWrite(MotorPins[n][1], LOW);
    }
    analogWrite(MotorPins[n][2], PWMStart[n]+PWM[n]);
}

void Stop()
{
    // 为了保证响应速度，每个电机会设置一个刚好不使轮子转动的PWM值
    Target[0] = 0;
    Target[1] = 0;
    Target[2] = 0;
    Target[3] = 0;
}
void GoForward(int TargetV)
{
    Target[0] = TargetV;
    Target[1] = TargetV;
    Target[2] = TargetV;
    Target[3] = TargetV;
}
void GoBackward(int TargetV)
{
    Target[0] = -TargetV;
    Target[1] = -TargetV;
    Target[2] = -TargetV;
    Target[3] = -TargetV;
}
void TurnLeft(int TargetV)
{
    Target[0] = -TargetV;
    Target[1] = TargetV;
    Target[2] = TargetV;
    Target[3] = TargetV;
}
void TurnRight(int TargetV)
{
    Target[0] = TargetV;
    Target[1] = -TargetV;
    Target[2] = TargetV;
    Target[3] = TargetV;
}

//输出电机的转速、PWM值、角度
void PrintV()
{
    Serial.println("******输出电机转速******");
    for (int i = 0; i < 4; i++)
    {
        Serial.print("Motor");
        Serial.print(i);
        Serial.print(":");
        Serial.print(Velocity[i]);
        Serial.print("  ");
    }
    Serial.println();
}
void PrintPwm()
{
    Serial.println("******输出电机PWM值******");
    for (int i = 0; i < 4; i++)
    {
        Serial.print("PWM");
        Serial.print(i);
        Serial.print(":");
        Serial.print(PWM[i]);
        Serial.print("  ");
    }
    Serial.println();
}
void PrintAngle()
{
}

// 将计算好的PWMControl进行限制并赋值到PWM[4]
void SetPWM()
{
    for (int n = 0; n < 4; n++)
    {
        if (PWMControl[n] > 255)
        {
            PWM[n] = 255;
        }
        PWM[n] = (int)PWMControl;
    }
    DriveMotor(0, Target[0]);
    DriveMotor(1, Target[1]);
    DriveMotor(2, Target[2]);
    DriveMotor(3, Target[3]);
}

// 通过增量PID计算PWM增量
float PwmInc(int n, float SetValue)
{
    if(SetValue < 0) {
        SetValue = -SetValue;
    }
    float PWMInc = 0;
    float ActualValue = 0;

    PID_IncTypeDef *PID = &pid[n];

    ActualValue = Velocity[n];

    PID->Ek = SetValue - ActualValue;
    PWMInc += (PID->Kp * PID->Ek) - (PID->Ki * PID->Ek1) + (PID->Kd * PID->Ek2);

    PID->Ek2 = PID->Ek1;
    PID->Ek1 = PID->Ek;

    return PWMInc;
}
