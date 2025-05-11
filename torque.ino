#include <Arduino.h>
#include <U8g2lib.h>
#include "HX711.h"

// Button
const int buttonPin = 10;
int buttonState = 0;

// Motor
const int stepPin = 8;
const int sleepPin = 9;

// Display
U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Scale
HX711 scale;
const int dataPin = 4;
const int clockPin = 5;

// Other
float torque;
int cnt;

void echo(String line0 = "", String line1 = "", String line2 = "");

void setup() {
  Serial.begin(57600);
  initMotor();
  initDisplay();
  echo("Warming", "up...");
  initScale();
}

void loop() {
  intro();
  process();
}

void intro() {
  cnt = 0;
  echo("Press", "button", "to lock");
  while(!digitalRead(buttonPin));
  digitalWrite(sleepPin, HIGH);
  echo();
  delay(1000);
  echo("Press", "button", "to start");
  while(!digitalRead(buttonPin));
  echo();
  delay(1000);
  echo("", "   Three");
  delay(750);
  echo("", "    Two");
  delay(750);
  echo("", "    One");
  delay(750);
  echo("", "   Start!");
  delay(1000);
}

void process() {
  while (1) {
    if (cnt < 2000) {
      makeEvolution();
    } else {
      digitalWrite(sleepPin, LOW);
      while(1);
    }

    torque = scale.get_units();
    if (torque < 0) {
      torque = 0;
    }
    echo("cnt:" + String(cnt), "trq:" + String(torque, 2));
    Serial.print(cnt);
    Serial.print(";");
    Serial.println(round(torque * 100));

    if (digitalRead(buttonPin)) {
      delay(3000);
      digitalWrite(sleepPin, LOW);
      Serial.println("");
      echo ("Process", "stopped");
      delay(1000);
      return;
    }
    cnt++;
  }
}

void initMotor() {
  pinMode(stepPin, OUTPUT);
  pinMode(sleepPin, OUTPUT);
  digitalWrite(sleepPin, LOW);
}

void initDisplay() {
  u8g2.begin();
  u8g2.setFont(u8g2_font_chargen_92_mr);
}

void initScale() {
  scale.begin(dataPin, clockPin);
  scale.tare();
  scale.set_scale(-7492.035156);
}

void makeEvolution() {
  for(int x = 0; x < 200; x++) {
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(375);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(375);
  }
}

void echo(String line0, String line1, String line2) {
  u8g2.firstPage();
  do {
    u8g2.setCursor(0, 16);
    u8g2.print(line0);
    u8g2.setCursor(0, 32);
    u8g2.print(line1);
    u8g2.setCursor(0, 48);
    u8g2.print(line2);
  } while (u8g2.nextPage());
}
