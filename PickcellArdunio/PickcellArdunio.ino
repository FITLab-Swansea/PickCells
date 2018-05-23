#include <SoftwareSerial.h>
int ldrs[5]; 
int activeSides[5]; 
int sensorValue[5]; 
int sensorMax[5]; 
int sensorMin[5]; 

void setup() {  
  Serial.begin(9600);
  // bluetooth serial
//  mySerial.begin(9600);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  for (int i=0; i<5; i++) {
    while (millis() < 5000*i+1) {
      readLRDValuesRaw();
      if (ldrs[i] > sensorMax[i]) {
        sensorMax[i] = ldrs[i];
      }
      if (ldrs[i] < sensorMin[i]) {
        sensorMin[i] = ldrs[i];
      }
    }
    
  }

}

void debugLDRs() {
    for (int i=0; i<5; i++){
      Serial.print("ldr ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(ldrs[i]);
      delay(300);
    }
}

void readLRDValuesRaw() {
    ldrs[0] = (analogRead(A0));
    ldrs[1] = (analogRead(A1));
    ldrs[2] = (analogRead(A2));
    ldrs[3] = (analogRead(A3));
    ldrs[4] = (analogRead(A4));
}

void readLRDValues() {
    ldrs[0] = (analogRead(A0));
    ldrs[0] = map(ldrs[0], sensorMin[0], sensorMax[0], 0, 255);
    ldrs[0] = constrain(ldrs[0], 0, 255);
    ldrs[1] = (analogRead(A1));
    ldrs[1] = map(ldrs[1], sensorMin[1], sensorMax[1], 0, 255);
    ldrs[1] = constrain(ldrs[1], 0, 255);
    ldrs[2] = (analogRead(A2));
    ldrs[2] = map(ldrs[2], sensorMin[2], sensorMax[2], 0, 255);
    ldrs[2] = constrain(ldrs[2], 0, 255);
    ldrs[3] = (analogRead(A3));
    ldrs[3] = map(ldrs[3], sensorMin[3], sensorMax[3], 0, 255);
    ldrs[3] = constrain(ldrs[3], 0, 255);
    ldrs[4] = (analogRead(A4));
    ldrs[4] = map(ldrs[4], sensorMin[4], sensorMax[4], 0, 255);
    ldrs[4] = constrain(ldrs[4], 0, 255);
}

void checkSides(){
  for (int i=0; i<5; i++){
      if (ldrs[i] < 110 && activeSides[i] == 0) {
          activeSides[i] = 1;
          sendValue(i);
      } else if (ldrs[i] > 110 && activeSides[i] == 1) {
        if (activeSides[i] == 1) {
          activeSides[i] = 0;
          sendValue(i);
        }
      }
  }
}

void sendValue(int cubeId){
    Serial.print('#');
    Serial.print(cubeId);
    Serial.print(":");
    Serial.print(activeSides[cubeId]);
    Serial.print('~'); //used as an end of transmission character - used in app for string length
    Serial.println();
}

void loop() {
    readLRDValues();
//    Serial.println(ldrs[4]);

    // debugLDRs();
    checkSides();
    delay(100);
}
