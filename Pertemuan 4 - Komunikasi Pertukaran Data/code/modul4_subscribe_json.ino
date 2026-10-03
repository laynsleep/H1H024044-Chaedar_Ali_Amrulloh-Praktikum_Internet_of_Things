#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "L";
const char* password = "12345678";

const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
/*  Varible untuk publish dan subscribe data ke/dari broker sesuai topik.
*   Topik perintah menerima data aksi aktuator.
*   Topik status mengirim status validasi data json.
*/
const char* topicPerintah = "unsoed/tk245004/kelompok4/perintah";
const char* topicStatus   = "unsoed/tk245004/kelompok4/status";

// GPIO4 = D2 pada NodeMCU
const int ledPin = D2;

WiFiClient espClient;
PubSubClient client(espClient);

// Callback dipanggil otomatis setiap ada pesan masuk
void callback(char* topic, byte* payload, unsigned int length) {
  String pesan;
  /*  Pengulangan untuk memasukkan tiap karakter
  *   data dari payload ke variable pesan.
  */
  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }

  // Print pesan
  Serial.print("Pesan diterima [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(pesan);

  // Deserialisasi JSON
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, pesan);

  /*  Pengkondisian jika gagal deserialize maka
  *   publish status ke topicStatus dan hentikan
  *   fungsi callback.
  */
  if (error) {
    Serial.print("Gagal parsing JSON: ");
    Serial.println(error.c_str());
    client.publish(topicStatus, "{\"error\":\"JSON tidak valid\"}");
    return;
  }

  /*  Memanfaatkan data perintah hasil subscribe
  *   untuk menggerakan aktuator berdasarkan nilainya.
  *   ON == Nyalakan LED dan publish pesan "Aktuator: ON"
  *   OFF == Matikan LED dan publish pesan "Aktuator: OFF"
  *   Selain perintah diatas akan diabakikan.
  *   Jika perintah kosong maka print warning.
  */
  const char* perintah = doc["perintah"];
  if (perintah == nullptr) {
    Serial.println("Key 'perintah' tidak ditemukan");
    client.publish(topicStatus, "{\"error\":\"key perintah tidak ada\"}");
    return;
  }

  if (String(perintah) == "ON") {
    digitalWrite(ledPin, HIGH);
    Serial.println("Aktuator: ON");
    client.publish(topicStatus, "{\"aktuator\":\"ON\"}");
  } else if (String(perintah) == "OFF") {
    digitalWrite(ledPin, LOW);
    Serial.println("Aktuator: OFF");
    client.publish(topicStatus, "{\"aktuator\":\"OFF\"}");
  } else {
    Serial.println("Perintah tidak dikenal");
    client.publish(topicStatus, "{\"error\":\"perintah tidak dikenal\"}");
  }
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
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void hubungkanMQTT() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke broker MQTT...");
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("berhasil terhubung!");
      client.subscribe(topicPerintah);  // subscribe client ke topik perintah
      client.publish(topicStatus, "{\"status\":\"online\"}");
      Serial.print("Subscribe ke topic: ");
      Serial.println(topicPerintah);
    } else {
      Serial.print("gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 2 detik");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  hubungkanWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback); // set callback sebagai fungsi callback ketika pesan baru masuk
}

void loop() {
  if (!client.connected()) {
    hubungkanMQTT();
  }
  client.loop();  // wajib dipanggil terus-menerus
}