#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Replace with your network credentials
//const char* ssid = "POCO";
//const char* password = "1sampai8";
//
//// Initialize Telegram BOT
//#define BOTtoken "5988952113:AAHdA51mTIXE6BkOlOKHlJjGO16PR0fv0wE"  // your Bot Token (Get from Botfather)
//#define CHAT_ID "5201189309"

const char* ssid = "MyASUS";
const char* password = "hy12345678";

// Initialize Telegram BOT
String BOTtoken = "5630497051:AAGF7x-ewhnDMZVnqgrkQ2levx1SAx-2dhs";  // your Bot Token (Get from Botfather)
String CHAT_ID = "1096619393";


#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

const int analogInPin = A0; 
int relay= 16;
int sensorValue = 0;

// Handle what happens when you receive new messages
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
      welcome += "/status : Untuk mengetahui kondisi Tanah\n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/status") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "STATUS SENSOR\n\n";
      welcome += "/kelembapan : \n"+ sensorValue ;
      bot.sendMessage(chat_id, welcome, "");
      }
    }
  }


void setup() {
  // initialize serial communication at 115200
  Serial.begin(115200);
  pinMode(analogInPin, INPUT);
  pinMode(relay, OUTPUT);
  
  #ifdef ESP8266
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  #endif

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
    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
  baca_sensor();
}
void baca_sensor(){
  // read the analog in value
  sensorValue = analogRead(analogInPin);
 
  // print the readings in the Serial Monitor
  Serial.print("sensor = ");
  Serial.println(sensorValue);

  int pesan = 0;
  if (sensorValue > 650 ){
    pesan++;
    digitalWrite(relay, HIGH);
    if (pesan <= 1){
      bot.sendMessage(CHAT_ID, "Welcome \n Tanah Kering, Pompa Dihidupkan", "");
      
      }
    }
  else{
    digitalWrite(relay, LOW);
    }
  delay(2000);
  }
