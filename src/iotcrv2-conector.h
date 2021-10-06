bool get_mqtt_credentials();
void connect_to_IoTCRv2();
void send_data_to_broker();

DynamicJsonDocument mqtt_data_doc(2048);


//______________________________DATARECEIVER____________________________________
void dataReceiver(const Config & config) {
   get_mqtt_credentials();
   connect_to_IoTCRv2();


//******CADA POSICIÓN ES UN WIDGET*******
    mqtt_data_doc["variables"][0]["last"]["value"] = 1.0;
    mqtt_data_doc["variables"][0]["last"]["save"] = 1;

    mqtt_data_doc["variables"][1]["last"]["value"] = config.temp;
    mqtt_data_doc["variables"][1]["last"]["save"] = 1;

    mqtt_data_doc["variables"][2]["last"]["value"] = config.humid;
    mqtt_data_doc["variables"][2]["last"]["save"] = 1;

    mqtt_data_doc["variables"][3]["last"]["value"] = config.temp;
    mqtt_data_doc["variables"][3]["last"]["save"] = 1;

    mqtt_data_doc["variables"][4]["last"]["value"] = config.lux;
    mqtt_data_doc["variables"][4]["last"]["save"] = 1;

    mqtt_data_doc["variables"][5]["last"]["value"] =config.salt;
    mqtt_data_doc["variables"][5]["last"]["save"] = 1;


    mqtt_data_doc["variables"][6]["last"]["value"] = config.soilHum;
    mqtt_data_doc["variables"][6]["last"]["save"] = 1;

    mqtt_data_doc["variables"][7]["last"]["value"] = config.batvoltage;
    mqtt_data_doc["variables"][7]["last"]["save"] = 1;

    send_data_to_broker();

}


//______________________________________________________________________________

//______________________________________________________________________________
bool get_mqtt_credentials()
{

  Serial.print(underlinePurple + "\n\n\nGetting MQTT Credentials from WebHook" + fontReset + Purple + "  ⤵");
  delay(1000);

  String toSend = "dId=" + dId + "&password=" + webhook_pass;


  http.begin(webhook_endpoint);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int response_code = http.POST(toSend);

  if (response_code < 0)
  {
    Serial.print(boldRed + "\n\n         Error Sending Post Request :( " + fontReset);
    http.end();
    return false;
  }

  if (response_code != 200)
  {
    Serial.print(boldRed + "\n\n         Error in response :(   e-> " + fontReset + " " + response_code);
    http.end();
    return false;
  }

  if (response_code == 200)
  {
    String responseBody = http.getString();

    Serial.print(boldGreen + "\n\n         Mqtt Credentials Obtained Successfully :) " + fontReset);

    deserializeJson(mqtt_data_doc, responseBody);
    http.end();
    delay(1000);
  }

  return true;
}

//______________________________________________________________________________
void connect_to_IoTCRv2()
{

  if (!get_mqtt_credentials())
  {
    Serial.println(boldRed + "\n\n      Error getting mqtt credentials :( \n\n RESTARTING IN 10 SECONDS");
    Serial.println(fontReset);
    delay(10000);
    goToDeepSleepFiveMinutes();
  }

  //Setting up Mqtt Server
  mqttClient.setServer(broker, 1883);

  Serial.print(underlinePurple + "\n\n\nTrying MQTT Connection" + fontReset + Purple + "  ⤵");

  String str_client_id = "device_" + dId + "_" + random(1, 9999);
  const char *username = mqtt_data_doc["username"];
  const char *password = mqtt_data_doc["password"];
  String str_topic = mqtt_data_doc["topic"];



  if (mqttClient.connect(str_client_id.c_str(), username, password))
  {
    Serial.print(boldGreen + "\n\n         Mqtt Client Connected :) " + fontReset);
    delay(2000);

    mqttClient.subscribe((str_topic + "+/actdata").c_str());
  }
  else
  {
    Serial.print(boldRed + "\n\n         Mqtt Client Connection Failed :( " + fontReset);
  }
}

//______________________________________________________________________________
void send_data_to_broker()
{
  //long now = millis();

  for (int i = 0; i < mqtt_data_doc["variables"].size(); i++)
  {

    if (mqtt_data_doc["variables"][i]["variableType"] == "output")
    {
      continue;
    }

    //int freq = mqtt_data_doc["variables"][i]["variableSendFreq"];
    //if (now - varsLastSend[i] > freq * 1000)
    //{
      varsLastSend[i] = millis();

      String str_root_topic = mqtt_data_doc["topic"];
      String str_variable = mqtt_data_doc["variables"][i]["variable"];
      String topic = str_root_topic + str_variable + "/sdata";

      String toSend = "";

      serializeJson(mqtt_data_doc["variables"][i]["last"], toSend);

      mqttClient.publish(topic.c_str(), toSend.c_str());
      Serial.print(boldGreen + "\n\n         Mqtt ENVIADO:) " + fontReset);


      //STATS
      long counter = mqtt_data_doc["variables"][i]["counter"];
      counter++;
      mqtt_data_doc["variables"][i]["counter"] = counter;

    //}
  }
}
