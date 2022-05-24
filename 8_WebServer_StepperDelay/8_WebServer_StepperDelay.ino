#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <AccelStepper.h>
const char *ssid = "CU_W";
const char *password = "Aa123456";

WebServer server(80);

const int led = 2;
 // X slot
 const int xdirPin = 16;     // D5方向控制引脚
 const int xstepPin = 26;    // D2步进控制引脚
 // Y slot
 const int ydirPin = 27;     // D6方向控制引脚
 const int ystepPin = 25;    // D3步进控制引脚
 // Z slot
 const int zdirPin = 14;     // D7方向控制引脚
 const int zstepPin = 17;    // D4步进控制引脚
// A slot
const int adirPin = 18;    // D13方向控制引脚
const int astepPin = 19;   // D12步进控制引脚
AccelStepper stepperx = AccelStepper(1,xstepPin,xdirPin);
AccelStepper steppery = AccelStepper(1,ystepPin,ydirPin);
AccelStepper stepperz = AccelStepper(1,zstepPin,zdirPin);
AccelStepper steppera = AccelStepper(1,astepPin,adirPin);
const int enablePin = 12;  // D8使能控制引脚

float maxspeed = 1000.0;
float acceleration = 0.0;
float rspeed = 800.0;
bool running = false;

void handleSpeedChange() {
   String message = "";
   String spd = "";
   float speed = 0;

    digitalWrite(led, 1);
    delay(500);
    digitalWrite(led, 0);
    delay(500);
  //  float rspeed = 0;
    if (server.arg("spd") != "")//Parameter 'spd'
    {     
      message = "Speed=" ;
      message += server.arg("spd");//Gets the value of the query parameter
      spd = server.arg("spd");
      speed = spd.toFloat();
      }
  Serial.println(message);
  rspeed = ( speed / 100.0 ) * maxspeed;
  message = rspeed;
  Serial.println("Real time speed = ");
  Serial.println(message);
  server.send(200, "text/plain", "Speed changed");  //Returns the HTTP response
 }

 void handle5() {
   running = !running;
   if (rspeed < 0) { rspeed = 0 - rspeed; }
  server.send(200, "text/plain", "Set run as clockwise"); }

void handle4() {
 running = !running;
  if (rspeed > 0) { rspeed = 0 - rspeed; }
  server.send(200, "text/plain", "Set run as anti-clockwise"); }

void handleled1() {
  for(int i=0; i < 1; i++){
    digitalWrite(led, 0);
    delay(500);
    digitalWrite(led, 1);
    delay(500);
  }
  digitalWrite(led, 0);
  server.send(200, "text/plain", "this works as well");
}

void handleled2() {
for(int i=0;i < 2; i++){
    digitalWrite(led, 0);
    delay(500);
    digitalWrite(led, 1);
    delay(500);
  }
  digitalWrite(led, 0);
  server.send(200, "text/plain", "this works as well");
}

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp32!");
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(xstepPin,OUTPUT);     // Arduino控制A4988步进引脚为输出模式
  pinMode(xdirPin,OUTPUT);      // Arduino控制A4988方向引脚为输出模式
  pinMode(ystepPin,OUTPUT);     // Arduino控制A4988步进引脚为输出模式
  pinMode(ydirPin,OUTPUT);      // Arduino控制A4988方向引脚为输出模式
  pinMode(zstepPin,OUTPUT);     // Arduino控制A4988步进引脚为输出模式
  pinMode(zdirPin,OUTPUT);      // Arduino控制A4988方向引脚为输出模式
  pinMode(astepPin,OUTPUT);     // Arduino控制A4988步进引脚为输出模式
  pinMode(adirPin,OUTPUT);      // Arduino控制A4988方向引脚为输出模式
  pinMode(enablePin,OUTPUT);   // Arduino控制A4988使能引脚为输出模式
  digitalWrite(enablePin,0); // 将使能控制引脚设置为低电平从而让电机驱动板进入工作状态

  digitalWrite(xdirPin, 0);
  digitalWrite(ydirPin, 0);
  digitalWrite(zdirPin, 0);
  digitalWrite(adirPin, 0);
  
  stepperx.setMaxSpeed(maxspeed);     // 设置电机最大速度300 
  stepperx.setAcceleration(acceleration);  // 设置电机加速度20.0  
  steppery.setMaxSpeed(maxspeed);     // 设置电机最大速度300 
  steppery.setAcceleration(acceleration);  // 设置电机加速度20.0  
  stepperz.setMaxSpeed(maxspeed);     // 设置电机最大速度300 
  stepperz.setAcceleration(acceleration);  // 设置电机加速度20.0  
  steppera.setMaxSpeed(maxspeed);     // 设置电机最大速度300 
  steppera.setAcceleration(acceleration);  // 设置电机加速度20.0    
  rspeed = 800.0;//Initial speed

  server.on("/", handleRoot);
  server.on("/1",handleled1);
  server.on("/2",handleled2);
  server.on("/4", handle4);
  server.on("/5", handle5);
  server.on("/speed", handleSpeedChange);
//  server.on("/inline", []() {
//  server.send(200, "text/plain", "this works as well");
//  });
  server.onNotFound(handleNotFound);
  
  if (MDNS.begin("esp32")) { Serial.println("MDNS responder started"); }

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
//  delay(2);//allow the cpu to switch to other tasks
  if( running )
 {
   stepperx.stop();
   steppery.stop();
   stepperz.stop();
   steppera.stop();
 }
  else
  {
  stepperx.enableOutputs();
  steppery.enableOutputs();
  stepperz.enableOutputs();
  steppera.enableOutputs();
  
  stepperx.setCurrentPosition(0);
  steppery.setCurrentPosition(0);
  stepperz.setCurrentPosition(0);
  steppera.setCurrentPosition(0);
  
  stepperx.setSpeed(rspeed);
  steppery.setSpeed(rspeed);
  stepperz.setSpeed(rspeed);
  steppera.setSpeed(rspeed);
  
//  steppery.enableOutputs();
//  steppery.setCurrentPosition(0);
//  steppery.setSpeed(rspeed);
//  
//  stepperz.enableOutputs();
//  stepperz.setCurrentPosition(0);
//  stepperz.setSpeed(rspeed);
//
//  steppera.enableOutputs();
//  steppera.setCurrentPosition(0);
//  steppera.setSpeed(rspeed);

//  stepperx.runSpeed();
//  steppery.runSpeed();
//  stepperz.runSpeed();
//  steppera.runSpeed();
    stepperx.run();
    steppery.run();
    stepperz.run();
    steppera.run();
//  Serial.println(rspeed);
  }
}
