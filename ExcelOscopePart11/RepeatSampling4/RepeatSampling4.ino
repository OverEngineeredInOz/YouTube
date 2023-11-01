//http://yaab-arduino.blogspot.com/2015/02/fast-sampling-from-analog-input.html
int numSamples=0;
int samplesRequired = 200;
unsigned long t, t0, t0milli = 0;
byte sensorValue[1000];

const int len = 60;
char my_str[len];
int pos = 0;
int incomingByte = 0; // for incoming serial data
int interval = 2500;
int prescaler = 5;

unsigned long previousMillis = 0;

void setup()
{
  Serial.begin(38400);    // configure serial coms baud rate
  ADCSRA = 0;             // clear ADCSRA register
  ADCSRB = 0;             // clear ADCSRB register
  ADMUX |= (0 & 0x07);    // set A0 analog input pin
  ADMUX |= (1 << REFS0);  // set reference voltage
  ADMUX |= (1 << ADLAR);  // left align ADC value to 8 bits from ADCH register

  startSampling();
}

ISR(ADC_vect) //This is the interupt code.  It gets called everytime the analog to digital converter finishes a sample.
{
  sensorValue[numSamples] = ADCH;  // read 8 bit value from ADC into our array of samples
  numSamples++;  
  if (numSamples > samplesRequired-1){
    ADCSRA = 0;             // clear ADCSRA register to stop the analog to digital converter now.
  }
}

void startSampling(){
    // sampling rate is [ADC clock] / [prescaler] / [conversion clock cycles]
  // for Arduino Uno ADC clock is 16 MHz and a conversion takes 13 clock cycles
  //ADCSRA = (ADCSRA & B11111000) | 7; // 60Hz to 300Hz   //set division factor to the default arduino value of 128
  //ADCSRA = (ADCSRA & B11111000) | 6; //100Hz to 500Hz   //set division factor to 64 instead of the default arduino value of 128 
  //ADCSRA = (ADCSRA & B11111000) | 5;  //160Hz to 950Hz   //set division factor to 32 instead of the default arduino value of 128
  //ADCSRA = (ADCSRA & B11111000) | 4;  //250Hz to 4000Hz  //set division factor to 16 instead of the default arduino value of 128 
  //ADCSRA = (ADCSRA & B11111000) | 3;                    //set division factor to 8 instead of the default arduino value of 128 
  //ADCSRA = (ADCSRA & B11111000) | 2;                    //set division factor to 4 instead of the default arduino value of 128 
  ADCSRA = (ADCSRA & B11111000) | prescaler;

  ADCSRA |= (1 << ADATE); // enable auto trigger
  ADCSRA |= (1 << ADIE);  // enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN);  // enable ADC

  t0milli = millis();  //for slower sampling speeds we measure in milliseconds to prevent overflow.
  ADCSRA |= (1 << ADSC);  // start ADC measurements
  t0 = micros();  //record time we started sampling
  numSamples = 0;
}

void serialEvent() {
// send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

      my_str[pos] = incomingByte;
      pos++;  

    if(incomingByte == 10){                   //10 is line feed character
      splitCommaSeparated();

      //clear the string for next time
      for (int i = 0; i<=len-1; i++){
        my_str[i] = 0; 
      }
      
      pos = 0;
    }    
  }
}

void splitCommaSeparated(){
  //splits a comma separated char array into separated char arrays for each comma separated value
  char * param;
  int paramNum = 1;

  //param = strtok(my_str,",\n");   // strtok explanation at : https://www.cplusplus.com/reference/cstring/strtok/
  param = strtoke(my_str,",\n");   // strtoke explanation at : https://stackoverflow.com/questions/42315585/split-string-into-tokens-in-c-when-there-are-2-delimiters-in-a-row

  while (param != NULL)
  {

    processParam(param, paramNum);
    //finally get the next param from the comma separated my_str if there is one.
    //param = strtok(NULL, ",\n");
    param = strtoke(NULL, ",\n");
    paramNum++;
  }

}

void processParam(char * param, int paramNum){
/*  char key[] = "true"; //https://www.cplusplus.com/reference/cstring/strcmp/
   if (strcmp (key,param) == 0) {        
        digitalWrite(paramNum + 1, HIGH);
    } */
   if (paramNum == 1){
      interval = atoi(param);
   }
   if (paramNum == 2){
      prescaler = atoi(param);
   } 
}

char* strtoke(char *str, const char *delim)
{
  static char *start = NULL; /* stores string str for consecutive calls */
  char *token = NULL; /* found token */
  /* assign new start in case */
  if (str) start = str;
  /* check whether text to parse left */
  if (!start) return NULL;
  /* remember current start as found token */
  token = start;
  /* find next occurrence of delim */
  start = strpbrk(start, delim);
  /* replace delim with terminator and move start to follower */
  if (start) *start++ = '\0';
  /* done */
  return token;
}
  
void loop()
{   
  if (numSamples==samplesRequired)
  {
    t = micros()-t0;  // calculate elapsed time
    
    if (prescaler > 5){ //on slower sampling speeds micros() will overflow so use millis() instead
      t = millis()-t0milli;
      t = t * 1000;  //convert to microsecond equivalent.
    }
    
    t=t * 1000; //convert microseconds to nanoseconds so we don't lose decimal places / rounding issues when we divide by the number of samples.
    long samplelength = (t/samplesRequired);

    for (long i = 0; i<samplesRequired; i++){
      //Serial.print("Start:");
      Serial.print((i * samplelength) /1000 );  //convert back to microseconds and get integer part
      Serial.print(".");                        //decimal point
      Serial.print( (i * samplelength) % 1000); //decimal part
      Serial.print(",");
      float voltage = sensorValue[i] * (5.0 / 255.0);
      Serial.print(voltage);
      Serial.print(",a,a,");
      Serial.print(interval);
      Serial.print(",");
      Serial.print(prescaler);
      Serial.println(",");
    }
    numSamples++;
  }
  if(numSamples>samplesRequired){
     unsigned long currentMillis = millis();
    if(currentMillis - previousMillis > interval) {
        previousMillis = currentMillis;  
        startSampling();
    }
    

  }
}
