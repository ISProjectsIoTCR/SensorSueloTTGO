// READ Sensors

// READ Salt
// I am not quite sure how to read and use this number. I know that when put in water wich a DH value of 26, it gives a high number, but what it is and how to use ??????
uint32_t readSalt()
{
  uint8_t samples = 120;
  uint32_t humi = 0;
  uint16_t array[120];

  for (int i = 0; i < samples; i++)
  {
    array[i] = analogRead(SALT_PIN);

    delay(2);
  }
  std::sort(array, array + samples);
  for (int i = 0; i < samples; i++)
  {
    if (i == 0 || i == samples - 1)
      continue;
    humi += array[i];
  }
  humi /= samples - 2;
  return humi;
}

// READ Soil
uint16_t readSoil()
{
  Serial.println(soil_max);
  uint16_t soil = analogRead(SOIL_PIN);
  Serial.print("Soil before map: ");
  Serial.println(soil);
  return map(soil, soil_min, soil_max, 100, 0);
}

// READ SOIL TEMPERATURE
float readSoilTemp()
{
  float temp;
  if (USE_18B20_TEMP_SENSOR)
  {
    //Single data stream upload
    temp = temp18B20.temp();
  }
  else
  {
    temp = 0.00;
  }
  return temp;
}


// READ SOIL HUMIDITY
float readSoilHum()
{
  uint8_t samples = 120;
  uint32_t hum = 1;
  uint16_t array[120];

  for (int i = 0; i < samples; i++)
  {
    array[i] = analogRead(SensorPin);
    Serial.println(array[i]);
    delay(5);
  }
  std::sort(array, array + samples);
  for (int i = 0; i < samples; i++)
  {
    if (i == 0 || i == samples - 1)
      continue;
    hum += array[i];
  }
  hum /= samples - 2;
  hum = map(hum,900,4095,100,0);
  return hum;

  //float sensorValue = analogRead(15);
  //sensorValue = map(sensorValue,900,4095,100,0);
  //delay(3000);
  //Serial.println(sensorValue);
  //return sensorValue;
}

// READ Battery
float readBattery()
{
  int vref = 1100;
  uint16_t volt = analogRead(BAT_ADC);
  Serial.print("Volt direct ");
  Serial.println(volt);
  config.batvolt = volt;
  float battery_voltage = ((float)volt / 4095.0) * 2.0 * 3.3 * (vref) / 1000;
  config.batvoltage = battery_voltage;
  Serial.print("Battery Voltage: ");
  Serial.println(battery_voltage);
  battery_voltage = battery_voltage * 100;
  return map(battery_voltage, 416, 290, 100, 0);
}