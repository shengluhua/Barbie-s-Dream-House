#include <DHT11.h>  // 引入DHT库
#include <SoftwareSerial.h>
#include <Servo.h>  // 引入舵机控制库
#include <Ultrasonic.h>  // 引入超声波库

SoftwareSerial mySerial(14, 15); // RX, TX
SoftwareSerial tftSerial(16, 17); // RX, TX

#define DHTPIN 4      // DHT11 数据引脚连接到 Arduino 的 6 号引脚
#define FAN_PIN 3     // 风扇控制引脚
#define HUMIDIFIER_PIN 6 // 加湿器控制引脚

#define PIR_sensor A5      // 红外传感器引脚   靠近黄色 GND OUT VCC
#define TRIGGER_PIN 7     // 超声波 Trigger 引脚
#define ECHO_PIN 8        // 超声波 Echo 引脚
#define SERVO_PIN 9       // 舵机控制引脚

#define LED_RED 5
#define LED_BLUE 10
#define LED_GREEN 11

DHT11 dht(DHTPIN); // 创建一个DHT11实例
Servo myServo; // 创建舵机对象
void setLED(int red, int green, int blue);

int fanSpeed = 0; // 用于存储当前风扇速度的变量
int humidifierStatus = LOW; // 加湿器状态（LOW 为关闭，HIGH 为开启）
int personDetected = 0; // 人体感应状态，0 为没有检测到，1 为检测到
unsigned long lastCheckTime = 0; // 上次检查时间
const long checkInterval = 20000; // 检查间隔，单位为毫秒（20s）

long duration; // 用于存储超声波脉冲的持续时间
int distance; // 用于存储计算出的距离

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  //tftSerial.begin(9600);

  pinMode(FAN_PIN, OUTPUT);  // 设置风扇引脚为输出
  pinMode(HUMIDIFIER_PIN, OUTPUT);  // 设置加湿器引脚为输出
  pinMode(TRIGGER_PIN, OUTPUT); // 设置触发引脚为输出模式
  pinMode(ECHO_PIN, INPUT); // 设置回声引脚为输入模式

  myServo.attach(SERVO_PIN); // 初始化舵机引脚
  pinMode(PIR_sensor, INPUT); // 人体红外传感器引脚为输入

  pinMode(LED_RED, OUTPUT);  // 设置红色LED引脚为输出
  pinMode(LED_BLUE, OUTPUT);  // 设置蓝色LED引脚为输出
  pinMode(LED_GREEN, OUTPUT);  // 设置黄色LED引脚为输出
}

void loop() {
  
//Serial.println("Wait for Command"); // 等待命令
  String command = "";
  bool startAppending = false; // 标志变量，指示是否开始拼接字符
  unsigned long startMillis = millis();

  while (mySerial.available() > 0 && millis() - startMillis < 1000) {
    char c = mySerial.read();  // 逐个字符读取

    // 检查是否收到字母字符，并且是否应该开始拼接
    if (!startAppending && isAlpha(c)) {
      command += c;  // 如果是字母，开始拼接
      startAppending = true;  // 设置开始拼接标志为 true
    } else if (startAppending) {
      command += c;  // 如果已经开始拼接，继续拼接字符
    }

    if (c == '\n') break;  // 如果遇到换行符就结束
  }
  command.trim();  // 去除可能的前后空格

  if (command.length() > 0) {
    Serial.println("Received command: " + command);
    Serial.println(command.length());
    delay(2000);

     tftSerial.println(command);

     if (command.equals("Fan On")) {  // 打开风扇
    fanSpeed = 255; // 设置为最大速度
    analogWrite(FAN_PIN, fanSpeed);
    Serial.println("Fan is ON");
    mySerial.print("Fan is ON");
    tftSerial.println("Fan is ON");
  } 
  else if (command.equals("Fan Off")) {  // 关闭风扇
    fanSpeed = 0; // 设置为0速度
    analogWrite(FAN_PIN, fanSpeed);
    Serial.println("Fan is OFF");
    mySerial.print("Fan is ON");
    tftSerial.println("Fan is OFF");
  } 
  else if (command.equals("Fan Up")) {  // 调高风速
    if (fanSpeed < 255) {
      fanSpeed += 30; // 每次增加10
      if (fanSpeed > 255) {
        fanSpeed = 255;
      }
      analogWrite(FAN_PIN, fanSpeed);
      Serial.print("Fan speed increased to ");
      Serial.println(fanSpeed);
      tftSerial.print("Fan speed increased to ");
      tftSerial.println(fanSpeed);
    }
  } 
  else if (command.equals("Fan Down")) {  // 调低风速
    if (fanSpeed > 0) {
      fanSpeed -= 30; // 每次减少10
      if (fanSpeed < 0) {
        fanSpeed = 0;
      }
      analogWrite(FAN_PIN, fanSpeed);
      Serial.print("Fan speed decreased to ");
      Serial.println(fanSpeed);
      tftSerial.print("Fan speed decreased to ");
      tftSerial.println(fanSpeed);
    }
  }
    else if (command.equals("Conditioner Off")) {  // 关闭加湿器
      digitalWrite(HUMIDIFIER_PIN, LOW);
      humidifierStatus = LOW;
      Serial.println("Humidifier is OFF");
      tftSerial.println("Humidifier is OFF");
    } 
    else if (command.equals("Conditioner On")) {  // 打开加湿器
      digitalWrite(HUMIDIFIER_PIN, HIGH);
      humidifierStatus = HIGH;
      Serial.println("Humidifier is ON");
      tftSerial.println("Humidifier is ON");
    } 
    else if (command.equals("Choose Clothes")) {   //choose clothes
      tftSerial.println("Choose clothes");
    } 
     else if (command.equals("Display all clothes")) {   //Display all clothes
      tftSerial.println("Display all clothes");
    } 
    else if (command.equals("Read Temperature")) { 
      float temperature = dht.readTemperature(); // 读取温度 
      if (isnan(temperature)) {
        Serial.println("Failed to read temperature!");
      } else {
        Serial.print("Current Temperature: ");
        Serial.print(temperature);
        Serial.println("C");
        tftSerial.println("Current Temperature: ");
        tftSerial.println(temperature);
        tftSerial.println("C");

      }
    } 
    else if (command.equals("Read Humidity")) {  // 读取湿度
      float humidity = dht.readHumidity(); // 读取湿度
      if (isnan(humidity)) {
        Serial.println("Failed to read humidity!");
      } else {
        Serial.print("Current Humidity: ");
        Serial.print(humidity);
        Serial.println("%");
        tftSerial.println("Current Humidity: ");
        tftSerial.println(humidity);
        tftSerial.println("%");
      }
    }
    

  } 
  unsigned long currentMillis = millis();
  //tftSerial.println("111111");
  if (currentMillis - lastCheckTime > checkInterval) {
    lastCheckTime = currentMillis; // 更新上次检查时间

    float humidity = dht.readHumidity(); // 读取湿度
    float temperature = dht.readTemperature(); // 读取温度

    // 温度和湿度控制
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read temperature or humidity!");
    } else {
      bool temperatureLow = temperature < 24;
      bool temperatureHigh = temperature > 27;
      bool humidityLow = humidity < 30;
      bool humidityNormal = humidity >= 30;

      String status;
      Serial.print("humidityLow");
      Serial.println(humidityLow);
      Serial.print("humidityNormal:");
      Serial.println(humidityNormal);

      // 根据温度和湿度状态设置LED颜色
    if (temperatureLow && humidityLow) {
      setLED(0, 0, 255); // 蓝色
      Serial.println(F("Blue LED ON for Low temperature and Low humidity"));
    } else if (temperatureLow && humidityNormal) {
      setLED(0, 255, 255); // 青色
      Serial.println(F("Cyan LED ON for Low temperature and Normal humidity"));
    } else if (!temperatureLow && !temperatureHigh && humidityLow) {
      setLED(255, 255, 0); // 黄色
      Serial.println(F("Yellow LED ON for Normal temperature and Low humidity"));
    } else if (!temperatureLow && !temperatureHigh && humidityNormal) {
      setLED(255, 255, 255); // 白色
      Serial.println(F("White LED ON for Normal temperature and Normal humidity"));
    } else if (temperatureHigh && humidityLow) {
      setLED(255, 0, 255); // 紫色
      Serial.println(F("Magenta LED ON for High temperature and Low humidity"));
    } else {
      setLED(255, 0, 0); // 红色
      Serial.println(F("Red LED ON for High temperature and Normal humidity"));
    }
    delay(3000);
   // 发送状态到串口
      Serial.println(status);
      tftSerial.println(status);
    }

    // 检测红外传感器
    int val = analogRead(PIR_sensor);
    if (val > 150) {
      personDetected = 1;  // 假设接收到信号表示检测到人
      Serial.println("Detected person!");
      mySerial.println("Detected person!");  //告知语音模块来人了
      tftSerial.println("Detected person!");
    } else {
      personDetected = 0;  // 未检测到人体
    }

  

  }
  delay(100);  // 给系统一些时间，避免CPU占用过高
  
}

void setLED(int red, int green, int blue) {
  analogWrite(LED_RED, red);
  analogWrite(LED_GREEN, green);
  analogWrite(LED_BLUE, blue);
}