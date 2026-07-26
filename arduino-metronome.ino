#include <LiquidCrystal.h>

const int rs = 12;
const int en = 11;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;

const int buttonPin = 7;
const int buzzerPin = 9;
const int potentiometerPin = A0;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const char *signatures[] = {"4/4", "3/4"};
const byte beatsPerMeasure[] = {4, 3};
const int signatureCount = sizeof(signatures) / sizeof(signatures[0]);

unsigned long lastBeatTime = 0;
int bpm = 40;
bool currentButtonState = HIGH;
byte selectedSignature = 0;
byte currentBeat = 0;

void setup() {
  lcd.begin(16, 2);
  pinMode(buttonPin, INPUT_PULLUP);

  for (int i = 0; i < signatureCount; i++) {
    lcd.print(signatures[i]);
    lcd.print(" ");
  }

  lcd.setCursor(3, 0);
  lcd.write(byte(127));
}

void loop() {
  updateTimeSignature();

  int potentiometerValue = analogRead(potentiometerPin);
  bpm = map(potentiometerValue, 0, 1023, 40, 200);

  updateTone();
  updateDisplay();
}

void updateTimeSignature() {
  bool buttonState = digitalRead(buttonPin);

  if (buttonState != currentButtonState) {
    currentButtonState = buttonState;

    // The selection changes when the button is released.
    if (currentButtonState == HIGH) {
      int previousArrowPosition = 3 + selectedSignature * 4;
      selectedSignature = (selectedSignature + 1) % signatureCount;
      int newArrowPosition = 3 + selectedSignature * 4;

      lcd.setCursor(previousArrowPosition, 0);
      lcd.print(" ");

      lcd.setCursor(newArrowPosition, 0);
      lcd.write(byte(127));

      // Restart the measure when changing the time signature.
      currentBeat = 0;
    }
  }
}

void updateDisplay() {
  lcd.setCursor(0, 1);
  lcd.print(bpm);
  lcd.print(" BPM ");
}

void updateTone() {
  unsigned long currentTime = millis();
  unsigned long beatInterval = 60000UL / bpm;

  if (currentTime - lastBeatTime >= beatInterval) {
    lastBeatTime = currentTime;

    if (currentBeat == 0) {
      tone(buzzerPin, 950, 20);
    } else {
      tone(buzzerPin, 550, 20);
    }

    currentBeat++;

    if (currentBeat >= beatsPerMeasure[selectedSignature]) {
      currentBeat = 0;
    }
  }
}
