/*
Arduino先上传好程序， 然后Python用COM7与单片机通信
输入o打开 输入c关闭 灯
*/

const int pinNumber = 13; //使用UNO自带的L来检测串口通信的效果

//这里就是定义 开和关
void setup(){  //arduino程序必须有的函数，用作初始化，设置管脚，调用库等等
    Serial.begin(9600); //这里要和树莓派中串口波特率一致
    pinMode(pinNumber,OUTPUT); //将10号管脚状态设置为输出状态
}
void loop() {
  char val = Serial.read();
  if(val == 'o')
  {
    digitalWrite(pinNumber,HIGH); 
    }
  if(val == 'c')
  {
    digitalWrite(pinNumber,LOW); 
    }
}
