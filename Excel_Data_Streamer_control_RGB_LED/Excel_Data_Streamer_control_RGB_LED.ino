
const int len = 60;
char my_str[len];
int pos = 0;
int incomingByte = 0; // for incoming serial data

int red_light_pin= 11;
int green_light_pin = 10;
int blue_light_pin = 9;

int vRed = 0;    // variable to store the servo position
int vGreen = 0;
int vBlue = 0;

void setup() {
  Serial.begin(38400);    // configure serial coms baud rate
  
  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
  pinMode(blue_light_pin, OUTPUT);
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

  RGB_color(vRed, vGreen, vBlue);

}

void processParam(char * param, int paramNum){
  char key[] = "true"; //https://www.cplusplus.com/reference/cstring/strcmp/
   if (strcmp (key,param) == 0) {        
        digitalWrite(paramNum + 1, HIGH);
    }
   if (paramNum == 1){
      vRed = 255 - atoi(param);
   }
   if (paramNum == 2){
      vGreen = 255- atoi(param);
   } 
   if (paramNum == 3){
      vBlue = 255 - atoi(param);
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

void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(red_light_pin, red_light_value);
  analogWrite(green_light_pin, green_light_value);
  analogWrite(blue_light_pin, blue_light_value);
}

void loop() {
    //Do Nothing
}
