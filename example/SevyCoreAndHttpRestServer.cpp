/**
 * @file SevyCoreAndHttpRestServer.cpp
 * @author Yves Yuri (yyosilva@hotmail.com)
 * @brief
 * @version 0.1.0
 * @date 2022-01-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <SevyCore.h>

int ledStatusPin = 0;
int resetButtonPin = 15;
int switcherPin = 16;
String hostname = "deviceName";
uint8_t macAddress[] = {0x32, 0xAE, 0xA4, 0x07, 0x0D, 0x66};

SevyCore sevyCore(ledStatusPin, resetButtonPin, hostname, macAddress);
ESP8266WebServer httpRestServer(80);

void routeFunction()
{
    String request = sevyCore.toJson(sevyCore.decrypt(httpRestServer.arg("plain")))["filed"];

    Serial.print(request);
}

void httpRestServerRouting()
{
    httpRestServer.on("/route", HTTP_POST, routeFunction);
}

void setup()
{
    sevyCore.initialize();
    httpRestServerRouting();
    httpRestServer.begin();
}

void loop()
{
    httpRestServer.handleClient();
}