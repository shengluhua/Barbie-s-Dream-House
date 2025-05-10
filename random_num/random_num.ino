#include <Arduino.h>

// 定义数码管的引脚//共阴
#define SEG_A 2
#define SEG_B 3
#define SEG_C 4
#define SEG_D 5
#define SEG_E 6
#define SEG_F 7
#define SEG_G 8
#define SEG_DP 9

// 数码管0-9的编码（共阴极）
byte numbers[10] = {
  B00111111,  // "0" 对应的二进制是 0b00111111
  B00000110,  // "1" 对应的二进制是 0b00000110
  B01011011,  // "2" 对应的二进制是 0b01011011
  B01001111,  // "3" 对应的二进制是 0b01001111
  B01100110,  // "4" 对应的二进制是 0b01100110
  B01101101,  // "5" 对应的二进制是 0b01101101
  B01111101,  // "6" 对应的二进制是 0b01111101
  B00000111,  // "7" 对应的二进制是 0b00000111
  B01111111,  // "8" 对应的二进制是 0b01111111
  B01101111   // "9" 对应的二进制是 0b01101111
};

void setup() {
  // 初始化数码管引脚为输出模式
  pinMode(SEG_A, OUTPUT);
  pinMode(SEG_B, OUTPUT);
  pinMode(SEG_C, OUTPUT);
  pinMode(SEG_D, OUTPUT);
  pinMode(SEG_E, OUTPUT);
  pinMode(SEG_F, OUTPUT);
  pinMode(SEG_G, OUTPUT);
  pinMode(SEG_DP, OUTPUT);

  // 初始化串口通信
  Serial.begin(9600);
  while (!Serial) {
    ; // 等待串口连接。仅适用于 Leonardo。
  }
  Serial.println("Enter 'r' to randomize the display.");
}

void loop() {
  // 检查串口是否有数据
  if (Serial.available() > 0) {
    // 读取串口数据
    char command = Serial.read();
    
    // 如果接收到 'r' 命令，则随机显示数字
    if (command == 'r') {
      randomizeDisplay();
    }
  }
}

void randomizeDisplay() {
  // 生成一个0到9的随机数
  int randomNumber = random(0, 10);
  
  // 显示随机数
  displayNumber(randomNumber);
}

void displayNumber(int number) {
  // 根据数字设置数码管的段
  digitalWrite(SEG_A, (numbers[number] & B00000001)); // 共阳极，所以取反
  digitalWrite(SEG_B, (numbers[number] & B00000010));
  digitalWrite(SEG_C, (numbers[number] & B00000100));
  digitalWrite(SEG_D, (numbers[number] & B00001000));
  digitalWrite(SEG_E, (numbers[number] & B00010000));
  digitalWrite(SEG_F, (numbers[number] & B00100000));
  digitalWrite(SEG_G, (numbers[number] & B01000000));
  digitalWrite(SEG_DP, LOW); // 通常情况下，不显示小数点
}