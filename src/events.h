
#ifndef FLIGHTCOMPANION_EVENTS_H
#define FLIGHTCOMPANION_EVENTS_H

#include "elements.h"
#include <Arduino.h>
#include <BitsAndDroidsFlightConnector.h>
enum ENCODER_DIRECTION { ENCODER_UP, ENCODER_DOWN };

void APEncoderEvent(BitsAndDroidsFlightConnector &conn,
                    ENCODER_DIRECTION direction, APRow row);

void APButtonEvent(BitsAndDroidsFlightConnector &conn, APRow row,
                   TFT_eSPI &tft);
#endif // FLIGHTCOMPANION_ELEMENTS_H
