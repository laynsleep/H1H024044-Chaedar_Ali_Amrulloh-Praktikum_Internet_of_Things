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