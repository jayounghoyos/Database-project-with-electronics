#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <TinyGPS.h>
#include <HardwareSerial.h>

// Reemplaza con tus credenciales de red
const char* ssid = "CLARO-B637";
const char* password = "Cl4r0@D1B637";

Adafruit_MPU6050 mpu;
TinyGPS gps;

HardwareSerial mySerial(1); // Usar UART1 para el GPS

AsyncWebServer server(80);

void setup() {
  // Iniciar la comunicación Serial para enviar mensajes al ordenador
  Serial.begin(115200);
  Wire.begin();
  mySerial.begin(9600, SERIAL_8N1, 16, 17); // RX, TX

  // Inicializar MPU6050
  Serial.println("Inicializando MPU6050...");
  if (!mpu.begin()) {
    Serial.println("No se pudo encontrar el chip MPU6050");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Inicializado.");

  // Conectar a la red Wi-Fi
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // Esperar hasta que se establezca la conexión
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Ruta para la página web principal
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    sensors_event_t accel, gyro, temp;
    mpu.getEvent(&accel, &gyro, &temp);

    float temperature = temp.temperature;

    String gpsData = "";
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    if (age == TinyGPS::GPS_INVALID_AGE) {
      gpsData = "No se encontraron datos de GPS";
    } else {
      gpsData = "Latitud: " + String(flat, 6) + ", Longitud: " + String(flon, 6);
    }

    String html = "<!DOCTYPE html><html><head><title>Monitor de Temperatura y GPS</title></head><body><h1>Temperatura Actual: " + String(temperature) + " °C</h1><h2>Datos GPS: " + gpsData + "</h2></body></html>";
    request->send(200, "text/html", html);
  });

  // Iniciar servidor
  server.begin();
}

void loop() {
  while (mySerial.available() > 0) {
    gps.encode(mySerial.read());
  }
}
