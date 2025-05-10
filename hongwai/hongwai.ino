/****************************************hcsr501 part****************************************/
#define hcsr501Pin 7                                // 设置对应的引脚
#define hcsr501TimeInterval 1000                    // 检测一次的时间间隔
#define existTimeInterval 20000                     // 人存在的时间间隔
#define stableCountThreshold 10                      // 稳定计数阈值

unsigned long hcsr501Times = 0, existTimes = 0;      // 记录设备运行时间
int hcsr501Val = 0, existVal = 0;                    // 存储传感器值和存在状态
int stableCount = 0;                                 // 稳定计数器

/**************************************** set up and loop part *********************************/
void setup() {
  Serial.begin(9600);                               // 设置串口波特率为9600
  pinMode(hcsr501Pin, INPUT);                       // 引脚设置成输入
  
  Serial.println("设备上线！");
}

void loop() {
  judgmentExist();                                  // 判断人是否存在
}

/**************************************** hcsr501 part ****************************************/
/* 获取传感器的数据 */
void getHcsrData() {
  if (millis() - hcsr501Times >= hcsr501TimeInterval) {
    hcsr501Times = millis();                        // 一定时间执行一次
    int readVal = digitalRead(hcsr501Pin);
 
    // 如果读取的值与之前的值相同，则增加稳定计数器
    if (readVal == hcsr501Val) {
      stableCount++;
    } else {
      // 如果值改变，则重置稳定计数器和传感器值
      stableCount = 0;
      hcsr501Val = readVal;
    }
 
    // 如果稳定计数器超过阈值，则认为传感器状态稳定
    if (stableCount >= stableCountThreshold) {
      Serial.println(hcsr501Val);                   // 串口打印对应的值
    }
  }
}

/* 判断人是否存在 */
void judgmentExist() {
  getHcsrData();                                    // 获取传感器的数据
  if (hcsr501Val == 1 && existVal == 0) {            // 检测到有人
    existVal = 1;
    existTimes = millis();                          // 记录时间
    Serial.println("有人！");                        // 串口打印对应的值
  } else if (hcsr501Val == 0 && existVal == 1) {    // 检测到无人
    if (millis() - existTimes < existTimeInterval) {
      // 如果持续时间小于设定的阈值，则认为人还在
      existVal = 1;
      existTimes = millis();                        // 重置时间
    } else {
      existVal = 0;
      Serial.println("没人！");                      // 串口打印对应的值
    }
  }
}