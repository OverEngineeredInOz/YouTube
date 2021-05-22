/*
   OscilloscopeExcel

  Reads an analog input on pin A0, converts it to voltage, and prints the result to the Serial Monitor in the format shown below ( with ":" as separator).
  Start:<StartTime>:<voltage>:End:<EndTime>
  where 
    <StartTime> is the time we began reading from the voltage at A0 measured in time microsoeconds since arduino the time the Arduino was started
    <FinishTime> is the time we finished reading from the voltage at A0 measured in time microsoeconds since arduino the time the Arduino was started
    <voltage> is the voltage reading at pin A0 (value between 0 and 5V)
  Example output: 
    Start:12528:2.87:End:13804
*/

int runcount = 0;
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  
}

// the loop routine runs over and over again forever:
void loop() {
  if ( runcount < 1000 )
  {
    Serial.print("Start:");
    Serial.print(micros());
    Serial.print(":");
    // read the input on analog pin A0:
    int sensorValue = analogRead(A0);
    // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
    float voltage = sensorValue * (5.0 / 1023.0);
  
  // print out the value you read:
    Serial.print(voltage);
    Serial.print(":End:");
    Serial.println(micros());
    runcount++;
  }
}
