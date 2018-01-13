const int numFlexes = 2;

const int FLEX_PIN[] = {A11, A7};

const float VCC = 3.3;
const float R_DIV = 51000;

const float STRAIGHT_RESISTANCE[] = {12700, 11300};
const float BEND_RESISTANCE[] = {24000, 22000};

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
   
    float angle = map(flexR, STRAIGHT_RESISTANCE[i], BEND_RESISTANCE[i],
                     0, 90.0);
    
    Serial.println("Bend " + String(i+1) + ": " + String(angle) + " degrees");
    Serial1.println("Bend " + String(i+1) + ": " + String(angle) + " degrees");

    Serial.println();
  }

  Serial.println();
  Serial1.println();
  delay(500);
}

