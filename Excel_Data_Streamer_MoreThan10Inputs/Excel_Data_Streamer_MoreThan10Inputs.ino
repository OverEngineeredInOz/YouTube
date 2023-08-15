void setup() {
  // put your setup code here, to run once:
  pinMode(52,INPUT);
  pinMode(53,INPUT);
  pinMode(51,INPUT);
  pinMode(49,INPUT);
  pinMode(47,INPUT);
  pinMode(45,INPUT);
  pinMode(43,INPUT);
  pinMode(41,INPUT);
  
  pinMode(38,INPUT);
  pinMode(36,INPUT);
  pinMode(34,INPUT);
  pinMode(32,INPUT);
  pinMode(30,INPUT);
  pinMode(28,INPUT);
  pinMode(26,INPUT);
  pinMode(24,INPUT); 

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(600);
  Serial.print(digitalRead(52)); Serial.print("#"); 
  Serial.print(digitalRead(53)); Serial.print("#");
  Serial.print(digitalRead(51)); Serial.print("#");
  Serial.print(digitalRead(49)); Serial.print("#");
  Serial.print(digitalRead(47)); Serial.print("#");
  Serial.print(digitalRead(45)); Serial.print("#");
  Serial.print(digitalRead(43)); Serial.print("#");
  Serial.print(digitalRead(41)); Serial.print("#");

  Serial.print(digitalRead(38)); Serial.print("#");
  Serial.print(digitalRead(36)); Serial.print("#");
  Serial.print(digitalRead(34)); Serial.print("#");
  Serial.print(digitalRead(32)); Serial.print("#");
  Serial.print(digitalRead(30)); Serial.print("#");
  Serial.print(digitalRead(28)); Serial.print("#");
  Serial.print(digitalRead(26)); Serial.print("#");
  Serial.print(digitalRead(24)); Serial.print("#");

  Serial.print(analogRead(A15)); Serial.print("#");
  Serial.print(analogRead(A14)); Serial.print("#");
  Serial.println();
  
  
}
