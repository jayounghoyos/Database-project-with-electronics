#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

const char* ssid = "CLARO-B637";
const char* password = "Cl4r0@D1B637";

Adafruit_MPU6050 mpu;
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10); // Espera a que el puerto serie se inicialice

  // Conexión WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Inicializa el MPU6050
  if (!mpu.begin()) {
    Serial.println("No se pudo encontrar un MPU6050. Verifique las conexiones.");
    while (1) {
      delay(10);
    }
  }

  // Configura el MPU6050
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  Serial.println("MPU6050 inicializado correctamente.");

  // Configura el servidor
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    String data = "{";
    data += "\"acceleration\": {";
    data += "\"x\": " + String(a.acceleration.x) + ",";
    data += "\"y\": " + String(a.acceleration.y) + ",";
    data += "\"z\": " + String(a.acceleration.z) + "},";
    data += "\"gyro\": {";
    data += "\"x\": " + String(g.gyro.x) + ",";
    data += "\"y\": " + String(g.gyro.y) + ",";
    data += "\"z\": " + String(g.gyro.z) + "},";
    data += "\"temperature\": " + String(temp.temperature);
    data += "}";

    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", data);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.begin();
}

void loop() {
  // No es necesario poner código en el loop para el servidor asíncrono
}
