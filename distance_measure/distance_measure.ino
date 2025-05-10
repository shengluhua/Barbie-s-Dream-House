#include <Servo.h>

Servo servo;

int trigPin = 8;
int echoPin = 10;

long duration, distance;

void setup() {
  Serial.begin(9600); // 初始化串口通信
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  servo.attach(9); // 将伺服电机连接到9号引脚
}

void loop() {
  // 伺服电机从0度转动到180度
  for (int angle = 0; angle <= 180; angle += 30) {
    servo.write(angle);
    delay(500); // 等待伺服电机稳定

    // 测量距离
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;

    // 输出距离信息
    Serial.print("Angle: ");
    Serial.print(angle);
    Serial.print(" degrees, Measured distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    delay(500); // 等待0.5秒
  }

  // 伺服电机从180度转动回0度
  for (int angle = 180; angle >= 0; angle -= 30) {
    servo.write(angle);
    delay(500); // 等待伺服电机稳定

    // 测量距离
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;

    // 输出距离信息
    Serial.print("Angle: ");
    Serial.print(angle);
    Serial.print(" degrees, Measured distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    delay(500); // 等待0.5秒
  }
}