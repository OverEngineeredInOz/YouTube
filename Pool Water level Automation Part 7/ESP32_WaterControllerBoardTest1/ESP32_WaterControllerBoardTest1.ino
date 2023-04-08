//input/output pins
#define IN_LOW 22
#define IN_MEDIUM 19
#define IN_HIGH 17
#define IN_HIGHEST 16
#define OUT_RELAY1 26 //WORKS:25,26,27,33,32,14,13,12   NOT WORKING:35,34
#define OUT_RELAY2 27
#define OUT_WIFI_LED 33

int thresholdConsecutiveCount = 40;
int mediumConsecutiveHighCount = 0;
int mediumConsecutiveLowCount = 0;
void setup() {
  // put your setup code here, to run once:
  pinMode(IN_LOW, INPUT_PULLUP);
  pinMode(IN_MEDIUM, INPUT_PULLUP);
  pinMode(IN_HIGH, INPUT_PULLUP);
  pinMode(IN_HIGHEST, INPUT_PULLUP); 
  pinMode(OUT_RELAY1, OUTPUT);
  pinMode(OUT_RELAY2, OUTPUT);
  pinMode(OUT_WIFI_LED, OUTPUT);
  
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  delay(100);
  Serial.print(digitalRead(IN_LOW));
  digitalWrite(OUT_RELAY2,HIGH);  
  
  //delay(1000);
  Serial.print(digitalRead(IN_MEDIUM));
  digitalWrite(OUT_RELAY2,LOW);  
  //delay(1000);
  Serial.print(digitalRead(IN_HIGH));
  digitalWrite(OUT_WIFI_LED,HIGH);  

  //delay(1000);
  Serial.print(digitalRead(IN_HIGHEST));
  digitalWrite(OUT_WIFI_LED,LOW); 
  
  
  if (digitalRead(IN_MEDIUM)==HIGH){  //Water has reached the turn off height
    mediumConsecutiveLowCount = 0;
    if (mediumConsecutiveHighCount < thresholdConsecutiveCount) {
      mediumConsecutiveHighCount++;    
    } else {
      digitalWrite(OUT_RELAY1,HIGH);    
      Serial.println("Relay High");
    }
  }else {
    mediumConsecutiveHighCount = 0;
    if (mediumConsecutiveLowCount < thresholdConsecutiveCount) {
      mediumConsecutiveLowCount++;
    } else{
      digitalWrite(OUT_RELAY1,LOW);
      Serial.println("Relay Low");
    }
  }
}
