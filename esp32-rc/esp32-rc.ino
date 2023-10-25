//
//  RC controller
//
#include <Arduino.h>
#include <Arduino_JSON.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebSrv.h>
#include <ESP32Servo.h>
#include <L298N.h>

#define LED 2
#define M_IN_1 33
#define M_IN_2 32
#define SERVO_PIN 25
#define MOTOR_PIN 26


enum curren_state {
  ON_STATE,
  OFF_STATE,
};

enum cmd_code {
  ON,
  OFF,
  TURN,
  STOP,
  FORWARD,
  BACKWARD
};

Servo myservo;
L298N myMotor(MOTOR_PIN, M_IN_1, M_IN_2);
int last_pos = 90;
int new_pos = 90;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
// wifi名称
const char *ssid = "AOETECH";
// wifi 密码
const char *password = "67810550";
curren_state current = OFF_STATE;


// uint8_t to string
String converter(uint8_t *str) {
  return String((char *)str);
}

// websocket 事件监听
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  JSONVar myObject;
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      myObject = JSON.parse(converter(data));
      onCommond(myObject);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

cmd_code hashit(String inString) {
  if (inString == "ON") return ON;
  if (inString == "OFF") return OFF;
  if (inString == "STOP") return STOP;
  if (inString == "TURN") return TURN;
  if (inString == "FORWARD") return FORWARD;
  if (inString == "BACKWARD") return BACKWARD;
}

// 处理远程命令
void onCommond(JSONVar obj) {
  String CMD = String((const char *)obj["COMMOND"]);
  int val = (int)obj["VALUE"];
  Serial.println(CMD);
  Serial.println(val);
  switch (hashit(CMD)) {
    case ON:
      current = ON_STATE;
      digitalWrite(LED, HIGH);
    case OFF:
      current = OFF_STATE;
      digitalWrite(LED, LOW);
      myMotor.stop();
      break;
    case STOP:
      if (current == OFF_STATE) return;
      myMotor.stop();
      break;
    case FORWARD:
      if (current == OFF_STATE) return;
      myMotor.setSpeed(val);
      myMotor.forward();
      break;
    case BACKWARD:
      if (current == OFF_STATE) return;
      myMotor.setSpeed(val);
      myMotor.backward();
      break;
    case TURN:
      if (current == OFF_STATE) return;
      new_pos = val;
      break;
  }
}

// 初始化websocket
void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

// 初始化wifi
void initWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi Failed!\n");
    return;
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// 初始化引脚
void initPin() {
  pinMode(LED, OUTPUT);
  myservo.setPeriodHertz(50);
  myservo.attach(SERVO_PIN);
  myservo.write(last_pos);
  delay(15);
  myMotor.setSpeed(0);
}

void turn() {
  if (current == OFF_STATE) return;
  if (new_pos != last_pos) {
    if (last_pos > new_pos) {
      last_pos -= 1;
    }
    if (last_pos < new_pos) {
      last_pos += 1;
    }
  }
  myservo.write(last_pos);
  delay(10);
}

void setup() {
  Serial.begin(115200);
  initPin();
  initWifi();
  initWebSocket();
  server.begin();
}

void loop() {
  turn();
}