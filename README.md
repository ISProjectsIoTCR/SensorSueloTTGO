MATERIALES DISPONIBLES EN CR CIBERNETICA:

-ESP32 Soil Humidity and Temperature Sensor Module
-Soil Hygrometer Sensor Module (SOLDADO EN LOS PINES 3V,GND,2,15 DEL TTGO-HIGROW)
-DASHBOARD IoTProjects


Base de código para ESP ttgo higrow compatible con la plataforma IoTProjects.

¿Para que es esto?

IoTProjects es una plataforma en la Nube para microcontroladores Arduino, ESP y cualquier dispositivo que soporte el protocolo MQTT. Su principal cualidad es que no depende de ningún servicio de terceros como Adafruit, Ubidots, Cayenne u otro.

Aquí un DEMO: https://app.iotcostarica.ml/demologin

¿Como funciona?

PASO_1: Ingrese a su cuenta en IoTProjects y comience a crear el TEMPLATE de su proyecto

PASO_2: Agrege un dispositivo y seleccione el template creado. Automaticamente la plataforma le generará un PASSWORD de conexión.

PASO_3: Descargue este proyecto y en "USER-VARIABLES" complete la información correspondiente a WIFI, IoTPROJECTS

PASO_4: Gestione las lecturas de sus sensores con un constructor Json y envíelo como parametro a la función sendToDashboard(config);

NOTA: ESTE PROYECTO ESTA BASADO EN EL SUPER REPOSITORIO /pesor/TTGO-T-HIGrow . YA CONTIENE UNA CONEXIÓN NTP, RUTINAS DE AHORRO DE ENERGÍA Y LECTURAS DE BATERÍA, ADICIONALMENTE SE AGREGÓ UN MODULO PARA MEDIR LA HUMEDAD DEL SUELO.

VIDEO DE AYUDA: https://youtu.be/_vzZYyu7I1c
