#define DEBUG 1
#define UART Serial2
#define USB  Serial
#define GMT 18000
const char* ssid     = "CYBERSPACE";
const char* password = "IDDQD999";
const String wallet_name  = "p0s.near";
const String balance_link = "https://nearblocks.io/api/account/balance?address=" + wallet_name;
#define PAYGET "PAYGET"
#define PAYOK "PAYOK"
#define PAYNOT "PAYNOT"
const int timeout = 1000 * 60;

#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>

StaticJsonDocument<1000> doc;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
String payload = "";
String tmp = "" ;
String hum = "" ;
String balance = "";
String formattedDate;
String dayStamp;
String timeStamp;
String prev_balance;
String get_uart;
unsigned long next_millis;
void setup(void) {
  USB.begin(115200);
  UART.begin(115200);
  WiFi.begin(ssid, password);
  USB.print(F("Connecting to "));
  USB.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    USB.print(".");
  }
  USB.println("");
  USB.println(F("WiFi connected."));
  USB.println(F("IP address: "));
  USB.println(WiFi.localIP());
  get_balance();
}

void loop() {
  if (UART.available() > 0) {
    byte READ_byte = UART.read();
    if ((READ_byte >= 65) && (READ_byte  <= 90)) {
      //      USB.println(char(READ_byte));
      //      USB.println(get_uart);
      //      get_uart += char(UART.read());
      get_uart += char(READ_byte);

      if (get_uart == PAYGET) {
        USB.println(get_uart);
        prev_balance = balance;
        next_millis = millis() + timeout;
        while (1) {
          if (next_millis > millis()) {
            get_balance();
            if (prev_balance != balance) {
              USB.println(PAYOK);
              UART.println(PAYOK);
              get_uart = "";
              break;
            } else {
//              USB.print(".");
              UART.print(".");
              delay(1000);
            }
          } else {
            USB.println(PAYNOT);
            UART.println(PAYNOT);
            get_uart = "";
            break;
          }
        }
      }
    }
  }
}

void get_balance() {
  //  USB.println(balance_link);
  if ((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;
    http.begin(balance_link);
    int httpCode = http.GET();
    if (httpCode > 0) {
      payload = http.getString();
      // Serial.println(httpCode);
      //      Serial.println(payload);
    } else {
      USB.println("Error on HTTP request");
    }
    http.end();
  }
  char inp[1000];
  payload.toCharArray(inp, 1000);
  deserializeJson(doc, inp);
  String curr_balance = doc["balance"];
  balance = curr_balance;
  //  balance = "10";
  USB.print(F("Balance: "));
  USB .println(balance);
}
