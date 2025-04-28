/*********************************************************************************
* MIT License
* 
* Copyright (c) 2023 Guillaume Larue
* 
* This firmware uses the HomeSpan library (https://github.com/HomeSpan/HomeSpan)
* by Gregg E. Berman, licensed under the MIT License.
*********************************************************************************/

#ifndef DEV_IDENTIFY_H
#define DEV_IDENTIFY_H

#include "HomeSpan.h"

//////////////////////////////////
//     ACCESSORY IDENTIFY       //
//////////////////////////////////

struct DEV_Identify : Service::AccessoryInformation {
    // Class members
    int nBlinks;                    // Number of times to blink built-in LED in identify routine
    SpanCharacteristic *identify;   // Reference to the Identify Characteristic
    
    /**
     * Constructor for Accessory Information Service
     * @param name Device name
     * @param manu Manufacturer name
     * @param sn Serial number
     * @param model Model identifier
     * @param version Firmware version
     * @param nBlinks Number of times to blink LED during identify
     */
    DEV_Identify(const char *name, 
                 const char *manu, 
                 const char *sn, 
                 const char *model, 
                 const char *version, 
                 int nBlinks) : Service::AccessoryInformation() 
    { 
        // Create required characteristics
        new Characteristic::Name(name);
        new Characteristic::Manufacturer(manu);
        new Characteristic::SerialNumber(sn);
        new Characteristic::Model(model);
        new Characteristic::FirmwareRevision(version);
        
        // Store identify characteristic reference
        identify = new Characteristic::Identify();
        
        // Store blink count and configure LED pin
        this->nBlinks = nBlinks;
        pinMode(homeSpan.getStatusPin(), OUTPUT);

        // Log initialization
        Serial.printf("Configuring Accessory Information:\n");
        Serial.printf("  Name: %s\n", name);
        Serial.printf("  Manufacturer: %s\n", manu);
        Serial.printf("  Serial: %s\n", sn);
        Serial.printf("  Model: %s\n", model);
        Serial.printf("  Version: %s\n", version);
        Serial.printf("  Identify Blinks: %d\n", nBlinks);
    }

    /**
     * Update method - Handles identify request by blinking LED
     * @return true when identify sequence complete
     */
    boolean update() {
        LOG1("Starting identify sequence: ");
        LOG1(nBlinks);
        LOG1(" blinks\n");

        // Blink LED specified number of times
        for(int i = 0; i < nBlinks; i++) {
            digitalWrite(homeSpan.getStatusPin(), LOW);   // LED on
            delay(250);                                   // Wait 250ms
            digitalWrite(homeSpan.getStatusPin(), HIGH);  // LED off
            delay(250);                                   // Wait 250ms
        }

        LOG1("Identify sequence complete\n");
        return true;
    }
};

#endif
