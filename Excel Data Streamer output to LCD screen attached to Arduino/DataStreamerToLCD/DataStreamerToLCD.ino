/*
 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
  http://www.arduino.cc/en/Tutorial/LiquidCrystalHelloWorld
*/

// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int len = 60;
char my_str[len];
char my_str2[len];
int pos = 0;
int incomingByte = 0; // for incoming serial data

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a test message to the LCD.
  lcd.print("hello, world!");

  Serial.begin(38400);    // configure serial coms baud rate
}

void serialEvent() {
  //statements
  outputStuff();
}

void outputStuff(){
// send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

      my_str[pos] = incomingByte;
      pos++;  

    if(incomingByte == 10){                   //10 is line feed character
    
      //clear the top line
      lcd.setCursor(0, 0);
      lcd.print("                ");
      //clear the bottom line
      lcd.setCursor(0, 1);
      lcd.print("                ");

      //print the received string on the top line
      lcd.setCursor(0, 0);
      lcd.print(my_str);  //print first line of text

      if (pos > 15){      
        for(int i = 0; i<pos; i++){
          my_str2[i] = my_str[i+16];  //get second line of text for the lcd
        }
        //my_str[pos+1]='\0';
        lcd.setCursor(0, 1);  
        lcd.print(my_str2);  //print second line of text         
      }
      
      //clear the string for next time
      for (int i = 0; i<=len-1; i++){
        my_str[i] = 0; 
      }
      pos = 0;
    }    
  }
}

void loop() {

}
