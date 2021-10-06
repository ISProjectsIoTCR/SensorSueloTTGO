void goToDeepSleep()
{
  Serial.print("Going to sleep... ");
  Serial.print(TIME_TO_SLEEP);
  Serial.println(" seconds");
  if (logging)
  {
    writeFile(SPIFFS, "/error.log", "Going to sleep for 10800 seconds \n");
  }

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  btStop();

  WRITE_PERI_REG(SENS_SAR_START_FORCE_REG, reg_a); // fix ADC registers
  WRITE_PERI_REG(SENS_SAR_READ_CTRL2_REG, reg_b);
  WRITE_PERI_REG(SENS_SAR_MEAS_START2_REG, reg_c);

  // Configure the timer to wake us up!
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  // Testpurposes
  //esp_sleep_enable_timer_wakeup(10 * uS_TO_S_FACTOR);

  if (logging)
  {
    writeFile(SPIFFS, "/error.log", "Going to deep sleep \n \n \n");
  }

  // Go to sleep! Zzzz
  esp_deep_sleep_start();
}
void goToDeepSleepFiveMinutes()
{
  Serial.print("Going to sleep... ");
  Serial.print("300");
  Serial.println(" sekunder");
  if (logging)
  {
    writeFile(SPIFFS, "/error.log", "Going to sleep for 300 seconds \n");
  }

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  btStop();


  adc_power_off();
  esp_wifi_stop();
  esp_bt_controller_disable();

  WRITE_PERI_REG(SENS_SAR_START_FORCE_REG, reg_a); // fix ADC registers
  WRITE_PERI_REG(SENS_SAR_READ_CTRL2_REG, reg_b);
  WRITE_PERI_REG(SENS_SAR_MEAS_START2_REG, reg_c);

  // Configure the timer to wake us up!
  ++sleep5no;
  esp_sleep_enable_timer_wakeup(30 * uS_TO_S_FACTOR);

  // Go to sleep! Zzzz
  esp_deep_sleep_start();
}
