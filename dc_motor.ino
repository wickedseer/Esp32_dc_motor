#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Motor A
int motor1Pin1 = 18; // GPIO 18 (controls direction)
int motor1Pin2 = 5; // GPIO 5 (controls direction)
int enable1Pin = 19; // GPIO 19 (controls speed)

// Setting PWM properties
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 200;

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

#define BOTtoken "XXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXX" // Get from BotFather
#define CHAT_ID "XXXXXXXXX" // Get from IDBot

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 50 mili second.
int botRequestDelay = 50;
unsigned long lastTimeBotRan;

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to control your outputs.\n\n";
      welcome += "/clockwise \n";
      welcome += "/anticlockwise \n";
      welcome += "/stop \n";
      bot.sendMessage(chat_id, welcome, "");
    }
    
    if (text == "/clockwise@ESP32SoCWatchBot") {
      bot.sendMessage(chat_id, "Motor moving in clockwise direction", "");
      // Move DC motor forward with increasing speed
      digitalWrite(motor1Pin1, LOW);
      digitalWrite(motor1Pin2, HIGH);
      while (dutyCycle <= 255){
        ledcWrite(pwmChannel, dutyCycle);   
        Serial.print("Forward with duty cycle: ");
        Serial.println(dutyCycle);
        dutyCycle = dutyCycle + 5;
        delay(1000); 
      } 
    }
    
    if (text == "/anticlockwise@ESP32SoCWatchBot") {
      bot.sendMessage(chat_id, "Motor moving in anticlockwise direction", "");
      // Move DC motor backwards with increasing speed
      Serial.println("Moving Backwards");
      digitalWrite(motor1Pin1, HIGH);
      digitalWrite(motor1Pin2, LOW);
      while (dutyCycle <= 255){
        ledcWrite(pwmChannel, dutyCycle);   
        Serial.print("Forward with duty cycle: ");
        Serial.println(dutyCycle);
        dutyCycle = dutyCycle + 5;
        delay(1000);  
    }
    }
    
    if (text == "/stop@ESP32SoCWatchBot") {
      if (digitalRead(motor1Pin1)== LOW && digitalRead(motor1Pin2)== LOW){
        bot.sendMessage(chat_id, "Motor not in motion", "");
      }
      else{
        bot.sendMessage(chat_id, "Motor stopped", "");
        digitalWrite(motor1Pin1, LOW);
        digitalWrite(motor1Pin2, LOW);
      }
    }
  }
}
void setup() {
  // sets the pins as outputs:
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);

  // configure LED PWM functionalitites
  ledcSetup(pwmChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(enable1Pin, pwmChannel);

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}

 
