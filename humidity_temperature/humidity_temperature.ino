#include "DHT.h" // 加载DHT库

#define DHTPIN 2 // 定义DHT的输入引脚
#define DHTTYPE DHT11 // 定义DHT的型号类型

DHT dht(DHTPIN, DHTTYPE); // 创建一个为dht的DHT实例

void setup() {
  Serial.begin(115200); // 设置波特率
  Serial.println("DHT11 test!"); // 串口输出信息

  dht.begin(); // 启动dht
}

void loop() {
  float humidity = 0; // 定义一个浮点类型的变量存储湿度值
  float temperature = 0; // 定义一个浮点类型的变量存储温度值

  // 读取湿度值
  humidity = dht.readHumidity();
  // 读取温度值
  temperature = dht.readTemperature();

  // 检查读取是否成功
  if (isnan(humidity) || isnan(temperature))
  {
    Serial.println("Failed to get temperature and humidity value.");
  } 
  else 
  {
    Serial.print("Humidity: "); 
    Serial.print(humidity);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(temperature);
    Serial.println(" °C");
  }
  delay(1000); // 等待1秒
}