#include <Wire.h>
#include <WiFi.h>
#include "AdafruitIO_WiFi.h"

// Configuración WiFi y Adafruit IO
#define WIFI_SSID "A34"
#define WIFI_PASS "ceu85xeur39ig7u"
#define IO_USERNAME "Luissactic"
#define IO_KEY "aio_pvyZ593E8XJP59yLyjNMLVLg2w4T"

// Configuración I2C
#define I2C_ESP_ADDR 0x50
#define MOTOR_SLAVE_ADDR 0x21  // Dirección del esclavo de motores

// Estructuras de datos
typedef struct {
    float temperature;
    float humidity;
    float light;
} SensorData;

// Feeds de Adafruit IO
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
AdafruitIO_Feed *feed_temp = io.feed("s_temp");
AdafruitIO_Feed *feed_hum = io.feed("s_hum");
AdafruitIO_Feed *feed_light = io.feed("s_light");


// Variables globales
SensorData sensorData = {0};

void setup() {
    Serial.begin(115200);
    while(!Serial);
    
    // Inicializar I2C
    Wire.begin(I2C_ESP_ADDR);
    Wire.setClock(100000);
    Wire.onReceive(receiveEvent);
    
    // Conectar a WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("Conectando a WiFi");
    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConectado!");
    
    // Conectar a Adafruit IO
    io.connect();
    
    
    Serial.print("Conectando a Adafruit IO");
    while(io.status() < AIO_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConectado a Adafruit IO!");
}

void loop() {
    io.run(); // Mantener conexión
    
    // Enviar datos de sensores periódicamente
    static unsigned long lastUpdate = 0;
    if(millis() - lastUpdate > 10000) {
        sendSensorData();
        lastUpdate = millis();
    }
}

void sendSensorData() {
    if(!isnan(sensorData.temperature)) {
        feed_temp->save(sensorData.temperature);
    }
    if(!isnan(sensorData.humidity)) {
        feed_hum->save(sensorData.humidity);
    }
    if(!isnan(sensorData.light)) {
        feed_light->save(sensorData.light);
    }
}

void receiveEvent(int howMany) {
    if(howMany == sizeof(SensorData)) {
        uint8_t* bytes = (uint8_t*)&sensorData;
        for(int i = 0; i < howMany; i++) {
            bytes[i] = Wire.read();
        }
    } else {
        while(Wire.available()) Wire.read();
    }
}
