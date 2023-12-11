void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Hello World");
}

char aChar;

void loop() {
  // put your main code here, to run repeatedly:
while (Serial.available() > 0) {
    aChar = Serial.read();
    Serial.print(aChar);
    // look for the newline. That's the end of your sentence:
    if (aChar == 10 || aChar == 13) {
      Serial.println(" Was Received");
    }
  }
}
