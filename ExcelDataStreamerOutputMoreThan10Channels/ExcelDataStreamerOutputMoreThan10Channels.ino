int outputPin = 39;

//variables for reading and processing serial data
int incomingByte = 0;
const int len = 60;
char my_str[len];
int pos = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(39, OUTPUT); digitalWrite(39,HIGH);
  pinMode(41, OUTPUT); digitalWrite(41,HIGH);
  pinMode(43, OUTPUT); digitalWrite(43,HIGH);
  pinMode(45, OUTPUT); digitalWrite(45,HIGH);
  pinMode(47, OUTPUT); digitalWrite(47,HIGH);
  pinMode(49, OUTPUT); digitalWrite(49,HIGH);
  pinMode(51, OUTPUT); digitalWrite(51,HIGH);
  pinMode(53, OUTPUT); digitalWrite(53,HIGH);
  pinMode(2, OUTPUT); analogWrite(2,255);
  pinMode(3, OUTPUT); analogWrite(3,128);
  pinMode(4, OUTPUT); analogWrite(4,64);
  pinMode(5, OUTPUT); analogWrite(5,32);
  pinMode(6, OUTPUT); analogWrite(6,16);
  pinMode(7, OUTPUT); analogWrite(7,8);
  pinMode(8, OUTPUT); analogWrite(8,4);
  pinMode(9, OUTPUT); analogWrite(9,3);
  pinMode(10, OUTPUT); analogWrite(10,2);
  pinMode(11, OUTPUT); analogWrite(11,1);

  //initiate comms setting baud rate.  Microsoft Excel Datastreamer needs the same baud rate set
  Serial.begin(9600);
}

void loop() {
 
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

  param = strtoke(my_str,",\n");   // strtoke explanation at : https://stackoverflow.com/questions/42315585/split-string-into-tokens-in-c-when-there-are-2-delimiters-in-a-row

  while (param != NULL)
  {
    //Strip off any Quotation marks that excel puts on the string sent to the Arduino
    char quotationMark = 34;
    removeChar(param, quotationMark );
    
    //Debug print the input / output
    Serial.print("param");
    Serial.print(paramNum);
    Serial.print(": ");
    Serial.println(param);
    processParam(param, paramNum);
    //finally get the next param from the comma separated my_str if there is one.
    param = strtoke(NULL, ",\n");
    paramNum++;
  }
}

void processParam(char * param, int paramNum){
  
  if(paramNum < 11){
    analogWrite(paramNum + 1, atoi(param));
  }
  
  if (paramNum == 11){
      if (atoi(param)>0){digitalWrite(39,HIGH);} else {digitalWrite(39,LOW);}
  }
  if (paramNum == 12){
      if (atoi(param)>0){digitalWrite(41,HIGH);} else {digitalWrite(41,LOW);}
  }
  if (paramNum == 13){
      if (atoi(param)>0){digitalWrite(43,HIGH);} else {digitalWrite(43,LOW);}
  }
  if (paramNum == 14){
      if (atoi(param)>0){digitalWrite(45,HIGH);} else {digitalWrite(45,LOW);}
  }
  if (paramNum == 15){
      if (atoi(param)>0){digitalWrite(47,HIGH);} else {digitalWrite(47,LOW);}
  }
  if (paramNum == 16){
      if (atoi(param)>0){digitalWrite(49,HIGH);} else {digitalWrite(49,LOW);}
  }
  if (paramNum == 17){
      if (atoi(param)>0){digitalWrite(51,HIGH);} else {digitalWrite(51,LOW);}
  }
  if (paramNum == 18){
      if (atoi(param)>0){digitalWrite(53,HIGH);} else {digitalWrite(53,LOW);}
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

void removeChar(char *str, char c) {  //https://quescol.com/interview-preparation/remove-character-from-string-c
    int i, j;
    int len = strlen(str);
    for (i = j = 0; i < len; i++) {
        if (str[i] != c) {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}
