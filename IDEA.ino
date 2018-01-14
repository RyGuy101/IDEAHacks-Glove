#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);

const int NONE = 0;
const int FIST_DELAY = 1;
const int BRIGHTNESS_CONTROL = 2;
const int ONE_OPEN = 3;
const int ONE_CLOSE = 4;

int state = NONE;

float refAngle;
byte refBrightness;
int delayCounter;
int toggleCounter;

const int numFlexes = 2;

const int INDEX_FINGER = 0;
const int MIDDLE_FINGER = 1;

const int angleBufferLen = 20;
float angle[2][angleBufferLen];

const int FLEX_PIN[] = {A11, A7};

const float VCC = 3.3;
const float R_DIV = 51000;

const float STRAIGHT_RESISTANCE[] = {12700, 11300};
const float BEND_RESISTANCE[] = {24000, 22000};

const int LOOP_DELAY = 50;

const float FADE_AMOUNT = 1;
byte brightness;

sensors_event_t accelEvent;

void setup() 
{
  Serial.begin(9600);
  Serial1.begin(9600);
  for (int i = 0; i < numFlexes; i++) {
    pinMode(FLEX_PIN[i], INPUT);
  }
  accel.begin();
}

void loop() 
{
  accel.getEvent(&accelEvent);
  
  for (int i = 0; i < numFlexes; i++) 
  {
    int flexADC = analogRead(FLEX_PIN[i]);
    float flexV = flexADC * VCC / 1023.0;
    float flexR = ((R_DIV * VCC) / flexV) - R_DIV;
//    Serial.println("Resistance " + String(i+1) + ": " + String(flexR) + " ohms");
   
    float currentAngle = map(flexR, STRAIGHT_RESISTANCE[i], BEND_RESISTANCE[i], 0, 90.0);
                     
    for (int j = angleBufferLen-1; j >= 1; j--) 
  {
      angle[i][j] = angle[i][j-1];
    }
    angle[i][0] = currentAngle;
    
//    Serial.println("Bend " + String(i+1) + ": " + String(currentAngle) + " degrees");

//    Serial.println();
  }

  float indexAngle = angle[INDEX_FINGER][0];
  float middleAngle = angle[MIDDLE_FINGER][0];
//  Serial.println(degreesPerSecond(angle[MIDDLE_FINGER], 100));

  if (state == ONE_OPEN || state == ONE_CLOSE) {
    if (toggleCounter < 7) {
      toggleCounter++;
    } else {
      state = NONE;
    }
  }
  if (state != ONE_CLOSE && state != ONE_OPEN && degreesPerSecond(angle[MIDDLE_FINGER], 100) < -1250) {
    toggleCounter = 0;
    state = ONE_OPEN;
//    Serial.print("ONE_OPEN");
  } 
  else if (state == ONE_OPEN && degreesPerSecond(angle[MIDDLE_FINGER], 100) > 1250) {
    toggleCounter = 0;
    state = ONE_CLOSE;
//    Serial.print("ONE_CLOSE");
  }
  else if (state == ONE_CLOSE && degreesPerSecond(angle[MIDDLE_FINGER], 100) < -1250) {
//    Serial.print("toggle------------------------------------------------------------");
    brightness = brightness > 0 ? 0 : 127;
    Serial1.write(brightness);
    state = NONE;
  }
  else if (state != ONE_CLOSE && state != ONE_OPEN && middleAngle > 90) { 
    if (state == NONE) {
      delayCounter = 0;
      state = FIST_DELAY;
    }
    if (state == FIST_DELAY) 
    {
      if (delayCounter < 0) {
         delayCounter++;
      } else if (abs(degreesPerSecond(angle[MIDDLE_FINGER], 50)) < 135) {
        refAngle = indexAngle;
        refBrightness = brightness;
        state = BRIGHTNESS_CONTROL;
      }
    }
    if (state == BRIGHTNESS_CONTROL) {
      if (abs(degreesPerSecond(angle[MIDDLE_FINGER], 50)) < 135) {
        brightness = constrain(refBrightness - FADE_AMOUNT*(indexAngle - refAngle), 0, 127);
        if (brightness == 127) {
          refBrightness = brightness;
          refAngle = indexAngle;
        }
        Serial.println("brightness=" + String(brightness));
//        Serial.println("indexAngle=" + String(indexAngle));
        Serial1.write(brightness);
      }
    }
  }
  
  if (middleAngle < 90 && indexAngle > 90 && abs(sqrt(pow(accelEvent.acceleration.x, 2) + pow(accelEvent.acceleration.y, 2) + pow(accelEvent.acceleration.z, 2)) - 9.8) > 2) {
    Serial.println("color");
    Serial1.write(128);
  }
    
//  Serial.println();
  delay(LOOP_DELAY);
}

float degreesPerSecond(float angle[angleBufferLen], int millisHistory) {
  int oldIndex = millisHistory/LOOP_DELAY;
  return (angle[0] - angle[oldIndex])/((oldIndex*LOOP_DELAY)/1000.0);
}
