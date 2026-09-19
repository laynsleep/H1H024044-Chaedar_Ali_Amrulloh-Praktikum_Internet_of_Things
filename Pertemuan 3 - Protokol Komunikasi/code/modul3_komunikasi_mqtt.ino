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