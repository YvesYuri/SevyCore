/**
 * @file SevyCore.h
 * @author Yves Yuri (yyosilva@hotmail.com)
 * @brief
 * @version 0.1.0
 * @date 2022-01-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef _SevyCore_h_
#define _SevyCore_h_

#include <Arduino.h>
#include <EEPROM.h>
#include <Ticker.h>
#include <ConfigurationModelData.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <SevyEncryption.h>

class SevyCore
{
private:
    Ticker _reset;
    Ticker _wifiConnection;
    Ticker _ledStatus;
    Ticker _timeOut;
    ConfigurationModelData _configurationModelData;
    SevyEncryption _encryption;

    bool hasCommitedConfigurationData();
    bool wifiSmartConfig();

    void configurationHttpRestServerRouting();
    void sync();
    void commitConfigurationData();
    void connectTo(ConfigurationModelData configurationModelData);
    void setConfigurationModelData(ConfigurationModelData configurationModelData);

    static void configurationHttpRestServerStatus(int statusCode);
    static void setEncryptionKey();
    static void restart();
    static void reset();
    static void ledBlink();
    static void connectionVerify();
    static void resetButtonVerify();

    ConfigurationModelData getConfigurationModelData();

public:
    SevyCore(int ledPin, int resetPin, String hostName, uint8_t macAddress[]);
    void initialize();
    String encrypt(String plainText);
    String decrypt(String plaintText);

    static StaticJsonDocument<600> toJson(String plaintText);
};

#endif