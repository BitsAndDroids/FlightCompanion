// Created by DaveRiedel on 24-8-2022.
//
// TODO MOVE ALL COORDINATES HERE
#ifndef FLIGHTCOMPANION_ELEMENTS_H
#define FLIGHTCOMPANION_ELEMENTS_H

#include "Arduino.h"
#include "BitsAndDroidsFlightConnector.h"
#include "TFT_eSPI.h"
#include <cstdint>
#define xMargin 30
#define margin 20

#define APFirstColXCoords 150
#define APSecondColXCoords 323

#define APFirtstRowYCoords 60
#define APSecondRowYCoords 100
#define APThirdRowYCoords 140
#define APFourthRowYCoords 180
#define APFifthRowYCoords 220
#define APSixthRowYCoords 260
#define AP_INDICATOR_X (APSecondColXCoords + 120)
#define AP_INDICATOR_SIZE 21
#define AP_INDICATOR_RADIUS 5

#define AP_BTN_RADIUS 40

const int AP_ROW_COORDS[] = {APFirtstRowYCoords, APThirdRowYCoords,
                             APFifthRowYCoords};

struct square {
  int xStart;
  int yStart;
  int width;
  int height;
  int16_t color;
};

struct roundedSquare {
  int xStart;
  int yStart;
  int width;
  int height;
  byte cornerRadius;
  int16_t color;
  int command;
};

struct circle {
  int xStart;
  int yStart;
  int radius;
  int16_t color;
  int command;
};

struct triangle {
  int point1X;
  int point1Y;
  int point2X;
  int point2Y;
  int point3X;
  int point3Y;
  int16_t color;
};

struct arrowIndicator {
  int xStart;
  int yStart;
  int width;
  int height;
  int16_t color;
};

enum APRow {
  AP_ALTITUDE,
  AP_HEADING,
  AP_VERTICAL_SPEED,
};

extern circle APMasterTouchZone;

void drawFillRoundRect(TFT_eSPI &tft, roundedSquare toDraw);

void drawRoundedLeftTriangle(TFT_eSPI &tft, int x, int y, int size,
                             int cornerRadius, uint32_t color);

void drawAPIndicator(TFT_eSPI &tft, APRow activeRow);

void drawAPFieldsBg(TFT_eSPI &tft, BitsAndDroidsFlightConnector &conn);

void drawAPFields(TFT_eSPI &tft, BitsAndDroidsFlightConnector &conn);

void drawAPButton(TFT_eSPI &tft, BitsAndDroidsFlightConnector &conn);
#endif // FLIGHTCOMPANION_ELEMENTS_H
