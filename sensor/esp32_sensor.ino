#include <WiFi.h>
#include <HTTPClient.h>

// 联调前填写本地 WiFi；不要将真实凭据提交到仓库。
const char* ssid="YOUR_WIFI_SSID";
const char* password="YOUR_WIFI_PASSWORD";

String serverUrl="http://YOUR_BACKEND_IP:5000/event";

#define LED_LEAVE 23
#define LED_HOME 25
#define KEY_LEAVE 18
#define KEY_HOME 19

void connectWiFi()
{
  Serial.print("Connecting WiFi");
  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi Connected");
  Serial.println(WiFi.localIP());
}

void sendEvent(String event)
{
  if(WiFi.status()!=WL_CONNECTED)
  {
    Serial.println("WiFi disconnected");
    return;
  }

  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type","application/json");

  String json="{\"event\":\""+event+"\",\"device\":\"esp32_sensor\"}";

  Serial.println(json);

  int code=http.POST(json);

  Serial.print("HTTP Response:");
  Serial.println(code);

  http.end();
}

void triggerLeaveEvent()
{
  Serial.println("EVENT: LEAVE_EVENT");
  sendEvent("LEAVE_EVENT");

  digitalWrite(LED_LEAVE,LOW);
  delay(500);
  digitalWrite(LED_LEAVE,HIGH);
}

void triggerHomeEvent()
{
  Serial.println("EVENT: HOME_EVENT");
  sendEvent("HOME_EVENT");

  digitalWrite(LED_HOME,LOW);
  delay(500);
  digitalWrite(LED_HOME,HIGH);
}

void setup()
{
  Serial.begin(115200);

  pinMode(LED_LEAVE,OUTPUT);
  pinMode(LED_HOME,OUTPUT);

  pinMode(KEY_LEAVE,INPUT_PULLUP);
  pinMode(KEY_HOME,INPUT_PULLUP);

  digitalWrite(LED_LEAVE,HIGH);
  digitalWrite(LED_HOME,HIGH);

  connectWiFi();

  Serial.println("Sensor Node Ready");
}

void loop()
{
  if(digitalRead(KEY_LEAVE)==LOW)
  {
    triggerLeaveEvent();
    delay(500);
  }

  if(digitalRead(KEY_HOME)==LOW)
  {
    triggerHomeEvent();
    delay(500);
  }
}
