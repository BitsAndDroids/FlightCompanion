#include "elements.h"
#include "TFT_eSPI.h"

circle APMasterTouchZone = {15, 110, 90, 15, sendApMasterOn};

void drawFillRoundRect(TFT_eSPI &tft, roundedSquare toDraw) {
  tft.fillRoundRect(toDraw.xStart, toDraw.yStart, toDraw.width, toDraw.height,
                    toDraw.cornerRadius, toDraw.color);
}

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
void drawAPFieldsBg(TFT_eSPI &tft, BitsAndDroidsFlightConnector &conn) {
  bool apVSActive = conn.getAPVerticalHoldOn();
  bool apAltActive = conn.getAPAltitudeLockOn();
  bool apHDGActive = conn.getAPHeadingLockOn();

  roundedSquare sqHDGLeft = {
      APFirstColXCoords - 8,           APThirdRowYCoords - 10, 100, 40, 5,
      static_cast<int16_t>(TFT_ORANGE)};
  // This is a weird name for a square...
  roundedSquare sqHDGRight = {
      APSecondColXCoords - 8,
      APThirdRowYCoords - 10,
      100,
      40,
      5,
      static_cast<int16_t>(apHDGActive ? TFT_GREEN : TFT_ORANGE)};
  roundedSquare sqVSLeft = {
      APFirstColXCoords - 8,        APFifthRowYCoords - 10, 100, 40, 5,
      static_cast<int16_t>(TFT_RED)};
  // This is a weird name for a square...
  roundedSquare sqVSRight = {
      APSecondColXCoords - 8,
      APFifthRowYCoords - 10,
      100,
      40,
      5,
      static_cast<int16_t>(apVSActive ? TFT_GREEN : TFT_RED)};

  roundedSquare sqAltLeft = {
      APFirstColXCoords - 8,         APFirtstRowYCoords - 10, 100, 40, 5,
      static_cast<int16_t>(TFT_GOLD)};
  // This is a weird name for a square...
  roundedSquare sqAltRight = {
      APSecondColXCoords - 8,
      APFirtstRowYCoords - 10,
      100,
      40,
      5,
      static_cast<int16_t>(apAltActive ? TFT_GREEN : TFT_GOLD)};

  drawFillRoundRect(tft, sqAltLeft);
  drawFillRoundRect(tft, sqAltRight);
  drawFillRoundRect(tft, sqHDGLeft);
  drawFillRoundRect(tft, sqHDGRight);

  drawFillRoundRect(tft, sqVSLeft);
  drawFillRoundRect(tft, sqVSRight);
}

void drawAPButton(TFT_eSPI &tft, BitsAndDroidsFlightConnector &conn) {
  bool apMasterActive = conn.getAPMasterOn();

  tft.fillCircle(15 + AP_BTN_RADIUS, tft.getViewportHeight() / 2, AP_BTN_RADIUS,
                 TFT_SILVER);

  tft.fillCircle(15 + AP_BTN_RADIUS, tft.getViewportHeight() / 2,
                 AP_BTN_RADIUS - 8,
                 apMasterActive ? TFT_DARKGREEN : TFT_MAROON);

  tft.fillCircle(15 + AP_BTN_RADIUS, tft.getViewportHeight() / 2,
                 AP_BTN_RADIUS - 16, apMasterActive ? TFT_GREEN : TFT_RED);
}

void drawAPFields(TFT_eSPI &tft, BitsAndDroidsFlightConnector &conn) {
  bool apVSActive = conn.getAPVerticalHoldOn();
  bool apAltActive = conn.getAPAltitudeLockOn();
  bool apHDGActive = conn.getAPHeadingLockOn();

  drawAPFieldsBg(tft, conn);
  drawAPButton(tft, conn);
  tft.setFreeFont(&FreeSansBold12pt7b);

  tft.setTextColor(TFT_BLACK, static_cast<int16_t>(TFT_GOLD));
  tft.drawString("ALT", APFirstColXCoords, APFirtstRowYCoords);
  if (apAltActive) {
    tft.setTextColor(TFT_BLACK, static_cast<int16_t>(TFT_GREEN));
  }
  tft.drawString("ALT", APSecondColXCoords, APFirtstRowYCoords);

  tft.setTextColor(TFT_BLACK, static_cast<int16_t>(TFT_ORANGE));
  tft.drawString("HDG", APFirstColXCoords, APThirdRowYCoords);
  if (apHDGActive) {
    tft.setTextColor(TFT_BLACK, static_cast<int16_t>(TFT_GREEN));
  }
  tft.drawString("HDG", APSecondColXCoords, APThirdRowYCoords);

  tft.setTextColor(TFT_BLACK, static_cast<int16_t>(TFT_RED));
  tft.drawString("VS", APFirstColXCoords, APFifthRowYCoords);
  if (apVSActive) {
    tft.setTextColor(TFT_BLACK, static_cast<int16_t>(TFT_GREEN));
  }
  tft.drawString("VS", APSecondColXCoords, APFifthRowYCoords);
}
