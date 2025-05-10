int vaporizer = 9;  // 定义连接到HE-30A的PWM引脚
void setup() {
  pinMode(vaporizer, OUTPUT);  // 设置引脚为输出
}

void loop() {
  // 打开雾化器1000毫秒
  analogWrite(vaporizer, 255);  // 255代表100%的PWM信号，即全开
  delay(1000);                   // 等待1000毫秒
  
  // 关闭雾化器1000毫秒
  analogWrite(vaporizer, 0);     // 0代表0%的PWM信号，即关闭
  delay(1000);                   // 等待1000毫秒
}