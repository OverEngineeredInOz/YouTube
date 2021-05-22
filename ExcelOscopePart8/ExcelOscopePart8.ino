//http://yaab-arduino.blogspot.com/2015/02/fast-sampling-from-analog-input.html
int numSamples=0;
int samplesRequired = 400;
long t, t0 = 0;
byte sensorValue[1000];

void setup()
{
  Serial.begin(38400);    // configure serial coms baud rate
  ADCSRA = 0;             // clear ADCSRA register
  ADCSRB = 0;             // clear ADCSRB register
  ADMUX |= (0 & 0x07);    // set A0 analog input pin
  ADMUX |= (1 << REFS0);  // set reference voltage
  ADMUX |= (1 << ADLAR);  // left align ADC value to 8 bits from ADCH register

  // sampling rate is [ADC clock] / [prescaler] / [conversion clock cycles]
  // for Arduino Uno ADC clock is 16 MHz and a conversion takes 13 clock cycles
  //ADCSRA |= (1 << ADPS1) | (1 << ADPS0);    // 8 prescaler for 153.8 KHz
  ADCSRA |= (1 << ADPS1);     // 4 prescaler for 307 KHz  <- not quite getting to this due to code that has to run between each sample but this is the best setting.
  //ADCSRA |= (1 << ADPS0);     // 4 prescaler for 615 KHz  <-this is not achievable due to other code that needs to run

  ADCSRA |= (1 << ADATE); // enable auto trigger
  ADCSRA |= (1 << ADIE);  // enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN);  // enable ADC
  ADCSRA |= (1 << ADSC);  // start ADC measurements
  t0 = micros();  //record time we started sampling
}

ISR(ADC_vect) //This is the interupt code.  It gets called everytime the analog to digital converter finishes a sample.
{
  sensorValue[numSamples] = ADCH;  // read 8 bit value from ADC into our array of samples
  numSamples++;  
  if (numSamples > samplesRequired-1){
    ADCSRA = 0;             // clear ADCSRA register to stop the analog to digital converter now.
  }
}
  
void loop()
{   
  if (numSamples==samplesRequired)
  {
    t = micros()-t0;  // calculate elapsed time
    t=t * 1000; //convert microseconds to nanoseconds so we don't lose decimal places / rounding issues when we divide by the number of samples.
    int samplelength = (t/samplesRequired);

    for (long i = 0; i<samplesRequired; i++){
      Serial.print("Start:");
      Serial.print((i * samplelength) /1000 );  //convert back to microseconds and get integer part
      Serial.print(".");                        //decimal point
      Serial.print( (i * samplelength) % 1000); //decimal part
      Serial.print(":");
      float voltage = sensorValue[i] * (5.0 / 255.0);
      Serial.print(voltage);
      Serial.println(":a:a:");
    }
    numSamples++;
  }
}
