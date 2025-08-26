#include <WiFi.h>
#include "AdafruitIO_WiFi.h"

// Configuración WiFi y Adafruit IO
#define WIFI_SSID "HONOR 70 A"
#define WIFI_PASS "alejandrito"
#define IO_USERNAME "Mar23599"
#define IO_KEY "aio_hNAS73grxKrQihqbjBU8CyZWXmGj"

// Configuración de puertos seriales
#define SERIAL_SENSORES Serial2
#define PIN_RX 16
#define PIN_TX 17

//Constantes de tiempo
#define INTERVALO_ENVIO 10000

// Feeds de Adafruit IO
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
AdafruitIO_Feed *feed_temp = io.feed("s_temp");
AdafruitIO_Feed *feed_hum = io.feed("s_hum");
AdafruitIO_Feed *feed_light = io.feed("s_light");
AdafruitIO_Feed *feed_dc = io.feed("dc_motor");
AdafruitIO_Feed *feed_servo = io.feed("servo_motor");

// Variables para control de tiempo
unsigned long tiempoAnterior = 0;
bool datosDisponibles = false;
float temperatura = 0.0;
float humedad = 0.0;
float luz = 0.0;

// Prototipos de funciones
void WiFi_ADAIO_init(void);
void procesarDatosSensores();
void enviarADAFeed(float temperatura, float humedad, float luz);

//Funciones de recepcion de datos desde AdafruitIO

void handleDCMessage(AdafruitIO_Data *data);
void handleServoMessage(AdafruitIO_Data *data);

void setup() {
  Serial.begin(115200);
  SERIAL_SENSORES.begin(9600, SERIAL_8N1, PIN_RX, PIN_TX);
  WiFi_ADAIO_init();
  Serial.println("Sistema listo para recibir datos de sensores...");
}

void loop() {
  io.run();
  procesarDatosSensores();
  
  unsigned long tiempoActual = millis();
  
  if (tiempoActual - tiempoAnterior >= INTERVALO_ENVIO) {
    tiempoAnterior = tiempoActual;
    
    if (datosDisponibles) {
      Serial.println("Intervalo de 10 segundos alcanzado");
      enviarADAFeed(temperatura, humedad, luz);
      datosDisponibles = false;
    } else {
      Serial.println("Intervalo alcanzado, pero no hay datos nuevos");
    }
  }
  
  delay(10);
}

void WiFi_ADAIO_init(void) {
  Serial.print("Conectando a WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado a WiFi!");
  
  Serial.print("Conectando a Adafruit IO");
  io.connect();
  
  while(io.status() < AIO_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado a Adafruit IO!");
  feed_dc->onMessage(handleDCMessage);
  feed_servo->onMessage(handleServoMessage);
}

void procesarDatosSensores() {
  static String buffer = "";
  
  while (SERIAL_SENSORES.available()) {
    char c = SERIAL_SENSORES.read();
    buffer += c;
    
    if (c == '\n' || c == '\r') {
      buffer.trim();
      
      if (buffer.length() > 0) {
        Serial.print("Datos crudos recibidos: ");
        Serial.println(buffer);
        
        float tempTemp = 0.0;
        float tempHum = 0.0;
        float tempLuz = 0.0;
        
        int inicioH = buffer.indexOf('H');
        int finH = buffer.indexOf('H', inicioH + 1);
        if (inicioH != -1 && finH != -1 && finH > inicioH + 1) {
          String humedadStr = buffer.substring(inicioH + 1, finH);
          tempHum = humedadStr.toFloat();
          Serial.print("Humedad extraída: ");
          Serial.println(tempHum);
        }
        
        int inicioT = buffer.indexOf('T');
        int finT = buffer.indexOf('T', inicioT + 1);
        if (inicioT != -1 && finT != -1 && finT > inicioT + 1) {
          String tempStr = buffer.substring(inicioT + 1, finT);
          tempTemp = tempStr.toFloat();
          Serial.print("Temperatura extraída: ");
          Serial.println(tempTemp);
        }
        
        int inicioL = buffer.indexOf('L');
        int finL = buffer.indexOf('L', inicioL + 1);
        if (inicioL != -1 && finL != -1 && finL > inicioL + 1) {
          String luzStr = buffer.substring(inicioL + 1, finL);
          tempLuz = luzStr.toFloat();
          Serial.print("Luz extraída: ");
          Serial.println(tempLuz);
        }
        
        if (tempHum > 0 || tempTemp > 0 || tempLuz > 0) {
          temperatura = tempTemp;
          humedad = tempHum;
          luz = tempLuz;
          datosDisponibles = true;
          Serial.println("Datos actualizados para próximo envío");
        } else {
          Serial.println("No se pudieron extraer valores válidos");
        }
        
        buffer = "";
      }
    }
    
    if (buffer.length() > 100) {
      Serial.println("Buffer demasiado grande, limpiando...");
      buffer = "";
    }
  }
}

void enviarADAFeed(float temperatura, float humedad, float luz) {
  Serial.println("Enviando datos a Adafruit IO...");
  
  if (temperatura > -50 && temperatura < 100) {
    feed_temp->save(temperatura);
    Serial.print("Temperatura enviada: ");
    Serial.println(temperatura);
  }
  
  if (humedad >= 0 && humedad <= 100) {
    feed_hum->save(humedad);
    Serial.print("Humedad enviada: ");
    Serial.println(humedad);
  }
  
  if (luz >= 0) {
    feed_light->save(luz);
    Serial.print("Luz enviada: ");
    Serial.println(luz);
  }
  
  Serial.println("Datos enviados correctamente");
}



void handleDCMessage(AdafruitIO_Data *data) {
  int value = data->toInt();
  Serial.print("Received DC motor value: ");
  Serial.println(value);
  
  if(value == 1) {
    SERIAL_SENSORES.print('A');
    Serial.println("Enviado 'A' por Serial2");
  } else if(value == 0) {
    SERIAL_SENSORES.print('B');
    Serial.println("Enviado 'B' por Serial2");
  }
}

void handleServoMessage(AdafruitIO_Data *data) {
  int value = data->toInt();
  Serial.print("Received Servo value: ");
  Serial.println(value);
  
  if(value == 1) {
    SERIAL_SENSORES.print('C');
    Serial.println("Enviado 'C' por Serial2");
  } else if(value == 0) {
    SERIAL_SENSORES.print('D');
    Serial.println("Enviado 'D' por Serial2");
  }
}