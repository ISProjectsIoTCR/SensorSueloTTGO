//___________________LIBRERIAS_______________________
#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>
#include <DHT.h>
#include <NTPClient.h>
#include <SD.h>
#include <SPI.h>
#include <ESP.h>
#include <time.h>
#include <TimeLib.h>
#include "driver/adc.h"
#include <esp_wifi.h>
#include <esp_bt.h>
#include "user-variables.h"
#include <18B20_class.h>
#include <Adafruit_BME280.h>
#include "SPIFFS.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "Colors.h"

const String rel = "4.3.2"; 

//___________________INSTANCIAS_______________________
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
HTTPClient http;

// Date calculator
unsigned long epochTime;
String battChargeEpoc;
unsigned long epochChargeTime;
float battChargeDateDivider = 86400;
float daysOnBattery;
// Reboot counters
RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR int sleep5no = 0;
//Sensor bools
bool bme_found = false;

long varsLastSend[20];
String last_received_msg = "";
String last_received_topic = "";
int prev_temp = 0;
int prev_hum = 0;

//json construct setup
struct Config
{
  String date;
  String time;
  int bootno;
  int sleep5no;
  float lux;                                                               //luz
  float temp;                                                      //temperatura
  float humid;                                                         //humedad
  float soil;                                                           //tierra
  float soilTemp;
  float salt;                                                        //minerales
  String saltadvice;                                     //advertencia minerales
  float bat;                
  String batcharge;
  String batchargeDate;
  float daysOnBattery;
  float batvolt;
  float batvoltage;
  float pressure;                                       //no disponible en DHT11
  String rel;
  String soilHum;
};
Config config;

const int led = 13;

#define I2C_SDA 25
#define I2C_SCL 26
#define DHT_PIN 16
#define BAT_ADC 33
#define SALT_PIN 34
#define SOIL_PIN 32
#define BOOT_PIN 0
#define POWER_CTRL 4
#define USER_BUTTON 35
#define DS18B20_PIN 21
#define SensorPin 15 

BH1750 lightMeter(0x23); //0x23
Adafruit_BME280 bmp;     //0x77
DHT dht(DHT_PIN, DHT_TYPE);
DS18B20 temp18B20(DS18B20_PIN);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
String formattedDate;
String dayStamp;
String timeStamp1;

// Start Subroutines
#include <file-management.h>
#include <go-to-deep-sleep.h>
#include <get-string-value.h>
#include <read-sensors.h>
#include <connect-to-network.h>
#include <read-batt-info.h>
#include <floatConv.h>
#include <iotcrv2-conector.h>


//_________________________SET-UP_______________________
void setup()
{
  Serial.begin(115200);
  Serial.println("Void Setup");
  #include <module-parameter-management.h>
  //#include <battChargeDays.h>



  //CALCULOS AMBIENTALES
  if (dht_found)
  {
    dht.begin();
  }
  else
  {
    Serial.println(F("Could not find a valid DHT sensor, check if there is one present on board!"));
  }

  //! Sensor power control pin , use deteced must set high
  pinMode(POWER_CTRL, OUTPUT);
  digitalWrite(POWER_CTRL, 1);
  delay(1000);
  //wire can not be initialized at beginng, the bus is busy
  bool wireOk = Wire.begin(I2C_SDA, I2C_SCL);
  if (wireOk)
  {
    Serial.println(F("Wire ok"));
    if (logging)
    {
      writeFile(SPIFFS, "/error.log", "Wire Begin OK! \n");
    }
  }
  else
  {
    Serial.println(F("Wire NOK"));
  }

  if (!bmp.begin())
  {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    bme_found = false;
  }
  else
  {
    bme_found = true;
  }

  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE))
  {
    Serial.println(F("BH1750 Advanced begin"));
  }
  else
  {
    Serial.println(F("Error initialising BH1750"));
  }

  float luxRead = lightMeter.readLightLevel(); // 1st read seems to return 0 always
  Serial.print("lux ");
  Serial.println(luxRead);
  delay(2000);

  if (dht_found)
  {
    float t12 = dht.readTemperature(); // Read temperature as Fahrenheit then dht.readTemperature(true)
    config.temp = t12;
    delay(2000);
    float h12 = dht.readHumidity();
    config.humid = h12;
  }

  if (bme_found)
  {
    float bme_temp = bmp.readTemperature();
    config.temp = bme_temp;

    float bme_humid = bmp.readHumidity();
    config.humid = bme_humid;

    float bme_pressure = (bmp.readPressure() / 100.0F);
    config.pressure = bme_pressure;
  }

  //CALCULOS DEL SUELO
  uint16_t soil = readSoil();
  config.soil = soil;
  float soilTemp = readSoilTemp();
  config.soilTemp = soilTemp;
  float soilHum = readSoilHum();
  config.soilHum = soilHum;
  Serial.print("el valor de la humedad del suelo es: ");
  Serial.print(config.soilHum);


//CALCULAR FETILIZANTE
  uint32_t salt = readSalt();
  config.salt = salt;
  String advice;
  if (salt < 50)
  {
    advice = "Nivel muy bajo de fertilizante";
  }
  else if (salt < 100)
  {
    advice = "Fertilizante bajo";
  }
  else if (salt < 200)
  {
    advice = "Fertilizante optimo";
  }
  else if (salt > 200)
  {
    advice = "Fertilizante muy Alto";
  }
  Serial.println(advice);
  config.saltadvice = advice;



  //CALCULOS DE LA BATERIA, ESTADO DE LA CARGA Y DÍAS.
  float bat = readBattery();
  config.bat = bat;
  config.batcharge = "";
  Serial.println("Battery level");
  Serial.println(bat);
  if (bat > 130)
  {
    config.batcharge = "cargando batería";
    SPIFFS.remove("/batinfo.conf");
    epochChargeTime = timeClient.getEpochTime();
    battChargeEpoc = String(epochChargeTime) + ":" + String(dayStamp);
    const char *batinfo_write = battChargeEpoc.c_str();
    writeFile(SPIFFS, "/batinfo.conf", batinfo_write);
    Serial.println("dayStamp");
    Serial.println(dayStamp);
    config.batchargeDate = dayStamp;
  }
  Serial.println("Charge Epoc");
  Serial.println(battChargeEpoc);
  unsigned long epochTime = timeClient.getEpochTime();
  Serial.println("Test Epoc");
  Serial.println(epochTime);
  epochChargeTime = battChargeEpoc.toInt();
  Serial.println("first calculation");
  Serial.println(epochTime - epochChargeTime);
  float epochTimeFl = float(epochTime);
  float epochChargeTimeFl = float(epochChargeTime);
  daysOnBattery = (epochTimeFl - epochChargeTimeFl) / battChargeDateDivider;
  daysOnBattery = truncate(daysOnBattery, 1);
  config.daysOnBattery = daysOnBattery;
  if (bat > 100)
  {
    config.bat = 100;
  }
  config.bootno = bootCount;
  luxRead = lightMeter.readLightLevel();
  Serial.print("lux ");
  Serial.println(luxRead);
  config.lux = luxRead;
  config.rel = rel;

  
  //CREAR EL ARCHIVO JSON
  Serial.println(F("Creating JSON document..."));
  if (logging)
  {
    writeFile(SPIFFS, "/error.log", "Creating JSON document...! \n");
  }



  //INICIA WIFI Y ACTUALIZAR HORA
  delay(3000);
  connectToNetwork();
  Serial.println(" ");
  Serial.println("Connected to network");
  if (logging)
  {
    writeFile(SPIFFS, "/error.log", "Connected to network \n");
  }
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
  //  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  //  timeClient.setTimeOffset(7200);
  timeClient.setTimeOffset(gmtOffset_sec);
  while (!timeClient.update())
  {
    timeClient.forceUpdate();
  }
  #include <time-management.h>



  //ENVIAR A IOTPROJECTS
  dataReceiver(config);



  //IR A DORMIR
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  delay(1000);
  goToDeepSleep();
}


//________________________________LOOP___________________
void loop()
{
  //never is used
}
