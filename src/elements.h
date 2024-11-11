// Created by DaveRiedel on 24-8-2022.
//
// TODO MOVE ALL COORDINATES HERE
#ifndef FLIGHTCOMPANION_ELEMENTS_H
#define FLIGHTCOMPANION_ELEMENTS_H

#include "Arduino.h"
#include "TFT_eSPI.h"
#include <cstdint>
#define xMargin 30
#define margin 20

#define APFirstColXCoords 120
#define APSecondColXCoords 293

#define APFirtstRowYCoords 60
#define APSecondRowYCoords 100
#define APThirdRowYCoords 140
#define APFourthRowYCoords 180
#define APFifthRowYCoords 220
#define APSixthRowYCoords 260
#define AP_INDICATOR_X (APSecondColXCoords + 120)
#define AP_INDICATOR_SIZE 21
#define AP_INDICATOR_RADIUS 5

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

void drawRoundedLeftTriangle(TFT_eSPI &tft, int x, int y, int size,
                             int cornerRadius, uint32_t color) {
  // Calculate triangle points
  int topX = x + size;
  int topY = y;
  int bottomX = x + size;
  int bottomY = y + size;
  int tipX = x + (size / 3);
  int tipY = y + (size / 2);

  // Draw main triangle
  tft.fillTriangle(topX, topY, bottomX, bottomY, tipX, tipY, color);

  // Add rounded corners using circles
  // tft.fillCircle(topX + cornerRadius, topY + cornerRadius, cornerRadius,
  //                color); // Top right corner
  // tft.fillCircle(bottomX - cornerRadius, bottomY - cornerRadius,
  // cornerRadius,
  //                color);                           // Bottom right corner
  // tft.fillCircle(tipX, tipY, cornerRadius, color); // Left point
}

void drawAPIndicator(TFT_eSPI &tft, APRow activeRow) {
  // Clear all indicators first
  for (int i = 0; i < 3; i++) {
    drawRoundedLeftTriangle(tft, AP_INDICATOR_X, AP_ROW_COORDS[i],
                            AP_INDICATOR_SIZE, AP_INDICATOR_RADIUS, TFT_BLACK);
  }

  // Draw active indicator
  drawRoundedLeftTriangle(tft, AP_INDICATOR_X, AP_ROW_COORDS[activeRow],
                          AP_INDICATOR_SIZE, AP_INDICATOR_RADIUS, TFT_GREEN);
}
#endif // FLIGHTCOMPANION_ELEMENTS_H
