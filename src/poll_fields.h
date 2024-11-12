#ifndef FLIGHTCOMPANION_POLL_FIELDS_H
#define FLIGHTCOMPANION_POLL_FIELDS_H

#include "BitsAndDroidsFlightConnector.h"
#include "TFT_eSPI.h"
#include <elements.h>

enum MODES { RADIO = 1, FUEL = 2, G1000 = 3, ALT = 4 };
void poll_fields(BitsAndDroidsFlightConnector &conn, MODES mode, TFT_eSPI &tft);

extern bool lastApVSActive;
extern bool lastApAltActive;
extern bool lastApHDGActive;

bool getLastApVSActive();
void setLastApVSActive(bool value);

bool getLastApAltActive();
void setLastApAltActive(bool value);

bool getLastApHDGActive();
void setLastApHDGActive(bool value);

void poll_fields(BitsAndDroidsFlightConnector &conn, MODES mode, TFT_eSPI &tft);
void poll_ap_fields(BitsAndDroidsFlightConnector &conn, TFT_eSPI &tft);

#endif
