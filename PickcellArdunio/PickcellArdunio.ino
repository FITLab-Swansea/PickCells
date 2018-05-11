#include <SoftwareSerial.h>

//SoftwareSerial mySerial(0,1); // RX, TX

#define MESSAGE_BYTES  1  // the total bytes in a message
int ldrs[4]; 

void setup() {  
  Serial.begin(9600);
  // bluetooth serial
//  mySerial.begin(9600);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
}

void debugLDRs(){
    for (int i=0; i<4; i++){
      Serial.print("ldr ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(ldrs[i]);
      delay(100);
    }
}

void loop() {
    ldrs[0] = (analogRead(A0));
    ldrs[1] = (analogRead(A1));
    ldrs[2] = (analogRead(A2));
    ldrs[3] = (analogRead(A3));
    // debugLDRs();
    for (int i=0; i<4; i++){
      if (ldrs[i] < 5){
        Serial.print('#');
        Serial.print(i);
        Serial.print('~'); //used as an end of transmission character - used in app for string length
        Serial.println();
      }
    }

    
    delay(50);
}
