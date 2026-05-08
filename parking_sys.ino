#include <LiquidCrystal.h>
#include <Servo.h>

LiquidCrystal lcd(7, 8, 10, 11, 12, 13);
Servo gate;

#define ENTRY  2
#define EXIT   3
#define SLOT1  4
#define SLOT2  5
#define SERVO  9

#define OPEN   90
#define CLOSED 0

bool gateOpen = false;
unsigned long gateTimer = 0;
String oldLine1 = "";
String oldLine2 = "";

void showLCD(String top, String bot) {
  if (top != oldLine1 || bot != oldLine2) {
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print(top);
    lcd.setCursor(0, 1); lcd.print(bot);
    oldLine1 = top;
    oldLine2 = bot;
  }
}

void openGate() {
  gate.write(OPEN);
  gateOpen = true;
  gateTimer = millis();
}

void closeGate() {
  gate.write(CLOSED);
  gateOpen = false;
}


bool carAt(int pin) {
  return digitalRead(pin) == LOW;
}

void setup() {
  pinMode(ENTRY, INPUT_PULLUP);
  pinMode(EXIT,  INPUT_PULLUP);
  pinMode(SLOT1, INPUT_PULLUP);
  pinMode(SLOT2, INPUT_PULLUP);

  lcd.begin(16, 2);
  gate.attach(SERVO);
  closeGate();

  lcd.print("Smart Parking");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  delay(2000);
  lcd.clear();
}

void loop() {
  bool slot1 = carAt(SLOT1);
  bool slot2 = carAt(SLOT2);

  if (gateOpen && millis() - gateTimer >= 3000) {
    closeGate();
    oldLine1 = "";
  }

  if (carAt(EXIT) && !gateOpen) {
    showLCD("Car Leaving", "Updating Slots");
    openGate();
    while (carAt(EXIT)) delay(50);
    return;
  }

  if (carAt(ENTRY) && !gateOpen) {
    if (slot1 && slot2) {
      showLCD("Parking Full", "Gate Closed");
      while (carAt(ENTRY)) delay(50);
      delay(1500);
      oldLine1 = "";
    } else if (!slot1) {
      showLCD("Go to Slot 1", "Gate Opening...");
      openGate();
      while (carAt(ENTRY)) delay(50);
    } else {
      showLCD("Go to Slot 2", "Gate Opening...");
      openGate();
      while (carAt(ENTRY)) delay(50);
    }
    return;
  }

  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= 500) {
    lastUpdate = millis();
    String s1 = slot1 ? "Occ" : "Free";
    String s2 = slot2 ? "Occ" : "Free";
    showLCD("S1:" + s1 + " S2:" + s2, gateOpen ? "Gate Open..." : "Waiting...");
  }
}