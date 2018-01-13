const int NONE = 0;
const int FIST = 1;
const int FIST_INDEX_OUT = 2;

int state = NONE;

const int numFlexes = 2;

const int INDEX_FINGER = 0;
const int MIDDLE_FINGER = 1;

const int angleBufferLen = 20;
float angle[2][angleBufferLen];
float middleAngle[angleBufferLen];

const int FLEX_PIN[] = {A11, A7};

const float VCC = 3.3;
const float R_DIV = 51000;

const float STRAIGHT_RESISTANCE[] = {12700, 11300};
const float BEND_RESISTANCE[] = {24000, 22000};

const int LOOP_DELAY = 50;

void setup() 
{
  Serial.begin(9600);
  Serial1.begin(9600);
  for (int i = 0; i < numFlexes; i++) {
    pinMode(FLEX_PIN[i], INPUT);
  }
}

void loop() 
{
  for (int i = 0; i < numFlexes; i++) {
    int flexADC = analogRead(FLEX_PIN[i]);
    float flexV = flexADC * VCC / 1023.0;
    float flexR = ((R_DIV * VCC) / flexV) - R_DIV;
    Serial.println("Resistance " + String(i+1) + ": " + String(flexR) + " ohms");
   
    float currentAngle = map(flexR, STRAIGHT_RESISTANCE[i], BEND_RESISTANCE[i],
                     0, 90.0);
                     
    for (int j = angleBufferLen-1; j >= 1; j--) {
      angle[i][j] = angle[i][j-1];
    }
    angle[i][0] = currentAngle;
    
    Serial.println("Bend " + String(i+1) + ": " + String(currentAngle) + " degrees");
    Serial1.print("<dim100>");

    Serial.println();
  }

  Serial.println(degreesPerSecond(angle[MIDDLE_FINGER], 250));
  
  Serial.println();
  delay(LOOP_DELAY);
}

float degreesPerSecond(float angle[angleBufferLen], int millisHistory) {
  int oldIndex = millisHistory/LOOP_DELAY;
  return (angle[0] - angle[oldIndex])/((oldIndex*LOOP_DELAY)/1000.0);
}



