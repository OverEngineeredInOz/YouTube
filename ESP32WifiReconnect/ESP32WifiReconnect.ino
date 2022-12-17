#include <WiFi.h>

#define WIFI_NETWORK "YOUR WIFI NETWORK SSID"
#define WIFI_PASSWORD "YOUR WIFI PASSWORD"
#define WIFI_TIMEOUT_MS 20000
#define WIFI_STATUS_PIN 19

#define LED6_PIN 22
#define LED7_PIN 23

//------------------------------------WIFI KEEP ALIVE CODE--------------------------------------------------------
void keepWifiAlive(void * parameters){ 
  for(;;){
    //if connected do nothing and quit
    if(WiFi.status() == WL_CONNECTED){
      Serial.print("WiFi still connected: "); Serial.println(WiFi.localIP().toString().c_str());
      digitalWrite(WIFI_STATUS_PIN, HIGH);
      vTaskDelay(10000 / portTICK_PERIOD_MS);
      continue;
    }
    //initiate connection
    Serial.println("Wifi Connecting");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);
    unsigned long startAttemptTime = millis();

    //Indicate to the user that we are not currently connected but are trying to connect.
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS){
      digitalWrite(WIFI_STATUS_PIN, HIGH);
      Serial.print(".");
      vTaskDelay(500 / portTICK_PERIOD_MS);
      digitalWrite(WIFI_STATUS_PIN, LOW);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      continue;
    }
    //Indicate to the user the outcome of our attempt to connect.
    if(WiFi.status() == WL_CONNECTED){
      Serial.print("[WIFI] Connected: "); Serial.println(WiFi.localIP().toString().c_str());
      digitalWrite(WIFI_STATUS_PIN, HIGH);
    } else {
      Serial.println("[WIFI] Failed to Connect before timeout");
      digitalWrite(WIFI_STATUS_PIN, LOW);
    }
  }
}  
//---------------------------------END OF WIFI KEEP ALIVE CODE--------------------------------------------------------
//---------------------------------LED FLASHER TASK CODE--------------------------------------------------------------
void flashLeds(void * parameters){
  for(;;){
    vTaskDelay(200 / portTICK_PERIOD_MS);
    digitalWrite(LED6_PIN, LOW);
    digitalWrite(LED7_PIN, HIGH);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    digitalWrite(LED6_PIN, HIGH);
    digitalWrite(LED7_PIN, LOW);
  }
}
//---------------------------------END OF LED FLASHER TASK CODE--------------------------------------------------------
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  //Wifi setup
  pinMode(WIFI_STATUS_PIN, OUTPUT);
  digitalWrite(WIFI_STATUS_PIN, LOW);
  xTaskCreatePinnedToCore(
    keepWifiAlive,  //function name
    "Keep Wifi Alive", //task name
    5000, //stack size increase above 1000 for connect to wifi
    NULL, //task parameters
    2, //priority
    NULL, //task handle
    CONFIG_ARDUINO_RUNNING_CORE //CPU Core 
  );

  //LED Flasher setup
  pinMode(LED6_PIN, OUTPUT);
  pinMode(LED7_PIN, OUTPUT);
  xTaskCreate(
    flashLeds,  //function name
    "Keep Wifi Alive", //task name
    50000, //stack size increase above 1000 for connect to wifi
    NULL, //task parameters
    1, //priority
    NULL //task handle
  );
}

void loop() {
  // do nothing in this loop everything is done in FreeRTOS tasks
}
