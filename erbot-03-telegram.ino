#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

const char* ssid = "pasaman";
const char* password = "19451945";
String BOTtoken = "5189928151:AAF2EuKhXfwsQpwgWeY5yjrri5-xNhsVw3U";
String CHAT_ID = "760425294";
bool sendPhoto = false;

WiFiClientSecure clientTCP;
UniversalTelegramBot bot(BOTtoken, clientTCP);

#define FLASH_LED_PIN 4
#define pir 12

bool flashState = LOW;
bool nilai_pir = LOW;

//Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

//CAMERA_MODEL_AI_THINKER
#include "setcam.h"

void handleNewMessages(int numNewMessages) {
  Serial.print("Handle New Messages: ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }

    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);
    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome , " + from_name + "\n";
      welcome += "Ikuti Perintah Berikut: \n";
      welcome += "/photo : takes a new photo\n";
      welcome += "/flash : toggles flash LED \n";
      bot.sendMessage(CHAT_ID, welcome, "");
    }
    if (text == "/flash") {
      flashState = !flashState;
      digitalWrite(FLASH_LED_PIN, flashState);
      Serial.println("Change flash LED state");
    }
    if (text == "/photo") {
      sendPhoto = true;
      Serial.println("New photo request");
    }
  }
}

#include "sendtele.h"

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  Serial.begin(115200);
  pinMode(FLASH_LED_PIN, OUTPUT);
  pinMode(pir, INPUT);
  digitalWrite(pir, nilai_pir);
  digitalWrite(FLASH_LED_PIN, flashState);
  configInitCamera();
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  clientTCP.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("."); delay(500);
  }
  Serial.println();
  Serial.print("ESP32-CAM IP Address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(FLASH_LED_PIN, 1); delay(300);
  bot.sendMessage(CHAT_ID, "Alat Aktif", "");
  digitalWrite(FLASH_LED_PIN, 0); delay(1000);
  Serial.println("SUKSES");
}

void loop() {

  nilai_pir = digitalRead(pir);
  delay(500);

  while (nilai_pir == HIGH) {
    digitalWrite(FLASH_LED_PIN, 1);
    delay(5000);
    digitalWrite(FLASH_LED_PIN, 0);
    sendPhoto = true;
    Serial.println("gambar dalam proses");
    sendPhotoTelegram(); sendPhoto = false;
    Serial.println("Done");
    delay(4000); break;
  }
}
