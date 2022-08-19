#define TotalSamples 200
int inputPin = 14;
int runcount = 0;
unsigned int sampleTime[TotalSamples] = {};
int sensorValue[TotalSamples]={};

void setup() {
  Serial.begin(38400);  
  pinMode(inputPin, INPUT);
  pinMode(17, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(17)==LOW){
    doSamples();
    outputResult();
  }
}

void doSamples() {
  for (int runcount = 0; runcount < TotalSamples; runcount++){
    sampleTime[runcount]=micros();
    sensorValue[runcount]=analogRead(inputPin);
  }
}

void outputResult(){
  for (int i = 0; i < TotalSamples; i++)
    {
      //Serial.print("Start,");      //<------use comma
      Serial.print(sampleTime[i]-sampleTime[0]);
      Serial.print(",");          //<------use comma
      float voltage = sensorValue[i] * (3.3 / 4096.0);
      Serial.print(voltage);
      Serial.println(",");        //<------use comma
    }  
}
