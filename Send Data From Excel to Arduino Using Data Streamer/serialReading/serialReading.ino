const int len = 60;
char my_str[len];
char pos = 0;

int incomingByte = 0; // for incoming serial data
void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);    // configure serial coms baud rate
  pinMode(13, OUTPUT);    // sets the digital pin 13 (LED PIN) as output
}

void loop() {
  // put your main code here, to run repeatedly:
}
void serialEvent() {
  //statements
  outputStuff();
}

void outputStuff(){
// send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

      my_str[pos] = incomingByte;
      pos++;  

    if(incomingByte == 10){                   //10 is line feed character
      pos = 0;

      //Turn on pin 13 LED if first received character is the letter "t"
      if (my_str[0] == 't'){
        Serial.print("t,HIGH,");
          digitalWrite(13,HIGH);
      } else {
        digitalWrite(13,LOW);
        Serial.print("not t,LOW,");
      }
      Serial.print(my_str);
      //clear the string for next time
      for (int i = 0; i<=len-1; i++){
        my_str[i] = 0; 
 
      }
    }    
  }
}
