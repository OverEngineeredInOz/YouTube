
const int len = 60;
char my_str[len];
int pos = 0;
int incomingByte = 0; // for incoming serial data
int top = 4;
int topleft = 5;
int topright = 6;
int middle = 7;
int bottomleft = 8;
int bottom = 9;
int bottomright = 10;

int score1 = 0;
int score2 = 0;




void setup() {
  Serial.begin(38400);    // configure serial coms baud rate
  
  for (int i=2; i<14; i++){
    pinMode(i, OUTPUT);   
  }
  
  
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



}

void processParam(char * param, int paramNum){
  char key[] = "true"; //https://www.cplusplus.com/reference/cstring/strcmp/
   if (strcmp (key,param) == 0) {        
      for (int i=4; i<14; i++){
          digitalWrite(i, LOW);   
      }
    }
   if (paramNum == 1){
      score1 = atoi(param);
   }
   if (paramNum == 2){
      score2 = atoi(param);
   } 
   if (paramNum == 3){
      
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

void doSegments(int num){
  if (num == 0){
    digitalWrite(top, HIGH);
    digitalWrite(topleft, HIGH);
    digitalWrite(topright,HIGH);
    digitalWrite(middle,LOW);
    digitalWrite(bottomleft,HIGH);
    digitalWrite(bottom,HIGH);
    digitalWrite(bottomright,HIGH);
  }
  if (num == 1){
    digitalWrite(top, LOW);
    digitalWrite(topleft, LOW);
    digitalWrite(topright,HIGH);
    digitalWrite(middle,LOW);
    digitalWrite(bottomleft,LOW);
    digitalWrite(bottom,LOW);
    digitalWrite(bottomright,HIGH);
  }
  if (num == 2){
    digitalWrite(top, HIGH);
    digitalWrite(topleft, LOW);
    digitalWrite(topright,HIGH);
    digitalWrite(middle,HIGH);
    digitalWrite(bottomleft,HIGH);
    digitalWrite(bottom,HIGH);
    digitalWrite(bottomright,LOW);
  }
  if (num == 3){
    digitalWrite(top, HIGH);
    digitalWrite(topleft, LOW);
    digitalWrite(topright,HIGH);
    digitalWrite(middle,HIGH);
    digitalWrite(bottomleft,LOW);
    digitalWrite(bottom,HIGH);
    digitalWrite(bottomright,HIGH);
  }
  if (num == 4){
    digitalWrite(top, LOW);
    digitalWrite(topleft, HIGH);
    digitalWrite(topright,HIGH);
    digitalWrite(middle,HIGH);
    digitalWrite(bottomleft,LOW);
    digitalWrite(bottom,LOW);
    digitalWrite(bottomright,HIGH);
  }
  if (num == 5){
    digitalWrite(top, HIGH);
    digitalWrite(topleft, HIGH);
    digitalWrite(topright,LOW);
    digitalWrite(middle,HIGH);
    digitalWrite(bottomleft,LOW);
    digitalWrite(bottom,HIGH);
    digitalWrite(bottomright,HIGH);
  }
  if (num == 6){
    digitalWrite(top, HIGH);
    digitalWrite(topleft, HIGH);
    digitalWrite(topright,LOW);
    digitalWrite(middle,HIGH);
    digitalWrite(bottomleft,HIGH);
    digitalWrite(bottom,HIGH);
    digitalWrite(bottomright,HIGH);
  }
  if (num == 7){
    digitalWrite(top, HIGH);
    digitalWrite(topleft, LOW);
    digitalWrite(topright,HIGH);
    digitalWrite(middle,LOW);
    digitalWrite(bottomleft,LOW);
    digitalWrite(bottom,LOW);
    digitalWrite(bottomright,HIGH);
  }
  if (num == 8){
    digitalWrite(top, HIGH);
    digitalWrite(topleft, HIGH);
    digitalWrite(topright,HIGH);
    digitalWrite(middle,HIGH);
    digitalWrite(bottomleft,HIGH);
    digitalWrite(bottom,HIGH);
    digitalWrite(bottomright,HIGH);
  }  
  if (num == 9){
    digitalWrite(top, HIGH);
    digitalWrite(topleft, HIGH);
    digitalWrite(topright,HIGH);
    digitalWrite(middle,HIGH);
    digitalWrite(bottomleft,LOW);
    digitalWrite(bottom,LOW);
    digitalWrite(bottomright,HIGH);
  }
}

void loop() {
    //Do Nothing
    delay(5);
    doSegments(score1);
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);
    delay(5);
    doSegments(score2);
    digitalWrite(3, HIGH);
    digitalWrite(2, LOW);

    //digitalWrite(10, HIGH);   
}
