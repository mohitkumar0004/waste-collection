#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <ESP32Servo.h>

#define trig 5
#define echo 18
#define servo 19
#define ir 23
#define piezo 34
#define mq135 32
#define mq4 33
#define motor 25

Servo myservo;
TinyGPSPlus gps;

HardwareSerial GPS(1);
HardwareSerial GSM(2);

void setup() {
  Serial.begin(115200);

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(ir, INPUT);
  pinMode(piezo, INPUT);
  pinMode(motor, OUTPUT);

  myservo.attach(servo);
  myservo.write(0);

  GPS.begin(9600, SERIAL_8N1, 16, 17);
  GSM.begin(9600, SERIAL_8N1, 26, 27);

  GSM.println("AT");
  delay(1000);
  GSM.println("AT+CMGF=1");
}

void loop() {

  while (GPS.available()) {
    gps.encode(GPS.read());
  }

  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long time = pulseIn(echo, HIGH);
  int distance = time * 0.034 / 2;

  if (distance < 20) {
    myservo.write(90);
  } else {
    myservo.write(0);
  }

  if (digitalRead(ir) == LOW) {

    GSM.println("AT+CMGS=\"+91XXXXXXXXXX\"");
    delay(500);

    GSM.print("Garbage detected ");

    if (gps.location.isValid()) {
      GSM.print("Location: ");
      GSM.print("https://maps.google.com/?q=");
      GSM.print(gps.location.lat(), 6);
      GSM.print(",");
      GSM.print(gps.location.lng(), 6);
    }

    GSM.write(26);
    delay(5000);
  }

  int p = analogRead(piezo);
  int g1 = analogRead(mq135);
  int g2 = analogRead(mq4);

  Serial.print("Piezo: ");
  Serial.print(p);
  Serial.print(" MQ135: ");
  Serial.print(g1);
  Serial.print(" MQ4: ");
  Serial.println(g2);

  if (p > 1000 && (g1 > 1500 || g2 > 1500)) {
    digitalWrite(motor, HIGH);
    delay(1000);
    digitalWrite(motor, LOW);
  }

  delay(200);
}
