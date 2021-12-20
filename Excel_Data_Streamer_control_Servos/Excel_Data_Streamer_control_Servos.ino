#include <Servo.h>
Servo myservo;  // create servo object to control a servo
Servo myservo2;

const int len = 60;
char my_str[len];
int pos = 0;
int incomingByte = 0; // for incoming serial data

int spos = 0;    // variable to store the servo position
int spos2 = 0;

void setup() {
  Serial.begin(38400);    // configure serial coms baud rate
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);

  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  myservo2.attach(10);
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

  digitalWrite(2,LOW);
  digitalWrite(3,LOW);
  digitalWrite(4,LOW);
  digitalWrite(5,LOW);

  //param = strtok(my_str,",\n");   // strtok explanation at : https://www.cplusplus.com/reference/cstring/strtok/
  param = strtoke(my_str,",\n");   // strtoke explanation at : https://stackoverflow.com/questions/42315585/split-string-into-tokens-in-c-when-there-are-2-delimiters-in-a-row

  while (param != NULL)
  {
    Serial.print("param");
    Serial.print(paramNum);
    Serial.print(": ");
    Serial.println(param);
    processParam(param, paramNum);
    //finally get the next param from the comma separated my_str if there is one.
    //param = strtok(NULL, ",\n");
    param = strtoke(NULL, ",\n");
    paramNum++;
  }
  myservo.write(spos);
  myservo2.write(spos2);

}

void processParam(char * param, int paramNum){
  char key[] = "true"; //https://www.cplusplus.com/reference/cstring/strcmp/
   if (strcmp (key,param) == 0) {        
        digitalWrite(paramNum + 1, HIGH);
    }
   if (paramNum == 1){
      spos = atoi(param);
   }
   if (paramNum == 2){
      spos2 = atoi(param);
   } 
}

/* behaves like strtok() except that it returns empty tokens also
 */
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

void loop() {
    //Do Nothing
}
