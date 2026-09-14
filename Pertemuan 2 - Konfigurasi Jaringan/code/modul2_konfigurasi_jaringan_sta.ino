#include <ESP8266WiFi.h>

// Kredensial hotspot yang akan dihubungkan
const char* ssid     = "mad";
const char* password = "dang4444";

const int ledPin = D2;   // LED indikator status koneksi

void setup() {
    Serial.begin(115200);

    // Set mode LED dan matikan saat program dijalankan
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);

    // Set mode WiFi menjadi Station
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    Serial.print("Menghubungkan ke WiFi");
    // Pengulangan untuk mengecek koneksi WiFi hingga terhubung
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    // Jika berhasil terhubung,
    // Print IP, MAC, dan kuat sinyal WiFi
    Serial.println();
    Serial.println("WiFi berhasil terhubung!");
    Serial.print("IP Address  : ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC Address : ");
    Serial.println(WiFi.macAddress());
    Serial.print("RSSI (dBm)  : ");
    Serial.println(WiFi.RSSI());

    digitalWrite(ledPin, HIGH);  // nyalakan LED sebagai indikator
}

void loop() {
    // Cek status koneksi setiap 5 detik
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Status: Terhubung"); // Print status ketika terhubung
    } else {
        Serial.println("Status: Terputus"); // Print status ketika terputus
        digitalWrite(ledPin, LOW); // Matikan LED
    }

    // Print IP, MAC, dan kuat sinyal WiFi
    Serial.print("IP Address  : ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC Address : ");
    Serial.println(WiFi.macAddress());
    Serial.print("RSSI (dBm)  : ");
    Serial.println(WiFi.RSSI());
    delay(5000);
}