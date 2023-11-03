//
//  RC controller
//
#include <Arduino.h>
#include <Arduino_JSON.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "esp_wifi.h"
#include "Esc.h"

#define LED 2
#define SERVO_PIN 25
#define MOTOR_PIN 26

enum curren_state
{
  ON_STATE,
  OFF_STATE,
};

enum cmd_code
{
  ON,
  OFF,
  TURN,
  STOP,
  FORWARD,
  BACKWARD,
  SET_SERVO_STEP,
  SET_MOTOR_CONFIG,
};

Esc motorEsc;
int motorEscFreq = 50;
int motorEscChannel = 1;

Esc servoEsc;

int last_pos = 90;
int new_pos = 90;
int led_state = 0;
bool reconning = false;
int servoStep = 1;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
// wifi名称
const char *ssid = "RC_C";
// wifi 密码
const char *password = "15827330290";
unsigned long previousMillis = 0;
unsigned long interval = 10000;
unsigned long previousMillisLed = 0;
unsigned long intervalLed = 200; // 未连接wifi 200ms 闪烁 连接wifi未连接遥控 1500ms 闪烁 都连接 长亮

curren_state current = OFF_STATE;

// uint8_t to string
String converter(uint8_t *str)
{
  return String((char *)str);
}

cmd_code hashit(String inString)
{
  if (inString == "ON")
    return ON;
  if (inString == "OFF")
    return OFF;
  if (inString == "STOP")
    return STOP;
  if (inString == "TURN")
    return TURN;
  if (inString == "FORWARD")
    return FORWARD;
  if (inString == "BACKWARD")
    return BACKWARD;
  if (inString == "SET_SERVO_STEP")
    return SET_SERVO_STEP;
  if (inString == "SET_MOTOR_CONFIG")
    return SET_MOTOR_CONFIG;
  return STOP;
}

// 启动引擎
void launchEngine()
{
  Serial.println("launch engine");
  current = ON_STATE;
}

// 关闭引擎
void closeEngine()
{
  if (current == OFF_STATE)
    return;
  Serial.println("close engine");
  current = OFF_STATE;
}

// 电机停止
void motorStop()
{
  if (current == OFF_STATE)
    return;
  motorEsc.write(motorEsc.resolutionMiddle);
}

// 电机正转
void motorForward(int val)
{
  if (current == OFF_STATE)
    return;
  motorEsc.write(val, 0, 255, motorEsc.resolutionMiddle + 1, motorEsc.resolution);
}

// 电机反转
void motorBackward(int val)
{
  if (current == OFF_STATE)
    return;
  motorEsc.write(255 - val, 0, 255, 0, motorEsc.resolutionMiddle - 1);
}

// 处理远程命令
void onCommond(JSONVar obj)
{
  String CMD = String((const char *)obj["COMMOND"]);
  int val = (int)obj["VALUE"];

  // Serial.print(CMD);
  // Serial.print(" -- ");
  // Serial.print(val);
  // Serial.println();

  switch (hashit(CMD))
  {
  case ON:
    launchEngine();
    break;
  case OFF:
    closeEngine();
    break;
  case STOP:
    Serial.println("STOP MOTOR: ");
    Serial.println(val);
    motorStop();
    break;
  case FORWARD:
    Serial.print("FORWARD: ");
    Serial.println(val);
    motorForward(val);
    break;
  case BACKWARD:
    Serial.print("BACKWARD: ");
    Serial.println(val);
    motorBackward(val);
    break;
  case SET_SERVO_STEP:
    Serial.print("SET_SERVO_STEP: ");
    Serial.println(val);
    servoStep = val;
    break;
  case SET_MOTOR_CONFIG:
    Serial.print("SET_MOTOR_CONFIG: ");
    Serial.println(val);
    motorEsc.setUp(motorEscChannel, val, TIMER_10_BIT);
    servoStep = val;
    break;
  case TURN:
    if (current == OFF_STATE)
      return;
    new_pos = val;
    break;
  }
}

// websocket 事件监听
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  JSONVar myObject;
  switch (type)
  {
  case WS_EVT_CONNECT:
    intervalLed = -1;
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    closeEngine();
    intervalLed = 1500;
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

// 初始化websocket
void initWebSocket()
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

// 初始化wifi
void initWifi()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.printf("WiFi Failed!\n");
    return;
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  intervalLed = 1500;
}

// 初始化引脚
void initPin()
{
  pinMode(LED, OUTPUT);

  servoEsc.setUp(0);
  servoEsc.attachPin(SERVO_PIN);
  servoEsc.write(90, 0, 180, 500, 2500);

  motorEsc.setUp(motorEscChannel);
  motorEsc.attachPin(LED);
}

int turnInterval = 6;
int previousTurnMillis = 10;

void turn()
{
  if (current == OFF_STATE)
    return;
  unsigned long currentMillis = millis();
  if (currentMillis - previousTurnMillis >= (turnInterval * servoStep))
  {
    if (new_pos != last_pos)
    {
      int diff = abs(new_pos - last_pos);
      int offset = servoStep;
      if (diff < offset)
      {
        offset = diff;
      }
      if (last_pos > new_pos)
      {
        last_pos -= offset;
      }
      if (last_pos < new_pos)
      {
        last_pos += offset;
      }
      Serial.print("TRUN: ");
      Serial.println(last_pos);
      servoEsc.write(last_pos, 0, 180, 500, 2500);
      previousMillis = currentMillis;
    }
  }
}

void checkWifiState()
{
  unsigned long currentMillis = millis();
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval))
  {
    closeEngine();
    intervalLed = 200;
    reconning = true;
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }

  if (reconning && WiFi.status() == WL_CONNECTED)
  {
    intervalLed = 1500;
    reconning = false;
    Serial.println(WiFi.localIP());
    Serial.println("reconnect success");
  }
}

void checkLed()
{
  if (intervalLed == -1 && led_state == 0)
  {
    digitalWrite(LED, HIGH);
    led_state = 1;
    return;
  }
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillisLed >= intervalLed)
  {
    if (led_state == 0)
    {
      digitalWrite(LED, HIGH);
      led_state = 1;
    }
    else
    {
      digitalWrite(LED, LOW);
      led_state = 0;
    }
    previousMillisLed = currentMillis;
  }
}

void setup()
{
  initPin();
  Serial.begin(115200);
  initWifi();
  esp_wifi_set_max_tx_power(5);
  initWebSocket();
  server.begin();
}

void loop()
{
  turn();
  checkWifiState();
  checkLed();
}