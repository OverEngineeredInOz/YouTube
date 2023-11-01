bool LED_STATE = true;
volatile byte driveDirection = 0;
volatile byte turn = 0;
volatile byte isrCallCount = 0;

void setup() {
  // put your setup code here, to run once:
pinMode(8, INPUT_PULLUP); //for forward
pinMode(9, INPUT_PULLUP); //for backward
pinMode(10, INPUT_PULLUP); //for left
pinMode(11, INPUT_PULLUP); //for right
Serial.begin(38400);

//REFERENCES:
https://electronoobs.com/eng_arduino_tut140.php

//output pin
pinMode(13, OUTPUT);
  cli();                      //stop interrupts temporarily while we configure the settings
  /*1. First we reset the control registers to amke sure we start with everything disabled.*/
  TCCR1A = 0;                 // Reset entire TCCR1A to 0 
  TCCR1B = 0;                 // Reset entire TCCR1B to 0

  /*2. We set the prescalar to the desired value by changing the CS10 CS12 and CS12 bits. */  
  TCCR1B |= B00000100;        //Set CS12 to 1 so we get prescalar 256 

  /*3. We enable compare match mode on register A*/
  TIMSK1 |= B00000010;        //Set OCIE1A to 1 so we enable compare match A 

  /*4. Set the value of register A to 31250*/
  OCR1A = 31250;             //Finally we set compare register A to this value  
  sei();                     //Enable back the interrupts
}

void loop() {
  // put your main code here, to run repeatedly:
  

  if (digitalRead(8) == 0) {
    driveDirection = 1; //forward
    cli();
    OCR1A = 18;
    sei();
  } 
  else if (digitalRead(9) == 0) {
    driveDirection = 2; //backward
    cli();
    OCR1A = 50;
    sei();
  }
  else {
    driveDirection = 0;
    turn = 0;
    cli();
    LED_STATE = true;
    digitalWrite(13,LED_STATE);  //Write new state to the LED on pin D13 
  }

  //Handle Turning
  if (driveDirection != 0){
    if (digitalRead(10) == 0) {
      turn = 1; //left
    }
    else if (digitalRead(11) == 0) {
      turn = 2; //right
    }
    else {
      turn = 0;
    }
  }
  //Serial.print("driveDirection: ");Serial.println(driveDirection);
  //Serial.print("turn: ");Serial.println(turn);
  //delay(1000);
}


ISR(TIMER1_COMPA_vect){
  TCNT1  = 0;                  //First, set the timer back to 0 so it resets for next interrupt
  isrCallCount++;
  if (isrCallCount == 1) {
      if (turn == 1){
        LED_STATE = false;        
      }
  }
  else if (isrCallCount == 2){
    if (turn == 0){
        LED_STATE = false;        
      }
  }
  else if (isrCallCount == 3){
    if (turn == 2){
      
        LED_STATE = false;        
      }    
  }
  else {
    LED_STATE = true;
    isrCallCount = 0;
  }
  digitalWrite(13,LED_STATE);  //Write new state to the LED on pin D5 

}
