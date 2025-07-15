// AUTOMATIC IRRIGATION SYSTEM WITH WIFI

#include <LiquidCrystal.h>  // LCD Library
#include <WiFiNINA.h>       // WiFi Library for Arduino Uno WiFi
#include <Wire.h>

// WiFi Credentials
const char* ssid = "YOUR_WIFI_SSID";    // Replace with your WiFi SSID
const char* password = "YOUR_WIFI_PASSWORD"; // Replace with your WiFi password

WiFiServer server(80); // Create a web server on port 80

// Define Notes for Speaker
#define NOTE_C4 262 
#define NOTE_D4 294 
#define NOTE_E4 330 
#define NOTE_F4 349 
#define NOTE_G4 392 
#define NOTE_A4 440 
#define NOTE_B4 494 
#define NOTE_C5 523 

// Define Pins
int M_Sensor1 = A0; // Soil Moisture Sensor 1
int M_Sensor2 = A1; // Soil Moisture Sensor 2
int W_led = 7;      // Water Level Status LED
int Pump = 13;      // Pump Relay
int Speaker = 9;    // Speaker/Buzzer

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
    lcd.begin(16, 2);
    lcd.clear();

    pinMode(Pump, OUTPUT);
    pinMode(W_led, INPUT);
    pinMode(Speaker, OUTPUT);
    
    Serial.begin(9600);
    
    // WiFi Connection
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connecting...");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }

    Serial.println("\nConnected to WiFi!");
    lcd.clear();
    lcd.print("WiFi Connected");
    delay(2000);

    server.begin(); // Start web server
    Serial.print("Web Server Started: ");
    Serial.println(WiFi.localIP()); // Print IP address

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SMART IRRIGATION");
    lcd.setCursor(0, 1);
    lcd.print("SYSTEM ONLINE");
    delay(2500);
    lcd.clear();
}

void loop() {
    int Moisture1 = analogRead(M_Sensor1); // Read Soil Moisture Sensor 1
    int Moisture2 = analogRead(M_Sensor2); // Read Soil Moisture Sensor 2
    Serial.print("Moisture1: "); Serial.println(Moisture1);
    Serial.print("Moisture2: "); Serial.println(Moisture2);
    
    lcd.clear();
    
    // Soil Moisture Condition for Plant 1
    lcd.setCursor(0, 0);
    if (Moisture1 > 200) { 
        lcd.print("Plant 1: DRY");
        if (digitalRead(W_led) == 1) {
            digitalWrite(Pump, HIGH);
            lcd.setCursor(0, 1);
            lcd.print("PUMP: ON");
        } else {
            digitalWrite(Pump, LOW);
            lcd.setCursor(0, 1);
            lcd.print("PUMP: OFF");
            tone(Speaker, NOTE_C4, 500);
            delay(500);
            tone(Speaker, NOTE_D4, 500);
        }
    } else if (Moisture1 >= 70 && Moisture1 <= 200) {
        lcd.print("Plant 1: MOIST");
        digitalWrite(Pump, LOW);
    } else {
        lcd.print("Plant 1: SOGGY");
        digitalWrite(Pump, LOW);
    }
    
    // Soil Moisture Condition for Plant 2
    lcd.setCursor(0, 1);
    if (Moisture2 > 200) {
        lcd.print("Plant 2: DRY");
        digitalWrite(Pump, HIGH);
    } else if (Moisture2 >= 70 && Moisture2 <= 200) {
        lcd.print("Plant 2: MOIST");
        digitalWrite(Pump, LOW);
    } else {
        lcd.print("Plant 2: SOGGY");
        digitalWrite(Pump, LOW);
    }

    delay(1000);

    // Handle WiFi Requests
    WiFiClient client = server.available();
    if (client) {
        Serial.println("New Client Connected.");
        String request = client.readStringUntil('\r');
        client.flush();
        
        if (request.indexOf("/pump/on") != -1) {
            digitalWrite(Pump, HIGH);
        } else if (request.indexOf("/pump/off") != -1) {
            digitalWrite(Pump, LOW);
        }

        // Send HTML response
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println();
        client.println("<html><head><title>Smart Irrigation</title></head>");
        client.println("<body><h1>Smart Irrigation System</h1>");
        client.println("<p>Soil Moisture 1: " + String(Moisture1) + "</p>");
        client.println("<p>Soil Moisture 2: " + String(Moisture2) + "</p>");
        client.println("<p>Pump Status: " + String(digitalRead(Pump) ? "ON" : "OFF") + "</p>");
        client.println("<a href='/pump/on'>Turn Pump ON</a><br>");
        client.println("<a href='/pump/off'>Turn Pump OFF</a><br>");
        client.println("</body></html>");
        client.stop();
    }
}