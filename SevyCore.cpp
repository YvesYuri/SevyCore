/**
 * @file SevyCore.cpp
 * @author Yves Yuri (yyosilva@hotmail.com)
 * @brief
 * @version 0.1.0
 * @date 2022-01-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <SevyCore.h>

// TODO: Add variables on header file
// start variables
int _ledPin;
int _resetPin;
unsigned long _pressedTime;
int _currentState;
int _lastState = LOW;
int _resetTimer = 10000;
int _configurationDataCommitEepromAddress = 0;
const char *_encryptionKey;
bool _setedEncryptionKey = false;
ESP8266WebServer configurationHttpRestServer(80);
// end variables

SevyCore::SevyCore(int ledPin, int resetPin, String hostName, uint8_t macAddress[])
{
    _ledPin = ledPin;
    _resetPin = resetPin;
    WiFi.hostname(hostName.c_str());
    wifi_set_macaddr(STATION_IF, &macAddress[0]);
}

bool SevyCore::hasCommitedConfigurationData()
{
    EEPROM.begin(512);
    int resultData = EEPROM.read(_configurationDataCommitEepromAddress);
    bool result = resultData == 1 ? true : false;
    return result;
}

void SevyCore::commitConfigurationData()
{
    EEPROM.begin(512);
    EEPROM.write(_configurationDataCommitEepromAddress, 1);
    EEPROM.end();
}

void SevyCore::restart()
{
    ESP.restart();
}

ConfigurationModelData SevyCore::getConfigurationModelData()
{
    EEPROM.begin(512);
    ConfigurationModelData result = EEPROM.get<ConfigurationModelData>(1, _configurationModelData);
    return result;
}

void SevyCore::setConfigurationModelData(ConfigurationModelData configurationModelData)
{
    EEPROM.begin(512);
    EEPROM.put<ConfigurationModelData>(1, configurationModelData);
    EEPROM.commit();
}

bool SevyCore::wifiSmartConfig()
{
    bool result = false;
    WiFi.mode(WIFI_STA);
    WiFi.beginSmartConfig();
    while (1)
    {
        delay(500);
        if (WiFi.smartConfigDone())
        {
            result = true;
            delay(500);
            break;
        }
    }
    return result;
}

void SevyCore::initialize()
{
    pinMode(_resetPin, INPUT);
    pinMode(_ledPin, OUTPUT);

    _reset.attach(0.5, resetButtonVerify);
    _ledStatus.attach(0.2, ledBlink);

    delay(500);

    if (hasCommitedConfigurationData())
    {
        connectTo(getConfigurationModelData());
    }
    else
    {
        sync();
        restart();
    }

    _ledStatus.detach();
    digitalWrite(_ledPin, HIGH);
    _wifiConnection.attach(1, connectionVerify);
    configurationHttpRestServer.stop();
}

void SevyCore::connectTo(ConfigurationModelData configurationModelData)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(configurationModelData.wifiSsid, configurationModelData.wifiPass);
    _timeOut.attach(10, restart);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
    }
    _timeOut.detach();
}

void SevyCore::sync()
{
    if (wifiSmartConfig())
    {
        ConfigurationModelData configurationModelData;
        configurationHttpRestServerRouting();
        configurationHttpRestServer.begin();
        while (1)
        {
            delay(500);
            configurationHttpRestServer.handleClient();
            if (_setedEncryptionKey)
            {
                delay(500);
                break;
            }
        }
        strcpy(configurationModelData.wifiSsid, WiFi.SSID().c_str());
        strcpy(configurationModelData.wifiPass, WiFi.psk().c_str());
        strcpy(configurationModelData.encryptionKey, _encryptionKey);
        setConfigurationModelData(configurationModelData);
        commitConfigurationData();
    }
}

void SevyCore::ledBlink()
{
    int state = digitalRead(_ledPin);
    digitalWrite(_ledPin, !state);
}

void SevyCore::connectionVerify()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        restart();
    }
}

void SevyCore::reset()
{
    EEPROM.begin(512);
    for (int i = 0; i < 512; i++)
    {
        EEPROM.write(i, 0);
    }
    EEPROM.end();
    restart();
}

void SevyCore::resetButtonVerify()
{

    _currentState = digitalRead(_resetPin);

    if (_lastState == HIGH && _currentState == HIGH)
    {
        long pressDuration = millis() - _pressedTime;
        if (pressDuration > _resetTimer)
        {
            reset();
        }
    }

    else if (_lastState == LOW && _currentState == HIGH)
    {
        _pressedTime = millis();
    }
    _lastState = _currentState;
}

StaticJsonDocument<600> SevyCore::toJson(String plainText)
{
    StaticJsonDocument<600> jsonDocument;
    deserializeJson(jsonDocument, plainText);
    return jsonDocument;
}

void SevyCore::setEncryptionKey()
{
    const char *encryptionKey = toJson(configurationHttpRestServer.arg("plain"))["key"];
    configurationHttpRestServerStatus(201);
    _encryptionKey = encryptionKey;
    _setedEncryptionKey = true;
}

void SevyCore::configurationHttpRestServerRouting()
{
    configurationHttpRestServer.on("/encryptionKey", HTTP_POST, setEncryptionKey);
}

void SevyCore::configurationHttpRestServerStatus(int statusCode)
{
    configurationHttpRestServer.send(statusCode);
}

String SevyCore::encrypt(String plainText)
{
    String res = _encryption.encrypt(plainText, getConfigurationModelData().encryptionKey);
    return res;
}

String SevyCore::decrypt(String plainText)
{
    String res = _encryption.decrypt(plainText, getConfigurationModelData().encryptionKey);
    return res;
}
