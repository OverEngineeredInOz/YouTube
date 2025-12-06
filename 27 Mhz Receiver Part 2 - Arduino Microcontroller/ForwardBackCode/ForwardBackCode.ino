int numSamples=0;
int samplesRequired = 200;
unsigned long t, t0, t0milli = 0;
byte sensorValue[200];

int interval = 2500;
int prescaler = 5;

unsigned long previousMillis = 0;
float threshold = 2.00;  //this is the crossing point in volts that we are going to use to count the number of cycles to determine the frequency
bool belowThreshold = false;
int countOfCycles = 0; //this is the variable that counts the cycles  based on how many we will either enable the Forward or backward pin


void setup()
{
  //enable PINS for displaing status with LEDs
  pinMode(2,OUTPUT); //Pin for Backward signal
  pinMode(5,OUTPUT); //Pin for Forward signal
  
  //setup the analaog to digital converter using the faster method than using Arduion Digital Read
  //This is worth a read for how it works //http://yaab-arduino.blogspot.com/2015/02/fast-sampling-from-analog-input.html
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

    //work through the full array of the samples and do stuff
    for (long i = 0; i<samplesRequired; i++){
      
      float voltage = sensorValue[i] * (5.0 / 255.0);
      //Code to determine number of cycles
      if (belowThreshold == true) {
        if (voltage >= threshold) {
          //we have crossed from below threshold to above threshold so count as one cycle
          countOfCycles++;
          belowThreshold = false;
        }
      }

      if (voltage < threshold) {
        belowThreshold = true;
      }

      digitalWrite(2, LOW);
      digitalWrite(5, LOW);
      
    }
    numSamples++;
  }
  if(numSamples>samplesRequired){
    
    if (countOfCycles > 2){
      digitalWrite(2, HIGH);
      digitalWrite(5, LOW);
    }

    if (countOfCycles > 4){
      digitalWrite(5, HIGH);
      digitalWrite(2, LOW);
    }
    belowThreshold = false;
    countOfCycles = 0;

    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis > interval) {
        previousMillis = currentMillis;  
        startSampling();
    }
    

  }
}