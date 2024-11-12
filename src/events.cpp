#include "events.h"
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
void APButtonEvent(BitsAndDroidsFlightConnector &conn, APRow row,
                   TFT_eSPI &tft) {
  switch (row) {
  case AP_ALTITUDE:
    conn.send(sendAPAltitudeHold);
    break;
  case AP_HEADING:
    conn.send(sendAPHeadingHold);
    break;
  case AP_VERTICAL_SPEED:
    conn.send(sendAPVSHold);
    break;
  default:
    break;
  }
}
