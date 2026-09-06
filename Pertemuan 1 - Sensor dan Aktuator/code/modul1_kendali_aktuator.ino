#include <DHT.h> // Import DHT

// Definisikan nilai environment
#define DHTPIN D5 // GPIO14 - untuk sensor DHT22
#define DHTTYPE DHT11
#define RELAYPIN D6 // GPIO12 - untuk kendali relay/LED

DHT dht(DHTPIN, DHTTYPE); // Deklarasi dht
const float suhuThreshold = 30.0; // Inisialisasi threshold

void setup() {
    Serial.begin(115200);
    dht.begin(); // inisialisasi
    pinMode(RELAYPIN, OUTPUT); // Set RELAYPIN ke mode OUTPUT
    digitalWrite(RELAYPIN, LOW); // Nilai awal RELAYPIN
}

void loop() {
    float suhu = dht.readTemperature(); // membaca temperatur dari sensor
    if (isnan(suhu)) { // Cek apakah suhu tidak valid atau bukan angka
        Serial.println("Gagal membaca data sensor!");
    } else { // Jika valid print suhu ke serial
        Serial.print("Suhu: ");
        Serial.print(suhu);
        Serial.print(" Â°C -> ");
        if (suhu > suhuThreshold) { // Jika suhu melebihi threshold
            digitalWrite(RELAYPIN, HIGH); // Nyalakan LED
            Serial.println("Aktuator: ON"); // print Aktuator: ON
        } else { // Jika tidak
            digitalWrite(RELAYPIN, LOW); // Matikan LED
            Serial.println("Aktuator: OFF");// print Aktuator: OFF
        }
    }
    delay(2000); // DHT22 butuh jeda antar baca, minimal 2 detik
}