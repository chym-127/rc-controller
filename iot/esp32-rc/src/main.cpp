//
//  RC controller
//
#include <Arduino.h>
#include <Arduino_JSON.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP32Servo.h>
#include <L298N.h>

#define LED 2
#define M_IN_1 33
#define M_IN_2 32
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
  BACKWARD
};

Servo myservo;
L298N myMotor(MOTOR_PIN, M_IN_1, M_IN_2);
int last_pos = 90;
int new_pos = 90;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
// wifi名称
const char *ssid = "RC_C";
// const char *ssid = "AOETECH";
// wifi 密码
const char *password = "15827330290";
// const char *password = "67810550";

// static ip
IPAddress local_IP(192, 168, 133, 100);
IPAddress gateway(192, 168, 133, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

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
  return STOP;
}

// 启动引擎
void launchEngine()
{
  Serial.println("launch engine");
  current = ON_STATE;
  digitalWrite(LED, HIGH);
}

// 关闭引擎
void closeEngine()
{
  Serial.println("close engine");
  current = OFF_STATE;
  digitalWrite(LED, LOW);
  myMotor.stop();
}

// 电机停止
void motorStop()
{
  if (current == OFF_STATE)
    return;
  myMotor.stop();
}

// 电机正转
void motorForward(int val)
{
  if (current == OFF_STATE)
    return;
  myMotor.setSpeed(val);
  myMotor.forward();
}

// 电机反转
void motorBackward(int val)
{
  if (current == OFF_STATE)
    return;
  myMotor.setSpeed(val);
  myMotor.backward();
}

// 处理远程命令
void onCommond(JSONVar obj)
{
  String CMD = String((const char *)obj["COMMOND"]);
  int val = (int)obj["VALUE"];
  Serial.print(CMD);
  Serial.print(" -- ");
  Serial.print(val);
  Serial.println();

  switch (hashit(CMD))
  {
  case ON:
    launchEngine();
    break;
  case OFF:
    closeEngine();
    break;
  case STOP:
    motorStop();
    break;
  case FORWARD:
    motorForward(val);
    break;
  case BACKWARD:
    motorBackward(val);
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
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    closeEngine();
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
  // if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
  //   Serial.println("STA Failed to configure");
  // }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.printf("WiFi Failed!\n");
    return;
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// 初始化引脚
void initPin()
{
  pinMode(LED, OUTPUT);
  myservo.setPeriodHertz(50);
  myservo.attach(SERVO_PIN);
  myservo.write(last_pos);
  delay(15);
  myMotor.setSpeed(0);
}

void turn()
{
  if (current == OFF_STATE)
    return;
  if (new_pos != last_pos)
  {
    if (last_pos > new_pos)
    {
      last_pos -= 1;
    }
    if (last_pos < new_pos)
    {
      last_pos += 1;
    }
  }
  myservo.write(last_pos);
  delay(10);
}

void setup()
{
  Serial.begin(115200);
  initPin();
  initWifi();
  initWebSocket();
  server.begin();
}

void loop()
{
  turn();
}