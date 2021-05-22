/*
  Oscilloscope Excel 
  Reads an analog input on pin A0, converts it to voltage, and prints the result to the Serial Monitor in the format shown below ( with "," as separator).
  Start,<StartTime>,<voltage>
  where
    <StartTime> is time we began reading from the voltage of A0 measured in microseconds since Arduino was started
    <voltage> is the voltage reading at pin A0 (value between 0 and 5V)
  Example output
    Start,12523,2.87
*/ 
#define TotalSamples 400
int runcount = 0;
unsigned int sampleTime[TotalSamples] = {};
int sensorValue[TotalSamples]={};

//the setup routine runs once when you press the reset:
void setup() {
  // initialize serial communication at 2000000 bits per second
  Serial.begin(38400);  //<--------Default 9600 for Microsoft Excel DataStreamer, 38400 is faster and works

  ADCSRA = (ADCSRA & B11111000) | 4;  //set division factor to 16 instead of the default arduino value of 128
  //ADCSRA = (ADCSRA & B11111000) | 5;  //set division factor to 32 instead of the default arduino value of 128
  //ADCSRA = (ADCSRA & B11111000) | 6;  //set division factor to 64 instead of the default arduino value of 128
  //ADCSRA = (ADCSRA & B11111000) | 7;  //set division factor to the default arduino value of 128
  
}

//the loop routine runs over and over forever:
void loop() {
  if (runcount < TotalSamples)
  {
    sampleTime[runcount]=micros();
    sensorValue[runcount]=analogRead(A0);

    runcount++;
  }
  if (runcount == TotalSamples) {
    for (int i = 0; i < TotalSamples; i++)
    {
      Serial.print("Start,");      //<------use comma
      Serial.print(sampleTime[i]);
      Serial.print(",");          //<------use comma
      float voltage = sensorValue[i] * (5.0 / 1023.0);
      Serial.print(voltage);
      Serial.println(",");        //<------use comma
    }  
    runcount++;
  }
  
}
