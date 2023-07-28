#include <WiFi.h>
#include <ESP_Mail_Client.h>    //needed for sending emails
#include <esp_task_wdt.h>

#define WIFI_NETWORK "YOUR-WIFI-SSID"          //the SSID for the wireless network
#define WIFI_PASSWORD "YOUR-WIFI-PASSWORD"    //the password for the wireless network
#define WIFI_TIMEOUT_MS 20000               //The amount of time in milliseconds to wait for the Wifi connection when trying to connect before giving up
#define WIFI_CHECK_MS 10000                 //The amount of time in milliseconds to wait before checking the wifi is still up.                    
#define WIFI_STATUS_PIN 33                  //The LED pin that will give an indication of the wifi connection status.

//input/output pins
#define IN_LOW 22     //this pin is attached to the sensor at the lowest level in the pool
#define IN_MEDIUM 19  //this pin is at the "just right" water level in the pool
#define IN_HIGH 17
#define IN_HIGHEST 16 //this pin is attached to the sensor at the highest level in the pool
#define OUT_RELAY1 26 //WORKS:25,26,27,33,32,14,13,12   NOT WORKING:35,34
#define OUT_RELAY2 27

#define THRESHOLDCONSECUTIVECOUNT 40    //The number of times we have the same result before taking action.  Needed because water has waves.

//Email setup stuff
#define SMTP_HOST "smtp.office365.com"            // The smtp host name e.g. smtp.gmail.com for GMail or smtp.office365.com for Outlook or smtp.mail.yahoo.com 
#define SMTP_PORT esp_mail_smtp_port_587          // The smtp port  e.g. 25  or esp_mail_smtp_port_25, 465 or esp_mail_smtp_port_465,  587 or esp_mail_smtp_port_587
#define AUTHOR_EMAIL "FromMe@outlook.com"  // The log in credentials for the email account used to send emails:  user
#define AUTHOR_PASSWORD "SENDING-EMAIL-PASSWORD"                //                                                                    password
#define SENDER_NAME "ESP Mail"
#define AUTHOR_DOMAIN "mydomain.net"
#define RECIPIENT_NAME "John"                   //person we are sending the emails to
#define RECIPIENT_EMAIL "john@mailinator.com"      //target email address to send to

//GLOBAL VARIABLES
SMTPSession smtp;                                 // The SMTP Session object used for Email sending 
int mailInProgress = 0;
int lowPinHistory[THRESHOLDCONSECUTIVECOUNT];
int mediumPinHistory[THRESHOLDCONSECUTIVECOUNT];
int highPinHistory[THRESHOLDCONSECUTIVECOUNT];
int highestPinHistory[THRESHOLDCONSECUTIVECOUNT];

void keepWifiAlive(void * parameters){
  for(;;){
    //if connected do nothing and quit
    if(WiFi.status() == WL_CONNECTED){
      Serial.print("WiFi still connected: "); Serial.println(WiFi.localIP().toString().c_str());
      digitalWrite(WIFI_STATUS_PIN, HIGH);
      vTaskDelay(WIFI_CHECK_MS / portTICK_PERIOD_MS);
      continue;                                 //skip to the beginning of the for loop
    }

    //initiate connection
    mailInProgress=1;
    Serial.println("Wifi Connecting");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);
    unsigned long startAttemptTime = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS){
      //we are still not connected and haven't reached the timout so flash the wifi status LED and try again after 1 second.
      digitalWrite(WIFI_STATUS_PIN, HIGH);
      Serial.print(".");
      vTaskDelay(500 / portTICK_PERIOD_MS);
      digitalWrite(WIFI_STATUS_PIN, LOW);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      continue;                                 //skip to the beginning of the for loop
    }
    if(WiFi.status() == WL_CONNECTED){
      //if connection successful then wifi LED is to be solid on.
      Serial.print("[WIFI] Connected: "); Serial.println(WiFi.localIP().toString().c_str());
      digitalWrite(WIFI_STATUS_PIN, HIGH);
      mailInProgress=1;
      vTaskDelay(20 / portTICK_PERIOD_MS);  //to avoid watchdog timeouts
      sendEmail("ESP32 Pool Monitor: connected to WiFi", "<p>ESP32 Pool Monitor: (re)connected to WiFi</p><p>The message was sent via ESP device.</p>");
      mailInProgress=0;
      
    } else {
      Serial.println("[WIFI] Failed to Connect before timeout");
      digitalWrite(WIFI_STATUS_PIN, LOW);
    }
  }
}

void sendEmail(char subject[], char htmlMsg[]){
  Serial.print("Sending Email: Subject: ");  Serial.println(subject);

  MailClient.networkReconnect(true);

  vTaskDelay(20 / portTICK_PERIOD_MS);  //to avoid watchdog timeouts
  smtp.debug(0);                //Enable the debug via Serial port  0 for no debugging, 1 for basic level debugging   Debug port can be changed via ESP_MAIL_DEFAULT_DEBUG_PORT in ESP_Mail_FS.h
  smtp.callback(smtpCallback);  /* Set the callback function to get the sending results */
  ESP_Mail_Session session;     /* Declare the session config data */

  /* Set the session config */
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = F(AUTHOR_DOMAIN);
  vTaskDelay(20 / portTICK_PERIOD_MS);  //to avoid watchdog timeouts

  /* Set the NTP config time */
  session.time.ntp_server = F("pool.ntp.org,time.nist.gov");
  session.time.gmt_offset = 3;
  session.time.day_light_offset = 0;
  vTaskDelay(20 / portTICK_PERIOD_MS);  //to avoid watchdog timeouts

  /* Declare the message class */
  SMTP_Message message;

  /* Set the message headers */
  message.sender.name = F(SENDER_NAME);
  message.sender.email = AUTHOR_EMAIL;
  //message.subject = F("Test sending html Email");
  message.subject = subject;
  message.addRecipient(F(RECIPIENT_NAME), F(RECIPIENT_EMAIL));
  vTaskDelay(20 / portTICK_PERIOD_MS);  //to avoid watchdog timeouts

  //String htmlMsg = "<p>This is the <span style=\"color:#ff0000;\">html text</span> message.</p><p>The message was sent via ESP device.</p>";
  message.html.content = htmlMsg;

  message.html.charSet = F("us-ascii"); //The html text message character set e.g. us-ascii, utf-8, utf-7.  The default value is utf-8
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;  /** The content transfer encoding e.g. enc_7bit or "7bit" (not encoded), enc_qp or "quoted-printable" (encoded), enc_base64 or "base64" (encoded), enc_binary or "binary" (not encoded), enc_8bit or "8bit" (not encoded).  The default value is "7bit" */
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;  /** The message priority, esp_mail_smtp_priority_high or 1, esp_mail_smtp_priority_normal or 3, esp_mail_smtp_priority_low or 5.  The default value is esp_mail_smtp_priority_low */
  vTaskDelay(20 / portTICK_PERIOD_MS);  //to avoid watchdog timeouts

  /* Set the custom message header */
  message.addHeader(F("Message-ID: <abcde.fghij@gmail.com>"));

  /* Connect to server with the session config */
  if (!smtp.connect(&session))
    return;
  
  vTaskDelay(20 / portTICK_PERIOD_MS);  //to avoid watchdog timeouts
  /* Start sending Email and close the session */
  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());

  // to clear sending result log
  // smtp.sendingResult.clear();

  ESP_MAIL_PRINTF("Free Heap: %d\n", MailClient.getFreeHeap());
  
}



void monitorWaterLevel(void * parameters){
  
  int mediumConsecutiveHighCount = 0;       
  int mediumConsecutiveLowCount = 0;
  int highestConsecutiveLowCount=0;
  
  digitalWrite(OUT_RELAY2,LOW); 
  
  for(;;){
    vTaskDelay(100 / portTICK_PERIOD_MS);   //time between taking samples 100 milliseconds 
    int lowPinVal = digitalRead(IN_LOW);
    int mediumPinVal = digitalRead(IN_MEDIUM);
    int highPinVal = digitalRead(IN_HIGH);
    int highestPinVal = digitalRead(IN_HIGHEST);
    logPinValues(lowPinVal, mediumPinVal, highPinVal, highestPinVal );
   

    /**************CODE FOR MANAGING THE RELAY FOR FILLING THE POOL**********************************/
    if (mediumPinVal==HIGH){  //if pin value is HIGH Water is not touching the sensor pin.  IE below the turn off height
      mediumConsecutiveLowCount = 0;
      if (mediumConsecutiveHighCount < THRESHOLDCONSECUTIVECOUNT) {
        mediumConsecutiveHighCount++;    
      } else {
        digitalWrite(OUT_RELAY1,HIGH);    //turn on the relay to keep filling the pool
        //Serial.println("Relay High");
        if (mediumConsecutiveHighCount == THRESHOLDCONSECUTIVECOUNT && WiFi.status() == WL_CONNECTED && mailInProgress < 1 ){
          mediumConsecutiveHighCount++; 
          mailInProgress=1;
          String emailBody = "<p>Started Filling Pool</p><p>The message was sent via ESP device.</p>";
          emailBody += pinValuesLog();

          //convert string to char array so as that is the datatype accepted by the sendEmail function
          int str_len = emailBody.length() + 1;   // Length (with one extra character for the null terminator)
          char char_array[str_len]; // Prepare the character array (the buffer) 
          emailBody.toCharArray(char_array, str_len);  // Copy it over
          
          vTaskDelay(20 / portTICK_PERIOD_MS);  //to avoid watchdog timeouts
          sendEmail("Started Filling Pool", char_array);
          mailInProgress=0;
        }
      }
    }else {
      mediumConsecutiveHighCount = 0;
      if (mediumConsecutiveLowCount < THRESHOLDCONSECUTIVECOUNT) {
        mediumConsecutiveLowCount++;
      } else{
        digitalWrite(OUT_RELAY1,LOW);
        //Serial.println("Relay Low");
        if (mediumConsecutiveLowCount == THRESHOLDCONSECUTIVECOUNT && WiFi.status() == WL_CONNECTED && mailInProgress < 1){
          mediumConsecutiveLowCount++;
          mailInProgress=1;
          String emailBody = "<p>Stopped Filling Pool</p><p>The message was sent via ESP device.</p>";
          emailBody += pinValuesLog();

          //convert string to char array so as that is the datatype accepted by the sendEmail function
          int str_len = emailBody.length() + 1;   // Length (with one extra character for the null terminator)
          char char_array[str_len]; // Prepare the character array (the buffer) 
          emailBody.toCharArray(char_array, str_len);  // Copy it over
          
          vTaskDelay(20 / portTICK_PERIOD_MS);  //to avoid watchdog timeouts
          sendEmail("Stopped Filling Pool", char_array);
          mailInProgress=0;
        }
      }
    }
    /**************END OF CODE FOR MANAGING THE RELAY FOR FILLING THE POOL**********************************/

    /*******************CODE FOR WARNING ABOUT VERY HIGH WATER LEVEL****************************************/
    if (highestPinVal==LOW){  //LOW value at this pin means water has reached the warning height
      
      if (highestConsecutiveLowCount < THRESHOLDCONSECUTIVECOUNT) {
        highestConsecutiveLowCount++;    
      } else {
        if(highestConsecutiveLowCount == THRESHOLDCONSECUTIVECOUNT && mailInProgress < 1){
          mailInProgress=1;
          String emailBody = "<p>WARNING Pool Level TOO HIGH</p><p>The message was sent via ESP device.</p>";
          emailBody += pinValuesLog();

          //convert string to char array so as that is the datatype accepted by the sendEmail function
          int str_len = emailBody.length() + 1;   // Length (with one extra character for the null terminator)
          char char_array[str_len]; // Prepare the character array (the buffer) 
          emailBody.toCharArray(char_array, str_len);  // Copy it over 
          
          sendEmail("WARNING Pool Level TOO HIGH", char_array);
          mailInProgress=0;
          highestConsecutiveLowCount++;
        }
      }
    } else {
        highestConsecutiveLowCount=0;
    }
    /*******************END OF CODE FOR WARNING ABOUT VERY HIGH WATER LEVEL****************************************/
  }  //end for 
}// end task monitorWaterLevel


//function logPinValues /////////////////////////////////////////////////////////////////
//  records the a history of the last read values from each pin.  
//  array index 0 is the oldest kept value
//  array index THRESHOLDCONSECUTIVECOUNT - 1 is the most recent value
/////////////////////////////////////////////////////////////////////////////////////////
void logPinValues(int lowPinVal, int mediumPinVal, int highPinVal, int highestPinVal ){
  Serial.print(lowPinVal);
  Serial.print(mediumPinVal);
  Serial.print(highPinVal);
  Serial.print(highestPinVal);  
  Serial.print(" ");
  for (int i=0; i < THRESHOLDCONSECUTIVECOUNT - 1; i++){
    lowPinHistory[i] = lowPinHistory[i + 1];
    mediumPinHistory[i] = mediumPinHistory[i + 1];
    highPinHistory[i] = highPinHistory[i + 1];
    highestPinHistory[i] = highestPinHistory[i + 1];
  }
  lowPinHistory[THRESHOLDCONSECUTIVECOUNT - 1] = lowPinVal;
  mediumPinHistory[THRESHOLDCONSECUTIVECOUNT - 1] = mediumPinVal;
  highPinHistory[THRESHOLDCONSECUTIVECOUNT - 1] = highPinVal;
  highestPinHistory[THRESHOLDCONSECUTIVECOUNT - 1] = highestPinVal;
  
}

//function pinValuesLog ////////////////////////////////////////////////////////////////
//  This function returns html formatted rows in a string that shows the historic read
//  values from the 4 water sensor pins
//  0011   would mean lowest and medium pin were in the water
//  0000   would mean all pins were in the water.
//  l = lowest, m = medium, h = high pin and H = highest pin
///////////////////////////////////////////////////////////////////////////////////////
String pinValuesLog(){
  String string1 = "lmhH 0 means pin in water</br>";
  String string2 = "xxxx</br>";
  String logString = "";
  logString = logString += string1;
  for (int i=0; i < THRESHOLDCONSECUTIVECOUNT - 1; i++){
    string2 = "";
    string2 += lowPinHistory[i];
    string2 += mediumPinHistory[i];
    string2 += highPinHistory[i];
    string2 += highestPinHistory[i];
    string2 += "</br>";
    logString = logString += string2;
  }
  return logString;
}



void setup() {
  esp_task_wdt_init(30, false);  //Set Watchdog timeout to 30.  Higher than default because timeouts were happening when trying to send emails
  
  // put your setup code here, to run once:
  pinMode(IN_LOW, INPUT_PULLUP);
  pinMode(IN_MEDIUM, INPUT_PULLUP);
  pinMode(IN_HIGH, INPUT_PULLUP);
  pinMode(IN_HIGHEST, INPUT_PULLUP); 
  pinMode(OUT_RELAY1, OUTPUT);
  pinMode(OUT_RELAY2, OUTPUT);
  
  
  Serial.begin(9600);

  //Wifi setup
  pinMode(WIFI_STATUS_PIN, OUTPUT);
  digitalWrite(WIFI_STATUS_PIN, LOW);
  xTaskCreatePinnedToCore(
    keepWifiAlive,  //function name
    "Keep Wifi Alive", //task name
    50000, //stack size increase above 1000 for connect to wifi
    NULL, //task parameters
    2, //priority
    NULL, //task handle
    CONFIG_ARDUINO_RUNNING_CORE //CPU Core 
  );


  //Pool Water Level Monitoring setup
  xTaskCreate(
    monitorWaterLevel,  //function name
    "Monitor Water Level", //task name
    50000, //stack size increase above 1000 for connect to wifi
    NULL, //task parameters
    1, //priority
    NULL //task handle
  );
}


/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status)
{
  /* Print the current status */
  Serial.println(status.info());
  vTaskDelay(20 / portTICK_PERIOD_MS);  //to avoid watchdog timeouts

  /* Print the sending result */
  if (status.success())
  {
    // ESP_MAIL_PRINTF used in the examples is for format printing via debug Serial port
    // that works for all supported Arduino platform SDKs e.g. AVR, SAMD, ESP32 and ESP8266.
    // In ESP32 and ESP32, you can use Serial.printf directly.

    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    vTaskDelay(20 / portTICK_PERIOD_MS);  //to avoid watchdog timeouts
    ESP_MAIL_PRINTF("Message sent failed: %d\n", status.failedCount());
    Serial.println("----------------\n");

    for (size_t i = 0; i < smtp.sendingResult.size(); i++)
    {
      vTaskDelay(20 / portTICK_PERIOD_MS);  //to avoid watchdog timeouts
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);

      // In case, ESP32, ESP8266 and SAMD device, the timestamp get from result.timestamp should be valid if
      // your device time was synched with NTP server.
      // Other devices may show invalid timestamp as the device time was not set i.e. it will show Jan 1, 1970.
      // You can call smtp.setSystemTime(xxx) to set device time manually. Where xxx is timestamp (seconds since Jan 1, 1970)
      time_t ts = (time_t)result.timestamp;

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %s\n", asctime(localtime(&ts)));
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients.c_str());
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject.c_str());
    }
    Serial.println("----------------\n");
    vTaskDelay(20 / portTICK_PERIOD_MS);  //to avoid watchdog timeouts
    // You need to clear sending result as the memory usage will grow up.
    smtp.sendingResult.clear();
  }
}

void loop() {
  // don't use main loop as this is multitasking using RTOS tasks
}
