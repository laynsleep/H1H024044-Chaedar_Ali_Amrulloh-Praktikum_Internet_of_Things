#include <DHT11.h> // Import  library DHT11

DHT11 dht11(D5);   // GPIO14, aman dipakai, gampang ditemukan di board

void setup() {
    Serial.begin(9600); // Jalankan serial monitor dengan baud rate 9600
}

void loop() {
    // Inisialisasi variable
    int temperature = 0;
    int humidity = 0;

    // Membaca suhu dan kelembapan dari sensor
    int result = dht11.readTemperatureHumidity(temperature, humidity);

    // Error handling apabila result mengembalikan nilai error (selain 0),
    // Menampilkan temperatur dan kelembapan pada serial monitor jika result
    // mengembalikan nilai 0
    if (result == 0) {
        // Print output dalam format "Temperature: n °C  Humidity: n %"
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.print(" Â°C\tHumidity: ");
        Serial.print(humidity);
        Serial.println(" %");
    } else {
        Serial.println(DHT11::getErrorString(result)); // Print err message
    }

    delay(1000); // Delay satu detik
}