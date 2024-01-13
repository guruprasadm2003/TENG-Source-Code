#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MAX30100_PulseOximeter.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pulse oximeter settings
PulseOximeter pox;
uint32_t lastBeatTime;
float bpm, spo2;

// WiFi settings
const char* ssid = "Your_WiFi_SSID";
const char* password = "Your_WiFi_Password";

// ThingSpeak settings
unsigned long channelID = Your_Channel_ID;
const char* writeAPIKey = "Your_Write_API_Key";

// Buzzer settings
const int buzzerPin = 7;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Health Monitoring");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();

  // Initialize pulse oximeter
  pox.begin();
  pox.setOnBeatDetectedCallback(onBeatDetected);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
  // Update the pulse oximeter
  pox.update();

  // Display heart rate and oxygen saturation on the LCD
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Heart Rate: ");
  display.print(bpm);
  display.println(" BPM");
  display.setCursor(0, 10);
  display.print("Oxygen Level: ");
  display.print(spo2);
  display.println("%");
  display.display();

  // Upload data to ThingSpeak
  ThingSpeak.writeField(channelID, 1, bpm, writeAPIKey);
  ThingSpeak.writeField(channelID, 2, spo2, writeAPIKey);

  // Check if heart rate is outside the normal range (e.g., below 60 or above 100)
  if (bpm < 60 || bpm > 100) {
    // Activate the buzzer
    digitalWrite(buzzerPin, HIGH);
    delay(1000);
    digitalWrite(buzzerPin, LOW);
    delay(1000);
  }

  // Delay for 5 seconds before the next loop
  delay(5000);
}

// Callback function for beat detection
void onBeatDetected() {
  display.setCursor(0, 20);
  display.println("Heart Beat Detected");
  display.display();
  lastBeatTime = millis();
}
