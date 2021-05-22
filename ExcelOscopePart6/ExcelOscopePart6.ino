/*
  Oscilloscope Excel - 2 Channel 
  Reads an analog inputs on pin A0 and A1, converts it to voltage, and prints the result to the Serial Monitor in the format shown below ( with "," as separator).
  <StartTimeCH1>,<VoltageCH1>,<StartTimeCH2>,<VoltageCH2>,<best for freq range text>
  where
    <StartTimeCHx> is time we began reading from the voltage of A0 for CH1 and A1 for CH2 measured in microseconds since Arduino was started
    <voltageCHx> is the voltage reading at pin A0 for CH1 and A1 for CH2 (value between 0 and 5V)
    <best for freq range text> is some text indicating the frequencies the selected sampling rate is best for.  
  Example output
    12523,2.87,12323,1.43,60Hz to 300Hz
  Allows adjusting of the sampling speed by setting one of the digital input pins voltage to 0V using a jumper wire.
  Pins used assume an UNO board.
*/ 
#define TotalSamples 400

int runcount = 0;
unsigned int sampleTime[TotalSamples] = {};
int sensorValue[TotalSamples]={};
char frequencyRngString[16] = "Freq Text";      //<---------New


//the setup routine runs once when you press the reset:
void setup() {
  // initialize serial communication at 38400 bits per second
  Serial.begin(38400);   //Default 9600 for Microsoft Excel DataStreamer, 38400 is faster and works
  
  //enable pull up resistors so these Pins will be pulled to 5 volts unless shorted to 0V Gnd by a jumper wire.
  pinMode(10, INPUT_PULLUP); 
  pinMode(9, INPUT_PULLUP); 
  pinMode(8, INPUT_PULLUP); 
  pinMode(7, INPUT_PULLUP); 
  pinMode(6, INPUT_PULLUP); 
  pinMode(5, INPUT_PULLUP); 
  pinMode(4, INPUT_PULLUP); 

  //Configure ADCSRA -> ADC Prescaler value based on Digital low value detected on any of pins 4,5,6,7 if none then use default value for prescaler.
  ADCSRA = (ADCSRA & B11111000) | 7;
  strncpy(frequencyRngString, "60Hz to 300Hz", 16);   // 60Hz to 300Hz   //set division factor to the default arduino value of 128
                            
  if (digitalRead(6) == 0){ 
    ADCSRA = (ADCSRA & B11111000) | 6;
    strncpy(frequencyRngString, "100Hz to 500Hz", 16);
  }    //100Hz to 500Hz   //set division factor to 64 instead of the default arduino value of 128 
  
  if (digitalRead(5) == 0){ 
    ADCSRA = (ADCSRA & B11111000) | 5;
    strncpy(frequencyRngString, "160Hz to 950Hz", 16);
  }    //160Hz to 950Hz   //set division factor to 32 instead of the default arduino value of 128
  
  if (digitalRead(4) == 0){ 
    ADCSRA = (ADCSRA & B11111000) | 4;  
    strncpy(frequencyRngString, "250Hz to 4000Hz", 16);
  }    //250Hz to 4000Hz  //set division factor to 16 instead of the default arduino value of 128 
}

///////////////////////////////////////////////////////////////////////////SAMPLING PROCEDURES//////////////////////////////////////////////////
void doSamples(){
  //Sampling stage used when no delay is required.  Most useful for measuring frequencies above 60Hz
  if (runcount < TotalSamples)
  {
    //Channel 1 measurement
    sampleTime[runcount]=micros();
    sensorValue[runcount]=analogRead(A0);  //Channel 1
    
    runcount++;
    //Channel 2 measurement
    sampleTime[runcount]=micros();
    sensorValue[runcount]=analogRead(A1); //Channel 2
    runcount++;    
  }
}

void doSamplesWithDelay(int d, byte useMicros){
  //Sampling stage used when a delay is required.  Most useful for measuring frquencies below 60Hz
  if (runcount < TotalSamples)
  {
    //Channel 1 measurement
    sampleTime[runcount]=micros();
    sensorValue[runcount]=analogRead(A0);  //Channel 1
    
    runcount++;
    //Channel 2 measurement
    sampleTime[runcount]=micros();
    sensorValue[runcount]=analogRead(A1); //Channel 2
    runcount++;
    if ( useMicros > 0 )  {
      delayMicroseconds(d);
    } else {
      delay(d);  //**Delay for when we are measuring really slow items 
    }
  }
}


//////////////////////////////////////////////////MAIN LOOP START/////////////////////////////////////////////////////////////////////////
//the loop routine runs over and over forever:
void loop() {  
  ///////////////////////////////////////////////////////////////////////Sampling stage////////////////////////////////////////////////////
  //if any of pins 8,9,10 are digital low then introduce some level of delay in the sampling rate otherwise just sample normally.
  if (runcount < TotalSamples){
    if (digitalRead(8) == 0){
      doSamplesWithDelay(500,1);    //20Hz to 125Hz
      strncpy(frequencyRngString, "20Hz to 125Hz", 16);
    }  
    else if (digitalRead(9) == 0){
      doSamplesWithDelay(4,0); // 3Hz to 20Hz
      strncpy(frequencyRngString, "3Hz to 20Hz", 16);    
    }
    else if (digitalRead(10) == 0){
      doSamplesWithDelay(25,0);  //0.5 Hz to 3Hz
      strncpy(frequencyRngString, "0.5 Hz to 3Hz", 16);  
    }
    else{  //used if any of pins 4,5,6,7 are low.
      doSamples();   
    }    
  }  

  /////////////////////////////////////////////////////////////////////////Output Stage//////////////////////////////////////////////////
  if (runcount == TotalSamples) {
    doOutput();
  }  
} /////////////////////////////////////////////////////end of main program loop//////////////////////////////////////////////////////////

void doOutput(){
    int rollovers = 0;                  //<--------to keep track of each time the Sampletime from micros() overflows
    long timeIncludingRollovers = 0;    //<--------use a long so that we can handle times longer than 65535 microseconds
    for (int i = 0; i < TotalSamples; i++)  
    {
      // handle rollover past the range of an int in the Sampletime
      if (i > 0)  //**prevent index out of bounds must be able to look back one step in the array
      {
         //if current sample time is less than last sample time assume we have overflowed the maximum of 65535 seconds supported by micros() and it has started counting from zero again.
         if ( sampleTime[i] < sampleTime[i-1] ){      
            rollovers++;
         }
      }
      timeIncludingRollovers = sampleTime[i] + (rollovers * 65535); //work out the actual time taking into account rollovers of the clock back to 0
      //Serial.print("Start,");
      //Serial.print(sampleTime[i]);
      Serial.print(timeIncludingRollovers);   //<------Time taking into account rollovers
      Serial.print(",");
      float voltage = sensorValue[i] * (5.0 / 1023.0);
      Serial.print(voltage);
      if ((i % 2) == 0)  //if the remainder of a division by 2 is 1 then we are working with channel 2 otherwise this sample is for channel 1.
      {  
        Serial.print(",");  //Channel 1 don't go to new line
      }
      else
      {
        Serial.print(",");  
        Serial.println(frequencyRngString); //Channel 2 go to a new line after this entry
      }
    }   
    runcount++;
}
