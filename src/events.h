
#ifndef FLIGHTCOMPANION_EVENTS_H
#define FLIGHTCOMPANION_EVENTS_H

#include "elements.h"
#include <Arduino.h>
#include <BitsAndDroidsFlightConnector.h>
enum ENCODER_DIRECTION { ENCODER_UP, ENCODER_DOWN };

void APEncoderEvent(BitsAndDroidsFlightConnector &conn,
                    ENCODER_DIRECTION direction, APRow row) {
  switch (row) {
    // case ALTAP:
    //   Serial.println(conn.send());
    //   break;
    // case HDGAP:
    //   switchHDGTarget(direction);
    //   break;
    // case VSAP:
    //   switchVSTarget(direction);
    //   break;
    // default:
    //   break;
    // }
  }
}
#endif // FLIGHTCOMPANION_ELEMENTS_H
