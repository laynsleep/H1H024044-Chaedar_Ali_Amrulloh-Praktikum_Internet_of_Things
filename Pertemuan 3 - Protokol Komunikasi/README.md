# Modul 3 - Protokol Komunikasi
## Penjelasan Kode
### Percobaan 3A - Komunikasi Data HTTP <hr>

Pada percobaan pertama, kita implementasikan protokol HTTP untuk mengirim data JSON ke endpoint.

```cpp
#include <ESP8266WiFi.h>        // koneksi wifi esp8266
#include <ESP8266HTTPClient.h>  // protokol HTTP
#include <WiFiClientSecure.h>   // untuk pemanggilan method setInsecure()
#include <ArduinoJson.h>        // pembacaan dan pembuatan data JSON

const char* ssid = "L";
const char* password = "12345678";
/*  endpoint uji HTTP POST.
    dimana data akan dikirim lewat request ke url tersebut dengan method POST
*/
const char* serverUrl = "https://httpbin.org/post"; 

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi berhasil terhubung!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    /*  deklarasi objek client dari class WiFiClientSecure.
        client akan memanggil method setInsecure untuk melewati tahap
        verifikasi sertifikat untuk keperluan uji coba/praktikum.
    */
    WiFiClientSecure client;
    client.setInsecure();

    /*  deklarasi objek http dari class HTTPClient.
        http akan memulai komunikasi dengan endpoint yang disimpan di serverUrl
        dengan tambahan header Content-Type pada request.
    */
    HTTPClient http;
    http.begin(client, serverUrl);
    http.addHeader("Content-Type", "application/json");

    /*  deklarasi objek doc dari class JsonDocument.
        doc akan menambahkan dua data yaitu suhu dan kelembapan
    */
    JsonDocument doc          // Membuat objek data sensor dalam format JSON
    doc["suhu"] = 28.5;       // contoh data suhu (°C)
    doc["kelembaban"] = 65.0; // contoh data kelembaban (%)

    String requestBody;
    serializeJson(doc, requestBody); // Konversi data ke JSON dan disimpan di requestBody

    Serial.print("Mengirim data: ");
    Serial.println(requestBody);

    // Mengirim data melalui HTTP POST
    int httpResponseCode = http.POST(requestBody);

    /*  percabangan untuk mengecek jika pengiriman data berhasil dilakukan
        atau tidak, jika berhasil, response dari endpoint akan diprint ke dalam
        serial disertai kode responnya.
    */
    if (httpResponseCode > 0) {
      Serial.print("Kode Response HTTP: ");
      Serial.println(httpResponseCode);
      Serial.println("Isi Response:");
      Serial.println(http.getString());
    } else {
      Serial.print("Pengiriman gagal, kode error: ");
      Serial.println(httpResponseCode);
    }

    http.end(); // matikan komunikasi
  }

  delay(10000); // kirim data setiap 10 detik
}
```
### Library
- ESP8266WiFi
- ESP8266HTTPClient
- WiFiClientSecure.h
- ArduinoJSON

### Percobaan 3B - Komunikasi Data MQTT<hr>

Pada percobaan kedua, kita implementasikan protokol MQTT untuk mengirim data JSON ke server broker melalui arsitektur publish-subscribe.

```cpp
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// =========================
// WiFi
// =========================
const char* ssid = "WIFI";
const char* password = "PW";

// =========================
// HiveMQ Cloud
// =========================
// Server broker mqtt
const char* mqttServer =
  "b10b628edde04465b095d96794a7d0b1.s1.eu.hivemq.cloud";

// Di port berapa client bisa terkonek ke broker
const int mqttPort = 8883;

// Username dan password dari HiveMQ Cloud
const char* mqttUsername = "USERNAME_HIVEMQ";
const char* mqttPassword = "PASSWORD_HIVEMQ";

// Topic MQTT
const char* mqttTopic =
  "unsoed/tk245004/kelompokAnda/sensor";

// =========================
// MQTT Client
// =========================
WiFiClientSecure espClient;
PubSubClient client(espClient);


// =========================
// Hubungkan WiFi
// =========================
void hubungkanWiFi() {

    WiFi.begin(ssid, password);

    Serial.print("Menghubungkan ke WiFi");

    while (WiFi.status() != WL_CONNECTED) {

        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi berhasil terhubung!");

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}


// =========================
// Hubungkan MQTT
// =========================
void hubungkanMQTT() {

    while (!client.connected()) {

        Serial.println();
        Serial.print("Menghubungkan ke broker MQTT...");

        // Client ID dibuat unik
        String clientId =
            "ESP8266Client-" +
            String(ESP.getChipId(), HEX);

        Serial.print(" Client ID: ");
        Serial.println(clientId);

        // Connect dengan username dan password HiveMQ
        if (client.connect(
                clientId.c_str(),
                mqttUsername,
                mqttPassword)) {

            Serial.println("MQTT berhasil terhubung!");

            Serial.print("Broker : ");
            Serial.println(mqttServer);

            Serial.print("Port   : ");
            Serial.println(mqttPort);

            Serial.println("================================");

        } else {

            Serial.print("MQTT gagal, rc=");
            Serial.println(client.state());

            Serial.println("Mencoba lagi dalam 2 detik...");

            delay(2000);
        }
    }
}


// =========================
// Setup
// =========================
void setup() {

  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println("================================");
  Serial.println(" ESP8266 MQTT - HiveMQ Cloud");
  Serial.println("================================");

  // -------------------------
  // WiFi
  // -------------------------
  hubungkanWiFi(); // memanggil fungsi untuk menghubungkan wifi

  // -------------------------
  // TLS
  // -------------------------
  // Untuk testing.
  // Tidak melakukan validasi sertifikat TLS.
  espClient.setInsecure();

  // -------------------------
  // MQTT Server
  // -------------------------
  client.setServer( // set port dan server broker untuk client terhubung
    mqttServer,
    mqttPort
  );

  // Hubungkan ke MQTT
  hubungkanMQTT();
}


// =========================
// Loop
// =========================
void loop() {

    /*  Memastikan koneksi tetap terjaga tiap loop
        dengan kondisi apabila status client terdikoneksi
        akan memanggil fungsi hubungkanMQTT kembali
    */
    if (!client.connected()) {

        Serial.println();
        Serial.println("MQTT terputus!");

        hubungkanMQTT();
    }

    // Wajib dipanggil terus-menerus
    // Menjaga koneksi antara client dan broker tetap terbuka
    client.loop();


    // =========================
    // Membuat data sensor JSON
    // =========================
    JsonDocument doc;

    //  Membuat data baru pada objek doc
    doc["suhu"] = 28.5;
    doc["kelembaban"] = 65.0;

    char buffer[128];

    //  konversi data di dalam doc menjadi format json dan disimpan di var buffer
    serializeJson(doc, buffer);

    // =========================
    // Publish MQTT
    // =========================
    bool berhasil =
    client.publish(
        mqttTopic,
        buffer
    );

    if (berhasil) {

        Serial.println();
        Serial.println("Data berhasil dikirim!");

        Serial.print("Topic   : ");
        Serial.println(mqttTopic);

        Serial.print("Payload : ");
        Serial.println(buffer);

    } else {

        Serial.println();
        Serial.println("Gagal mengirim data!");
    }

    // Publish setiap 5 detik
    delay(5000);
}
```
### Library
- ESP8266WiFi
- PubSubClient
- WiFiClientSecure.h
- ArduinoJSON

<br>

## Pertanyaan Praktikum
### Percobaan 3A <hr>

>Modifikasi program agar ESP32 dapat mengirimkan data tambahan berupa waktu (dalam milidetik sejak dinyalakan menggunakan millis()) ke dalam JSON yang dikirim, dan berikan penjelasan di setiap baris kode yang ditambahkan dalam bentuk README.md

```cpp
#include <WiFi.h>        // koneksi wifi esp8266
#include <HTTPClient.h>  // protokol HTTP
#include <WiFiClientSecure.h>   // untuk pemanggilan method setInsecure()
#include <ArduinoJson.h>        // pembacaan dan pembuatan data JSON

const char* ssid = "Wokwi-GUEST";
const char* password = "";
/*  endpoint uji HTTP POST.
    dimana data akan dikirim lewat request ke url tersebut dengan method POST
*/
const char* serverUrl = "https://httpbin.org/post"; 

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi berhasil terhubung!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    /*  deklarasi objek client dari class WiFiClientSecure.
        client akan memanggil method setInsecure untuk melewati tahap
        verifikasi sertifikat untuk keperluan uji coba/praktikum.
    */
    WiFiClientSecure client;
    client.setInsecure();

    /*  deklarasi objek http dari class HTTPClient.
        http akan memulai komunikasi dengan endpoint yang disimpan di serverUrl
        dengan tambahan header Content-Type pada request.
    */
    HTTPClient http;
    http.begin(client, serverUrl);
    http.addHeader("Content-Type", "application/json");

    /*  deklarasi objek doc dari class JsonDocument.
        doc akan menambahkan dua data yaitu suhu dan kelembapan
    */
    JsonDocument doc;         // Membuat objek data sensor dalam format JSON
    doc["suhu"] = 28.5;       // contoh data suhu (°C)
    doc["kelembaban"] = 65.0; // contoh data kelembaban (%)
    doc["timelapse"] = millis();

    String requestBody;
    serializeJson(doc, requestBody); // Konversi data ke JSON dan disimpan di requestBody

    Serial.print("Mengirim data: ");
    Serial.println(requestBody);

    // Mengirim data melalui HTTP POST
    int httpResponseCode = http.POST(requestBody);

    /*  percabangan untuk mengecek jika pengiriman data berhasil dilakukan
        atau tidak, jika berhasil, response dari endpoint akan diprint ke dalam
        serial disertai kode responnya.
    */
    if (httpResponseCode > 0) {
      Serial.print("Kode Response HTTP: ");
      Serial.println(httpResponseCode);
      Serial.println("Isi Response:");
      Serial.println(http.getString());
    } else {
      Serial.print("Pengiriman gagal, kode error: ");
      Serial.println(httpResponseCode);
    }

    http.end(); // matikan komunikasi
  }

  delay(10000); // kirim data setiap 10 detik
}
```

```cpp
JsonDocument doc;         // Membuat objek data sensor dalam format JSON
doc["suhu"] = 28.5;       // contoh data suhu (°C)
doc["kelembaban"] = 65.0; // contoh data kelembaban (%)
// tambah data timelapse sejak program dimulai dalam format millisecond
doc["timelapse"] = millis();
```

<br>

## Dokumentasi
### Percobaan 3A

<div align="center">
    <img src="dokumentasi/percobaan1_sta.jpg" width="400"/>
</div>

### Percobaan 3B

<div align="center">
    <img src="dokumentasi/percobaan2_ap.jpg" width="400"/>
    <img src="dokumentasi/percobaan2_terhubung.jpg" width="400"/>
</div>