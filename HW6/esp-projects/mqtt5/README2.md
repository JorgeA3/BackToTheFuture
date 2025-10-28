Configuración Tarea 6 - Jorge Isaac Aguirre Morgado (0246911)

Prefijo Único
u01

Parte B: Configuración del Broker Local
Broker IP: 192.168.1.78

Topics Locales:
u01/sensors/temperature
u01/sensors/gps

Parte C: Configuración de Adafruit IO
Adafruit IO Username: JorgeA3
    Adafruit IO Feeds:
    u01.temperature (Key: u01.temperature)
    u01.gps (Key: u01.gps)

Topics de Publicación en Adafruit IO:
JorgeA3/feeds/u01.temperature
JorgeA3/feeds/u01.gps



Nota sobre el Payload del Mapa
El widget de Mapa de Adafruit IO requirió un formato de payload JSON (ej. {"value": 27, "lat": 20.67, "lon": -103.35}) en lugar del formato de texto simple ("lat,lon") especificado en el PDF de la tarea. El código del ESP32 fue adaptado para enviar este formato JSON.