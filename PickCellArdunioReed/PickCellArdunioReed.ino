 #include <SoftwareSerial.h>

int reeds[4]; 
int activeSides[5]; 
int sensorValue[5]; 

int bottomLDR;
int sensorMax; 
int sensorMin;

void setup() {  
  Serial.begin(9600);

  pinMode(A0, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  
  while (millis() < 5000) {
    bottomLDR = analogRead(A0);
    if (bottomLDR > sensorMax) {
      sensorMax = bottomLDR;
    }
    if (bottomLDR < sensorMin) {
      sensorMin = bottomLDR;
    }
  }
}

void checkSides(){
  for (int i=0; i<4; i++){
      if (reeds[i] == 0 && activeSides[i] == 0) {
          activeSides[i] = 1;
          sendValue(i);
      } else if (reeds[i] == 1 && activeSides[i] == 1) {
         activeSides[i] = 0;
         sendValue(i);
      }
  }
}

int checkBottom(){
  if (bottomLDR < 90 && activeSides[4] == 0) {
    activeSides[4] = 1;
    sendValue(4);
  } else if (bottomLDR > 90 && activeSides[4] == 1) {
    if (activeSides[4] == 1) {
      activeSides[4] = 0;
      sendValue(4);
    }
  }
}

void sendValue(int side){
    Serial.print('#');
    Serial.print(side);
    Serial.print(":");
    Serial.print(activeSides[side]);
    Serial.print('~');
    Serial.println();
}

void readReeds(){
  reeds[0] = digitalRead(2);
  reeds[1] = digitalRead(3);
  reeds[2] = digitalRead(4);
  reeds[3] = digitalRead(5);
}

void printReeds(){
  for (int i=0; i<4; i++){
    Serial.print("REED: ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(reeds[i]);
    delay(300);
  }
}

void readLRD(){
  bottomLDR = (analogRead(A0));
  bottomLDR = map(bottomLDR, sensorMin, sensorMax, 0, 255);
  bottomLDR = constrain(bottomLDR, 0, 255);
}

void loop() {
    readReeds();
    readLRD();
    checkSides();
    checkBottom();
    delay(100);
}
