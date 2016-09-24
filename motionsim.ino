#define PIN_IN_POT1  A0
#define PIN_TX_MOT1  11
#define PIN_IN_POT2  A1
#define PIN_TX_MOT2  12

#define P_VALUE 2
#define MIN_SPEED 10
#define MAX_SPEED 30
#define KEVIN_FAILSAFE_FACTOR 100

#include <SoftwareSerial.h>
#include <SabertoothSimplified.h>

SoftwareSerial SWSerial(NOT_A_PIN, PIN_TX_MOT1);
SabertoothSimplified ST(SWSerial);

int mot1_soll = 0;
int mot2_soll = 0;
boolean failsafe = false;

void setup() {
  pinMode(PIN_IN_POT1, INPUT);
  SWSerial.begin(9600);
  Serial.begin(115200);
  randomSeed(analogRead(0));
}

void loop() {
  int mot1_ist = 0;
  int mot1_speed = 0;
  int mot2_ist = 0;
  int mot2_speed = 0;

  if(failsafe) {
    ST.motor(1, 0);
    ST.motor(2, 0);
    delay(500);
    return;
  }
  
  mot1_ist = (int) (analogRead(PIN_IN_POT1) - 39 - 471);
  mot1_speed = (int) ((mot1_soll - mot1_ist) / P_VALUE);
  mot2_ist = (int) (analogRead(PIN_IN_POT2) - 39 - 471);
  mot2_speed = (int) -1 * ((mot2_soll - mot2_ist) / P_VALUE);

  static int mot1_ist_last = mot1_ist;
  static int mot2_ist_last = mot2_ist;

  if ((abs(mot1_ist - mot1_ist_last) > KEVIN_FAILSAFE_FACTOR) or
     ((abs(mot2_ist - mot2_ist_last) > KEVIN_FAILSAFE_FACTOR))) {
    failsafe = true;
    Serial.println("\nFAILSAFE ACTIVE!");
      
    Serial.print("mot1_ist: ");
    Serial.print(mot1_ist);
    Serial.print(", mot1_ist_last: ");
    Serial.print(mot1_ist_last);
    
    Serial.print(", mot2_ist: ");
    Serial.print(mot2_ist);
    Serial.print(", mot2_ist_last: ");
    Serial.println(mot2_ist_last);
    
    ST.motor(1, 0);
    ST.motor(2, 0);
    return;
  }

  mot1_ist_last = mot1_ist;
  mot2_ist_last = mot2_ist;
  
  if (mot1_speed > MAX_SPEED) {
    mot1_speed = MAX_SPEED;
  } else if (mot1_speed < -MAX_SPEED) {
    mot1_speed = -MAX_SPEED;
  }
  if (mot2_speed > MAX_SPEED) {
    mot2_speed = MAX_SPEED;
  } else if (mot2_speed < -MAX_SPEED) {
    mot2_speed = -MAX_SPEED;
  }

  if (mot1_speed > 0 and mot1_speed < MIN_SPEED) {
    mot1_speed = MIN_SPEED;
  } else if (mot1_speed < 0 and mot1_speed > -MIN_SPEED) {
    mot1_speed = -MIN_SPEED;
  }
  if (mot2_speed > 0 and mot2_speed < MIN_SPEED) {
    mot2_speed = MIN_SPEED;
  } else if (mot2_speed < 0 and mot2_speed > -MIN_SPEED) {
    mot2_speed = -MIN_SPEED;
  }

  if ((abs(mot1_soll - mot1_ist) <= 5) and (abs(mot2_soll - mot2_ist) <= 5)) {
    ST.motor(1, 0); 
    mot1_soll = random(-300, 300);
    //mot2_soll = -mot1_soll;
    mot2_soll = random(-300, 300);
  }

  Serial.print("mot1_soll: ");
  Serial.print(mot1_soll);
  Serial.print(", mot1_ist: ");
  Serial.print(mot1_ist);
  Serial.print(", mot1_speed: ");
  Serial.print(mot1_speed);
  Serial.print(", mot2_soll: ");
  Serial.print(mot2_soll);
  Serial.print(", mot2_ist: ");
  Serial.print(mot2_ist);
  Serial.print(", mot2_speed: ");
  Serial.println(mot2_speed);

  if(!failsafe) {
    ST.motor(1, mot1_speed);
    ST.motor(2, mot2_speed);
  }
  
  delay(20);
}
