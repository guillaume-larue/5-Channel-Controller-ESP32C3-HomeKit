/*********************************************************************************
* MIT License
* 
* Copyright (c) 2023 Guillaume Larue
* 
* This firmware uses the HomeSpan library (https://github.com/HomeSpan/HomeSpan)
* by Gregg E. Berman, licensed under the MIT License.
*********************************************************************************/

#ifndef DEV_LED_H
#define DEV_LED_H

#include "HomeSpan.h"

////////////////////////////////////
//        DIMMABLE LED            //
////////////////////////////////////

struct DEV_DimmableLED : Service::LightBulb { 
    // Class members
    LedPin *ledPin;                    // Reference to Led Pin
    SpanCharacteristic *power;         // Reference to the On Characteristic
    SpanCharacteristic *level;         // Reference to the Brightness Characteristic
    
    /**
     * Constructor for Dimmable LED
     * @param pin GPIO pin number for the LED
     */
    DEV_DimmableLED(int pin) : Service::LightBulb() { 
        // Initialize characteristics with NVS storage enabled
        power = new Characteristic::On(0, true);
        level = new Characteristic::Brightness(50, true);
        level->setRange(5, 100, 1);

        // Configure PWM LED output
        this->ledPin = new LedPin(pin);

        // Log initialization
        Serial.print("Configuring Dimmable LED: Pin=");
        Serial.print(ledPin->getPin());
        Serial.print("\n");

        // Restore LED state from NVS
        bool p = power->getVal();
        int l = level->getVal();
        ledPin->set(p * l);
    }
    
    /**
     * Update method called when characteristics change
     * @return true if update successful
     */
    boolean update() {
        // Log current state
        LOG1("Updating Dimmable LED on pin="); 
        LOG1(ledPin->getPin());
        LOG1(": Current Power=");
        LOG1(power->getVal()?"true":"false");
        LOG1(" Current Brightness=");
        LOG1(level->getVal());
        
        // Log changes if any
        if(power->updated()) {
            LOG1(" New Power=");
            LOG1(power->getNewVal()?"true":"false");
        }
        
        if(level->updated()) {
            LOG1(" New Brightness=");
            LOG1(level->getNewVal());
        }
        
        LOG1("\n");
        
        // Update PWM output
        ledPin->set(power->getNewVal() * level->getNewVal()); 
        return true;
    }
};

////////////////////////////////////
//           RGB LED              //
////////////////////////////////////

struct DEV_RgbLED : Service::LightBulb {
    // Class members
    LedPin *redPin, *greenPin, *bluePin;
    SpanCharacteristic *power;         // On/Off control
    SpanCharacteristic *H;             // Hue (0-360)
    SpanCharacteristic *S;             // Saturation (0-100%)
    SpanCharacteristic *V;             // Brightness (0-100%)
    
    /**
     * Constructor for RGB LED
     * @param red_pin GPIO pin for red channel
     * @param green_pin GPIO pin for green channel
     * @param blue_pin GPIO pin for blue channel
     */
    DEV_RgbLED(int red_pin, int green_pin, int blue_pin) : Service::LightBulb() {
        // Initialize characteristics with NVS storage enabled
        power = new Characteristic::On(0, true);
        H = new Characteristic::Hue(0, true);
        S = new Characteristic::Saturation(0, true);
        V = new Characteristic::Brightness(100, true);
        V->setRange(5, 100, 1);

        // Configure PWM LED outputs
        this->redPin = new LedPin(red_pin);
        this->greenPin = new LedPin(green_pin);
        this->bluePin = new LedPin(blue_pin);

        // Log initialization
        char cBuf[128];
        sprintf(cBuf, "Configuring RGB LED: Pins=(%d,%d,%d)\n", 
                redPin->getPin(), greenPin->getPin(), bluePin->getPin());
        Serial.print(cBuf);

        // Restore LED state from NVS
        restoreState();
    }

    /**
     * Restore LED state from Non-Volatile Storage
     */
    void restoreState() {
        float h = H->getVal<float>();
        float s = S->getVal<float>();
        float v = V->getVal<float>();
        bool p = power->getVal();

        float r, g, b;
        LedPin::HSVtoRGB(h, s / 100.0, v / 100.0, &r, &g, &b);

        int R = p * r * 100;
        int G = p * g * 100;
        int B = p * b * 100;

        redPin->set(R);
        greenPin->set(G);
        bluePin->set(B);
    }

    /**
     * Update method called when characteristics change
     * @return true if update successful
     */
    boolean update() {
        boolean p;
        float v, h, s, r, g, b;
        char cBuf[128];

        // Get current values
        h = H->getVal<float>();
        s = S->getVal<float>();
        v = V->getVal<float>();
        p = power->getVal();

        // Log update start
        sprintf(cBuf, "Updating RGB LED: Pins=(%d,%d,%d): ",
                redPin->getPin(), greenPin->getPin(), bluePin->getPin());
        LOG1(cBuf);

        // Log power changes
        if(power->updated()) {
            p = power->getNewVal();
            sprintf(cBuf, "Power=%s->%s, ",
                    power->getVal()?"true":"false", p?"true":"false");
        } else {
            sprintf(cBuf, "Power=%s, ", p?"true":"false");
        }
        LOG1(cBuf);
        
        // Log Hue changes
        if(H->updated()) {
            h = H->getNewVal<float>();
            sprintf(cBuf, "H=%.0f->%.0f, ", H->getVal<float>(), h);
        } else {
            sprintf(cBuf, "H=%.0f, ", h);
        }
        LOG1(cBuf);

        // Log Saturation changes
        if(S->updated()) {
            s = S->getNewVal<float>();
            sprintf(cBuf, "S=%.0f->%.0f, ", S->getVal<float>(), s);
        } else {
            sprintf(cBuf, "S=%.0f, ", s);
        }
        LOG1(cBuf);

        // Log Brightness changes
        if(V->updated()) {
            v = V->getNewVal<float>();
            sprintf(cBuf, "V=%.0f->%.0f ", V->getVal<float>(), v);
        } else {
            sprintf(cBuf, "V=%.0f ", v);
        }
        LOG1(cBuf);

        // Convert HSV to RGB
        LedPin::HSVtoRGB(h, s/100.0, v/100.0, &r, &g, &b);

        // Calculate final PWM values
        int R = p * r * 100;
        int G = p * g * 100;
        int B = p * b * 100;

        // Log RGB values
        sprintf(cBuf, "RGB=(%d,%d,%d)\n", R, G, B);
        LOG1(cBuf);

        // Update PWM outputs
        redPin->set(R);
        greenPin->set(G);
        bluePin->set(B);
        
        return true;
    }
};

#endif
