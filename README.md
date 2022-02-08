
# SevyCore

![Version](https://img.shields.io/badge/version-v0.1.0-lightgrey)

![License](https://img.shields.io/badge/license-Apache%202.0-lightgrey)

![Architecture](https://img.shields.io/badge/architecture-ESP8266-red)

![Framework](https://img.shields.io/badge/framework-Arduino-blue)

![Build Status](https://img.shields.io/badge/build-passing-green)

Sevy functions library for Espressif ESP8266 microcontroller, to help build a firmware iot devices.

**This works with Espressif ESP8266 microcontroller, arduino platform and ArduinoJson v6.19.1 library need to be installed**

[https://github.com/esp8266/Arduino](https://github.com/esp8266/Arduino)

[https://github.com/bblanchon/ArduinoJson](https://github.com/bblanchon/ArduinoJson)

## Contents
 - [Using](#Using)
 - [Documentation](#Documentation)
   - [Initialize](#Initialize)
   - [JSONParse](#JSONParse)
   - [EncryptAndDecrypt](#EncryptAndDecrypt)
 - [Features](#Features)
    - [LedStatus](#LedStatus)
    - [ResetButton](#ResetButton)
    - [WifiConnectionStatus](#WifiConnectionStatus)
 - [Releases](#Releases)
    - [0.1.0](#0.1.0)

### Using
- Include in your sketch
```cpp
#include <SevyCore.h>          //https://github.com/YvesYuri/SevyCore
```

- Initialize identification variables, before setup function. So create a SevyCore library instance with the variables created.
```cpp
int ledStatusPin = 0;
int resetButtonPin = 15;
String hostname = "deviceName";
uint8_t macAddress[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

SevyCore sevyCore(ledStatusPin, resetButtonPin, hostname, macAddress);
```

- Also in the setup function add
```cpp
sevyCore.initialize();
```

After you write your sketch and start the device, it will checks if there is already saved credential. If yes, it will try to connect to the wifi network using the saved credentials, else it starts Espressif SmartConfig Protocol.

Also see examples

## Documentation

#### Initialize
Use this to initialize the library boot process. It will checks if there is already saved credential. If yes, it will try to connect to the wifi network using the saved credentials, else it starts Espressif SmartConfig Protocol. 
```cpp
initialize();
```

#### JSONParse
The toJson(jsonString) method parses a JSON string, constructing the JavaScript value or object described by the string. Returns StaticJsonDocument<600> object.
```cpp
toJson(String jsonString);
```

#### EncryptAndDecrypt
Two functions to data encryption by a security key generated and created after the finish Espressif SmartConfig Protocol . Provides encryption and decryption functions and return strings. This version of the library supports AES-128-ECB encryption mode. 
```cpp
sevyCore.encrypt(String plainText);
sevyCore.decrypt(String plainText);
```

## Features

#### LedStatus
`(ledStatusPin)` set the microcontroller pin Led blink every 200 milliseconds when trying to connect to an access point or when disconnected. Blink stops when connect to access point.

#### ResetButton
`(resetButtonPin)` set the microcontroller pin  to reset device pushed after 10 seconds.

#### WifiConnectionStatus
After `initialize()` function end, a wifi connection check routine is started. When the connection falls, microcontroller reset.

## Releases
### 0.1.0

### Development Overview

#### Added Public Methods
`sevyCore(int ledPin, int resetPin, String hostName, uint8_t macAddress[])`
`initialize()`
`toJson(String jsonString)`
`encrypt(String plainText)`
`decrypt(String plainText)`
