#include "poll_fields.h"

bool lastApVSActive = false;
bool lastApAltActive = false;
bool lastApHDGActive = false;
bool lastAPMasterActive = false;

bool getLastApVSActive() { return lastApVSActive; }
void setLastApVSActive(bool value) { lastApVSActive = value; }

bool getLastApAltActive() { return lastApAltActive; }
void setLastApAltActive(bool value) { lastApAltActive = value; }

bool getLastApHDGActive() { return lastApHDGActive; }
void setLastApHDGActive(bool value) { lastApHDGActive = value; }

void poll_fields(BitsAndDroidsFlightConnector &conn, MODES mode,
                 TFT_eSPI &tft) {
  switch (mode) {
  case RADIO:
    break;
  case FUEL:
    break;
  case G1000:
    break;
  case ALT:
    poll_ap_fields(conn, tft);
    break;
  default:
    break;
  }
}
void poll_ap_fields(BitsAndDroidsFlightConnector &conn, TFT_eSPI &tft) {
  bool apVSActive = conn.getAPVerticalHoldOn();
  if (getLastApVSActive() != apVSActive) {
    setLastApVSActive(apVSActive);
    drawAPFields(tft, conn);
  }
  bool apAltActive = conn.getAPAltitudeLockOn();
  if (getLastApAltActive() != apAltActive) {
    setLastApAltActive(apAltActive);
    drawAPFields(tft, conn);
  }
  bool apHDGActive = conn.getAPHeadingLockOn();
  if (getLastApHDGActive() != apHDGActive) {
    setLastApHDGActive(apHDGActive);
    drawAPFields(tft, conn);
  }
  bool apMasterActive = conn.getAPMasterOn();
  if (lastAPMasterActive != apMasterActive) {
    lastAPMasterActive = apMasterActive;
    drawAPButton(tft, conn);
  }
}
