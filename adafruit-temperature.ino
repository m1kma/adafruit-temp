//
// Mika Mäkelä - 2018
//
// Based to Adafruit example:
//
// Adafruit IO Temperature & Humidity Example
// Tutorial Link: https://learn.adafruit.com/adafruit-io-basics-temperature-and-humidity
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016-2017 Adafruit Industries
// Licensed under the MIT license.

#include "config.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// create DHT22 instance
DHT_Unified dht_1(13, DHT22);
DHT_Unified dht_2(12, DHT22);
DHT_Unified dht_3(14, DHT22);

AdafruitIO_Feed *io_temperature_1 = io.feed("temperaturesauna");
AdafruitIO_Feed *io_humidity_1 = io.feed("humiditysauna");

AdafruitIO_Feed *io_temperature_2 = io.feed("temperature");
AdafruitIO_Feed *io_humidity_2 = io.feed("humidity");

AdafruitIO_Feed *io_temperature_3 = io.feed("temperature3");
AdafruitIO_Feed *io_humidity_3 = io.feed("humidity3");

void setup()
{
    pinMode(5, OUTPUT); // TPL5110 donepin
    digitalWrite(5, LOW);

    pinMode(0, OUTPUT); // red led
    pinMode(2, OUTPUT); // blue led

    // start the serial connection
    Serial.begin(115200);
    while (!Serial);

    sensors_event_t event_dht1_t;
    sensors_event_t event_dht2_t;
    sensors_event_t event_dht3_t;
    sensors_event_t event_dht1_h;
    sensors_event_t event_dht2_h;
    sensors_event_t event_dht3_h;

    float celsius_1 = 0;
    float humidity_1 = 0;

    float celsius_2 = 0;
    float humidity_2 = 0;

    float celsius_3 = 0;
    float humidity_3 = 0;

    // ####### Connect to io.adafruit.com #######

    Serial.println("Connecting to Adafruit IO...");
    io.connect();

    int count = 0;

    // wait for a connection - blink led
    while (io.status() < AIO_CONNECTED)
    {
        Serial.println(io.statusText());

        digitalWrite(2, LOW);
        delay(150);
        digitalWrite(2, HIGH);
        delay(450);

        if (count > 20)
        {
            count = 0;
            
            // toggle DONE so TPL knows to cut power!
            while (1)
            {
                digitalWrite(5, HIGH);
                delay(1);
                digitalWrite(5, LOW);
                delay(1);
            }
        }
        count++;
    }

    // we are connected
    Serial.println();
    Serial.println(io.statusText());
    io.run();

    digitalWrite(0, HIGH); // turn off red led

    // ####### Read sensors ########

    Serial.println("Initialize DHT...");

    // initialize dht22
    dht_1.begin();
    dht_2.begin();
    dht_3.begin();

    delay(1000);
    dht_1.temperature().getEvent(&event_dht1_t);
    dht_2.temperature().getEvent(&event_dht2_t);
    dht_3.temperature().getEvent(&event_dht3_t);
    dht_1.humidity().getEvent(&event_dht1_h);
    dht_2.humidity().getEvent(&event_dht2_h);
    dht_3.humidity().getEvent(&event_dht3_h);
    delay(500);

    

    for (size_t i = 0; i < 5; i++)
    {
        dht_1.temperature().getEvent(&event_dht1_t);
        dht_2.temperature().getEvent(&event_dht2_t);

        // 1min = 60sec = 60000ms
        // If the first read fails blink the leds and try again
        if (isnan(event_dht1_t.temperature) || isnan(event_dht2_t.temperature))
        {
            Serial.print("DHT 1: ");
            Serial.println(event_dht1_t.temperature);
            Serial.print("DHT 2: ");
            Serial.println(event_dht2_t.temperature);
            Serial.print("DHT 3: ");
            Serial.println(event_dht3_t.temperature);

            digitalWrite(2, LOW);
            delay(1500);
            digitalWrite(2, HIGH);
            delay(1500);

            //Serial.println("VIRHE; lukemaa ei saatu. Going into deep sleep");
            //ESP.deepSleep(900e6);

            if (i >= 4)
            {
                Serial.println("VIRHE; lukemaa ei saatu. Power OFF...");

                // toggle DONE so TPL knows to cut power!
                while (1)
                {
                    digitalWrite(5, HIGH);
                    delay(1);
                    digitalWrite(5, LOW);
                    delay(1);
                }
            }
        }
        else
        {
            break;
        }
    }

    // Read success
    digitalWrite(0, LOW);
    delay(200);
    digitalWrite(0, HIGH);

    celsius_1 = event_dht1_t.temperature;
    celsius_2 = event_dht2_t.temperature;
    celsius_3 = event_dht3_t.temperature;
    humidity_1 = event_dht1_h.relative_humidity;
    humidity_2 = event_dht2_h.relative_humidity;
    humidity_3 = event_dht3_h.relative_humidity;

    Serial.println();
    Serial.print("celsius 1: ");
    Serial.println(celsius_1);

    Serial.println();
    Serial.print("celsius 2: ");
    Serial.println(celsius_2);

    Serial.println();
    Serial.print("celsius 3: ");
    Serial.println(celsius_3);

    Serial.print("humidity 1: ");
    Serial.print(humidity_1);
    Serial.println("%");

    Serial.print("humidity 2: ");
    Serial.print(humidity_2);
    Serial.println("%");

    Serial.print("humidity 3: ");
    Serial.print(humidity_3);
    Serial.println("%");

    // Send data to the Adafruit IO
    io_temperature_1->save(celsius_1);
    io_humidity_1->save(humidity_1);

    io_temperature_2->save(celsius_2);
    io_humidity_2->save(humidity_2);

    io_temperature_3->save(celsius_3);
    io_humidity_3->save(humidity_3);

    digitalWrite(0, LOW);
    delay(200);
    digitalWrite(0, HIGH);

    Serial.println("Power OFF...");

    // toggle DONE so TPL knows to cut power!
    while (1)
    {
        digitalWrite(5, HIGH);
        delay(1);
        digitalWrite(5, LOW);
        delay(1);
    }

    //Serial.println("Going into deep sleep for 10 mins");
    //ESP.deepSleep(600e6);

}

void loop()
{
    delay(1000);
}
