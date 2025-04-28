/*********************************************************************************
* MIT License
* 
* Copyright (c) 2023 Guillaume Larue
* 
* This firmware uses the HomeSpan library (https://github.com/HomeSpan/HomeSpan)
* by Gregg E. Berman, licensed under the MIT License.
*********************************************************************************/

/**
 * XIAO ESP32C3 LED Controller
 * 
 * A HomeKit compatible LED controller supporting both single-channel
 * and RGB configurations, built on the XIAO ESP32C3 platform.
 * 
 * Hardware:
 * - XIAO ESP32C3
 * - Up to 5 channels (PWM outputs)
 * - Common anode or common cathode depending on PCB assembly
 * - 12/24V DC input
 * - Max 6A per channel, 20A total
 * 
 * Features:
 * - HomeKit compatible
 * - Supports single color dimming, RGB control, or any up-to-5 channels topology
 * - Auto AP mode for configuration
 * - Non-volatile state storage
 * 
 */

#include "HomeSpan.h" 
#include "DEV_Identify.h" 
#include "DEV_LED.h" 

// Device Configuration
#define DEVICE_NAME     "XIAO ESP32 C3 LED Controller"
#define MANUFACTURER    "GL"
#define SERIAL_NUMBER   "123-ABC"
#define MODEL          "LED-5CH"
#define VERSION        "0.9"
#define HAP_VERSION    "1.1.0"
#define IDENTIFY_BLINKS 3

// Output Pin Definitions - Map color accordingly
// if additional channels must be used new code must be developped in DEV_LED.h
#define PIN_CH1 D1    // Single LED / Red channel
#define PIN_CH2 D2    // Green channel
#define PIN_CH3 D3    // Blue channel
//#define PIN_CH4 D4  // Additional channel (if needed)
//#define PIN_CH5 D5  // Additional channel (if needed)

/**
 * Initial setup of the device
 */
void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    Serial.println("----------------------------------------");
    Serial.println("Initializing XIAO ESP32C3 LED Controller");
    Serial.println("----------------------------------------");

    // Configure HomeSpan
    homeSpan.enableAutoStartAP();           // Enable auto AP mode for setup

    // Initialize HomeSpan
    homeSpan.begin(Category::Lighting, DEVICE_NAME);

    // Log device configuration
    Serial.println("Device Configuration:");
    Serial.printf("  Name: %s\n", DEVICE_NAME);
    Serial.printf("  Manufacturer: %s\n", MANUFACTURER);
    Serial.printf("  Serial: %s\n", SERIAL_NUMBER);
    Serial.printf("  Model: %s\n", MODEL);
    Serial.printf("  Version: %s\n", VERSION);
    Serial.printf("  HAP Version: %s\n", HAP_VERSION);

    // Create primary accessory
    new SpanAccessory();
    
    // Add mandatory characteristics
    new DEV_Identify(DEVICE_NAME, MANUFACTURER, SERIAL_NUMBER, 
                    MODEL, VERSION, IDENTIFY_BLINKS);
    new Service::HAPProtocolInformation();
    new Characteristic::Version(HAP_VERSION);

    // Add LED service
    #ifdef USE_RGB_LED
        Serial.println("Configuring RGB LED mode");
        new DEV_RgbLED(PIN_CH1, PIN_CH2, PIN_CH3);
    #else
        Serial.println("Configuring Single Channel LED mode");
        new DEV_DimmableLED(PIN_CH1);
    #endif

    Serial.println("----------------------------------------");
    Serial.println("Initialization complete!");
    Serial.println("----------------------------------------");
}

/**
 * Main loop
 */
void loop() {
    homeSpan.poll();
}
