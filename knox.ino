#include <Adafruit_NeoPixel.h>
#define PIN 9

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

const int knockSensor = A0;
const int knockPin = 3;
const int knockComplete = 2000;
const int maximumKnocks = 20;
const int threshold = 100;
const int knockFadeTime = 150;

int knockReadings[maximumKnocks];
int knockSensorValue = 0;

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.show();
  pinMode(knockPin, OUTPUT);
}

void loop() {
  int read_done = 0;

  resetKnockArray();

  read_done = listen2Knock();

  if (read_done == 1) {
    if (knockReadings[0] > 100) {
      for (int i = 0; i < 2; i++) {
        fadeIn(15, 0);
        fadeOut(15, 0);
      }
      delay(50);
      for (int i = 0; i < 1; i++) {
        fadeIn(15, 1);
        fadeOut(15, 1);
      }
      fadeIn(15, 1);
      playBackKnock();
      fadeOut(15, 1);
    }
    read_done = 0;
  }
}

void setColor(int color) {
  for (int h = 0; h < strip.numPixels(); h++) {
    switch (color) {
      case 0:
        strip.setPixelColor(h, strip.Color(255, 255, 255));
        break;
      case 1:
        strip.setPixelColor(h, strip.Color(255, 0, 0));
        break;
    }
  }
}

void fadeOut(uint8_t wait, int color) {
  uint16_t k, h;
  for (k = 0; k < 256; k++) {
    setColor(color);
    strip.setBrightness(255 - k);
    strip.show();
    delay(wait);
  }
}

void fadeIn(uint8_t wait, int color) {
  uint16_t i, j;
  for (j = 0; j < 256; j++) {
    setColor(color);
    strip.setBrightness(j);
    strip.show();
    delay(wait);
  }
}

int listen2Knock() {

  int startTime = millis();
  int now = millis();
  int currentKnockNumber = 0;

  do {
    knockSensorValue = analogRead(knockSensor);

    if (knockSensorValue >= threshold) {
      now = millis();
      knockReadings[currentKnockNumber] = now - startTime;
      currentKnockNumber ++;
      startTime = now;
      knockDelay();
    }

    now = millis();
  } 
  while ((now - startTime < knockComplete) && (currentKnockNumber < maximumKnocks));
  
  return 1;
}

void playBackKnock() {
  for (int i = 0; i < maximumKnocks; i++ ) {
    if (knockReadings[i] > 100) {
      knock();
      delay(knockReadings[i]);
    }
  }
  delay(500);
}

void resetKnockArray() {
  for (int i = 0; i < maximumKnocks; i++) {
    knockReadings[i] = 0;
  }
}

void knock() {
  digitalWrite(knockPin, HIGH);
  delay(10);
  digitalWrite(knockPin, LOW);
  delay(15);
}

void knockDelay() {
  int itterations = (knockFadeTime / 20);
  for (int i = 0; i < itterations; i++) {
    delay(10);
    analogRead(knockSensor);
    delay(10);
  }
}


