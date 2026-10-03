#include <ESP8266WiFi.h>      // ganti dari <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

const char* ssid = "L";
const char* password = "12345678";

const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
/*  Varible untuk publish dan subscribe data ke/dari broker sesuai topik.
*   Topik perintah menerima data aksi aktuator.
*   Topik data mengirim data hasil sensor DHT.
*/
const char* topicData     = "unsoed/tk245004/kelompok4/data";
const char* topicPerintah = "unsoed/tk245004/kelompok4/perintah";

#define DHTPIN D5             // GPIO14 = D5 (GPIO4 dipakai LED)
#define DHTTYPE DHT11
const int ledPin = D2;         // GPIO4 = D2

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long waktuTerakhirPublish = 0;
const long intervalPublish = 5000; // publish data setiap 5 detik (non-blocking)

void callback(char* topic, byte* payload, unsigned int length) {
  String pesan;
  /*  Pengulangan untuk memasukkan tiap karakter
  *   data dari payload ke variable pesan.
  */
  for (unsigned int i = 0; i < length; i++) pesan += (char)payload[i];

  JsonDocument doc;
  if (deserializeJson(doc, pesan)) return; // abaikan jika parsing gagal

  const char* perintah = doc["perintah"];
  if (perintah == nullptr) return;         // hindari crash jika key tidak ada

  /*  Memanfaatkan data perintah hasil subscribe
  *   untuk menggerakan aktuator berdasarkan nilainya.
  *   ON == Nyalakan LED dan publish pesan "Aktuator: ON"
  *   Selain perintah diatas akan mematikan LED.
  */
  digitalWrite(ledPin, String(perintah) == "ON" ? HIGH : LOW);
  Serial.print("Perintah diterima -> Aktuator: ");
  Serial.println(perintah);
}

void hubungkanWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi berhasil terhubung!");
}

void hubungkanMQTT() {
  while (!client.connected()) {
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      client.subscribe(topicPerintah);
      Serial.println("Terhubung dan subscribe topic perintah");
    } else {
      Serial.print("Gagal MQTT, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  dht.begin();
  hubungkanWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback); // set callback sebagai fungsi callback ketika pesan baru masuk
}

void loop() {
  if (!client.connected()) hubungkanMQTT();
  client.loop(); // memproses pesan masuk secara terus-menerus

  /*  Publish data sensor secara berkala tanpa memblokir proses subscribe.
  *   fungsi millis() akan mengembalikan nilai milisekon
  *   setelah waktu ESP8266 berjalan yang nilainya bisa dibandingkan
  *   dengan nilai millis() sebelumnya yang disimpan pada waktuTerakhirPublish
  *   untuk mendapatkan waktu yang sudah terlampau, kemudian dibandingkan
  *   dengan intervalPublish yang bernilai 5000 milisekon (5 detik).
  */
  if (millis() - waktuTerakhirPublish > intervalPublish) {
    waktuTerakhirPublish = millis();

    float suhu = dht.readTemperature();
    if (!isnan(suhu)) {
      JsonDocument doc;
      doc["suhu"] = suhu;
      char buffer[128];
      serializeJson(doc, buffer);
      client.publish(topicData, buffer);
      Serial.print("Data terkirim: ");
      Serial.println(buffer);
    } else {
      Serial.println("Gagal membaca sensor DHT22");
    }
  }
}