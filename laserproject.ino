// SMART IRRIGATION SYSTEM (SIS) - FINAL VERSION

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Create LCD object at I2C address 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin Definitions
int MoistureSensor = A2;   // Soil Moisture Sensor connected to A2
int RelayPin = 8;          // Relay control pin

void setup()
{
    lcd.init();             // Initialize LCD
    lcd.backlight();        // Turn on backlight

    pinMode(RelayPin, OUTPUT);
    Serial.begin(9600);

    digitalWrite(RelayPin, HIGH); // Initially turn OFF relay (Active LOW Relay)

    // Welcome Messages
    lcd.setCursor(0, 0);
    lcd.print("Designed by:");
    lcd.setCursor(0, 1);
    lcd.print("Kartikaye");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Auto Irrigation");
    lcd.setCursor(0, 1);
    lcd.print("System");
    delay(2500);
    lcd.clear();
    delay(1000);
}

void loop()
{
    lcd.clear();

    int moistureValue = analogRead(MoistureSensor); // Read Moisture Value

    Serial.print("Moisture Value: ");
    Serial.println(moistureValue);

    // --- Display Plant Status ---
    lcd.setCursor(0, 0);
    if (moistureValue > 200) {   // Very dry
        lcd.print("Plant: DRY   ");
    } 
    else if (moistureValue >= 100 && moistureValue <= 200) { // Moist
        lcd.print("Plant: MOIST ");
    } 
    else { // Very wet
        lcd.print("Plant: SOGGY ");
    }

    // --- Watering Logic & Display ---
    lcd.setCursor(0, 1);
    lcd.print("R:");
    lcd.print(moistureValue);

    if (moistureValue > 200) // Soil is dry
    {
        digitalWrite(RelayPin, LOW); // Turn ON valve
        lcd.print(" V:ON ");
    }
    else // Soil is moist or soggy
    {
        digitalWrite(RelayPin, HIGH); // Turn OFF valve
        lcd.print(" V:OFF");
    }

    delay(1000); // Update every second
}