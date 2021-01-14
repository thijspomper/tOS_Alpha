////////////////////////////////////////////////////////////////Connection settings
#ifndef STASSID
#define STASSID "xxxx"
#define STAPSK  "xxxx"
#define MQQTIP  "xxxx"
#define mUser   "xxxx"
#define mPass   "xxxx"
#define cID     "tOS_Alpha"
#endif
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////Dallas settings
#define ONE_WIRE_BUS D2
#define TEMPERATURE_PRECISION 12

const int bufferSize = 5;
const int sensorDelay = 100;
const int updateDelay = 20000;

DeviceAddress thijs = { 0x28, 0xEE, 0xA2, 0xC9, 0x2C, 0x16, 0x02, 0xEB };
DeviceAddress jort   = { 0x28, 0xEE, 0x50, 0x27, 0x2E, 0x16, 0x01, 0x3F };
DeviceAddress jw = { 0x28, 0xEE, 0xDE, 0xC9, 0x2C, 0x16, 0x02, 0xA7 };
DeviceAddress zolderkamer = { 0x28, 0xFF, 0xBC, 0x7E, 0xB0, 0x16, 0x03, 0x83 };
DeviceAddress warmWater = { 0x28, 0xFF, 0x6B, 0xC0, 0x22, 0x17, 0x04, 0x28 };
DeviceAddress aanvoerCV = { 0x28, 0xFF, 0x1E, 0x11, 0x94, 0x16, 0x05, 0xEE };
DeviceAddress retourCV = { 0x28, 0xFF, 0x59, 0xC2, 0x22, 0x17, 0x04, 0x28 };
DeviceAddress aanvoerWTW = { 0x28, 0xFF, 0x04, 0xC5, 0x22, 0x17, 0x04, 0xD7 };
DeviceAddress afvoerWTW = { 0x28, 0xFF, 0xD6, 0x95, 0x22, 0x17, 0x04, 0xD2 };


const char* tThijs = "thijs/sensor/temp";
const char* tJort = "jort/sensor/temp";
const char* tJw = "jw/sensor/temp";
const char* tZolderkamer = "zolder/sensor/tempKamer";
const char* tWarmWater = "zolder/sensor/tempWarmWater";
const char* tAanvoerCV = "zolder/sensor/tempAanvoerCV";
const char* tRetourCV = "zolder/sensor/tempRetourCV";
const char* tAanvoerWTW = "zolder/sensor/tempAanvoerWTW";
const char* tAfvoerWTW = "zolder/sensor/tempAfvoerWTW";

uint8_t* dallasSensorList[] = {thijs, jort, jw, zolderkamer, warmWater, aanvoerCV, retourCV, aanvoerWTW, afvoerWTW}; 

const char* dallasTopicList[] = {tThijs, tJort, tJw, tZolderkamer, tWarmWater, tAanvoerCV, tRetourCV, tAanvoerWTW, tAfvoerWTW};
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////DHT settings
DHT wtw1(D5, DHT22);
DHT wtw2(D6, DHT22);
DHT woonkamer(D1, DHT22);


const char* tWtw1 = "zolder/sensor/tempWtw1";
const char* hWtw1 = "zolder/sensor/humWtw1";
const char* tWtw2 = "zolder/sensor/tempWtw2";
const char* hWtw2 = "zolder/sensor/humWtw2";
const char* tWoonkamer = "woonkamer/sensor/temp";
const char* hWoonkamer = "woonkamer/sensor/hum";

DHT dhtSensorList[] = {wtw1, wtw2, woonkamer}; 

const char* dhtTopicList[] = {tWtw1, hWtw1, tWtw2, hWtw2, tWoonkamer, hWoonkamer};
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////Global settings

////////////////////////////////////////////////////////////////
