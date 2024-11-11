
#ifndef FLIGHTCOMPANION_EVENTS_H
#define FLIGHTCOMPANION_EVENTS_H

#include "elements.h"
#include <Arduino.h>
#include <BitsAndDroidsFlightConnector.h>
enum ENCODER_DIRECTION { ENCODER_UP, ENCODER_DOWN };

void APEncoderEvent(BitsAndDroidsFlightConnector &conn,
                    ENCODER_DIRECTION direction, APRow row) {
  switch (row) {
  case AP_ALTITUDE:
    conn.send(direction == ENCODER_UP ? sendAPAltitudeInc : sendAPAltitudeDec);
    break;
  case AP_HEADING:
    conn.send(direction == ENCODER_UP ? sendHeadingBugInc : sendHeadingBugDec);
    break;
  case AP_VERTICAL_SPEED:
    conn.send(direction == ENCODER_UP ? sendAPVSInc : sendAPVSDec);
    break;
  default:
    break;
  }
}
#endif // FLIGHTCOMPANION_ELEMENTS_H
