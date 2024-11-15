#include "elements.h"
#include "events.h"
#include "poll_fields.h"
#include <Arduino.h>
#include <BitsAndDroidsFlightConnector.h>
#include <ESP32Encoder.h>
#include <SPI.h>
#include <TFT_eSPI.h>
// TODO CUT UP THIS FILE INTO SEPERATE UTIL LIBRARY

byte btnDual = 32;
byte btnLower = 12;

TFT_eSPI tft = TFT_eSPI();
BitsAndDroidsFlightConnector conn = BitsAndDroidsFlightConnector();
uint16_t t_x = 0, t_y = 0; // To store the touch coordinates

ESP32Encoder encoderOuter;
ESP32Encoder encoderInner;
ESP32Encoder encoderLower;

enum APTarget { ALTAP, HDGAP, VSAP };
byte APTargetSelected = ALTAP;
byte mode = RADIO;
bool radioDrawn = false;
bool fuelDrawn = false;
bool g1000IsDrawn = false;
bool altDrawn = false;
float oldComs[2];

void resetDrawBooleans() {
  radioDrawn = false;
  fuelDrawn = false;
  g1000IsDrawn = false;
  altDrawn = false;
}

int16_t mainBgColor = TFT_BLACK;

// We can replace the fillRect function in the initializeRadio function with the
// following line
int screenWidth = tft.getViewportHeight();
int screenHeight = tft.getViewportWidth();
// Keeps track which com to display com 1, 2, nav 1,2
byte radioDisplayed = 0;
// Here we've created our struct

void drawFillSquare(square toDraw) {
  tft.fillRect(toDraw.xStart, toDraw.yStart, toDraw.width, toDraw.height,
               toDraw.color);
}

void drawFillTriangle(triangle toDraw) {
  tft.fillTriangle(toDraw.point1X, toDraw.point1Y, toDraw.point2X,
                   toDraw.point2Y, toDraw.point3X, toDraw.point3Y,
                   toDraw.color);
}

square values[4] = {{0}};
square barContainer[4] = {{0}};

square greySquare = {xMargin, margin, screenWidth - margin * 2, 150,
                     static_cast<int16_t>(TFT_SILVER)};

byte frameWidth = 4;
// The greySquare will be our reference point for almost all the inner values.
square innerSquare = {greySquare.xStart + frameWidth,
                      greySquare.yStart + frameWidth,
                      greySquare.width - frameWidth * 2,
                      greySquare.height - frameWidth * 2, mainBgColor};

square comTextSquare = {xMargin, margin, 90, 30, TFT_BLACK};

byte swapWidth = 40;
byte swapHeight = 25;

roundedSquare swapButton = {
    innerSquare.xStart + (innerSquare.width / 2) - (swapWidth / 2),
    innerSquare.yStart + (innerSquare.height / 2) - (swapHeight / 2),
    swapWidth,
    swapHeight,
    4,
    static_cast<int16_t>(0xF693)};

byte swapGraphSquareWidth = 10;
byte swapGraphSquareHeight = 7;

square swapGraphicSquare = {
    swapButton.xStart + (swapButton.width / 2) - (swapGraphSquareWidth / 2),
    swapButton.yStart + (swapButton.height / 2) - (swapGraphSquareHeight / 2),
    swapGraphSquareWidth, swapGraphSquareHeight, TFT_BLACK};

// This wil ensure our triangle has equal sides
byte triangleBoundaries = 14;

triangle leftPointTriangle = {
    // First point here move to the left (subtract) to create the left facing
    // point
    swapGraphicSquare.xStart - triangleBoundaries,
    swapGraphicSquare.yStart + (swapGraphicSquare.height / 2),
    // Second point
    swapGraphicSquare.xStart,
    swapGraphicSquare.yStart + swapGraphicSquare.height / 2 -
        triangleBoundaries / 2,
    // Third point
    swapGraphicSquare.xStart,
    swapGraphicSquare.yStart + swapGraphicSquare.height / 2 +
        triangleBoundaries / 2};

triangle rightPointTriangle = {
    // First point here move to the right (add) to create the right facing point
    swapGraphicSquare.xStart + swapGraphicSquare.width + triangleBoundaries,
    swapGraphicSquare.yStart + (swapGraphicSquare.height / 2),
    // Second point
    swapGraphicSquare.xStart + swapGraphicSquare.width,
    swapGraphicSquare.yStart + swapGraphicSquare.height / 2 -
        triangleBoundaries / 2,
    // Third point
    swapGraphicSquare.xStart + swapGraphicSquare.width,
    swapGraphicSquare.yStart + swapGraphicSquare.height / 2 +
        triangleBoundaries / 2};
// SOFTKEYS GPS
int g1000Actions[] = {5012, 5013, 5014, 5015, 5016, 5017,
                      5018, 5019, 5020, 5021, 5022, 5023};
int radioActions[] = {sendATCMenu1, sendATCMenu2, sendATCMenu3, sendATCMenu4,
                      sendATCMenu5, sendATCMenu6, sendATCMenu7, sendATCMenu8,
                      sendATCMenu9, sendATCMenu0};

byte buttonTextArray[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
byte buttonTextArrayG1000[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
roundedSquare buttonArray[12] = {{0}};

void addButtons() {
  tft.setFreeFont(&FreeSansBold12pt7b);
  int size = 10;
  if (mode == RADIO) {
    size = 10;
  } else if (mode == G1000) {
    size = 12;
  }
  for (int i = 0; i < size / 2; i++) {

    // These values are shared accross each button
    int height = 50;
    int width = 50;
    auto color = static_cast<int16_t>(0xF693);
    byte cornerRadius = 4;

    // Because each column has the same X starting coordinate
    // We can create a new button for the top and bottom row everytime we loop
    // throught this function How higher the i (amnt of loops) is the more
    // squares have been added already By knowing the amount we then know the
    // new starting position by multiplying i by the width + margins in between
    // buttons
    roundedSquare newButtonTop = {
        xMargin + ((width * i) + (margin * i)),
        greySquare.yStart + greySquare.height + margin,
        width,
        height,
        cornerRadius,
        color

    };

    // Most values are the same as the top row except for the starting Y
    // For this we take the start of Y from the first button and add the height
    // + margin
    roundedSquare newButtonBottom = {xMargin + ((width * i) + (margin * i)),
                                     newButtonTop.yStart + height + margin,
                                     width,
                                     height,
                                     cornerRadius,
                                     color};
    String textTop;
    String textBottom;
    if (mode == RADIO) {
      textTop = String(buttonTextArray[i]);
      textBottom = String(buttonTextArray[i + size / 2]);
    } else if (mode == G1000) {
      textTop = String(buttonTextArrayG1000[i]);
      textBottom = String(buttonTextArrayG1000[i + size / 2]);
    }

    // We store these values in our array
    // I want them to be organized, but in our loop we've added the button that
    // will hold 1 and 6 Because we are maintaining the same patern we can add
    // the second button at i + 5 In the end this will ensure that our array
    // holds 1,2...9,0 in order
    buttonArray[i] = newButtonTop;
    buttonArray[i + size / 2] = newButtonBottom;
    drawFillRoundRect(tft, newButtonTop);
    drawFillRoundRect(tft, newButtonBottom);
    tft.setTextColor(TFT_BLACK, color);
    tft.drawString(textTop, newButtonTop.xStart + newButtonTop.width / 2 - 6,
                   newButtonTop.yStart + newButtonTop.height / 2 - 9);
    byte offset = 6;
    if (textBottom.length() > 1) {
      offset = 14;
    }
    tft.drawString(textBottom,
                   newButtonBottom.xStart + newButtonBottom.width / 2 - offset,
                   newButtonBottom.yStart + newButtonBottom.height / 2 - 9);
  }
}

void setCom(byte place, float text) {

  // tft.setFreeFont(FSSB24);
  tft.setFreeFont(&FreeSansBold24pt7b);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  byte yText = greySquare.yStart + (greySquare.height / 2) - 16;

  if (place == 0) {

    tft.drawFloat(text, 3, xMargin + 15, yText);

  } else {
    tft.drawFloat(text, 3, swapButton.xStart + swapButton.width + 15, yText);
  }
}

void setHeader(char *header) {
  tft.setFreeFont(&FreeSansBold12pt7b);
  drawFillSquare(comTextSquare);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(header, xMargin, margin);
}

void initializeRadio() {
  // this looks way cleaner, especially if we add more squares

  tft.fillScreen(TFT_BLACK);
  drawFillSquare(greySquare);
  drawFillSquare(innerSquare);
  drawFillRoundRect(tft, swapButton);
  drawFillSquare(swapGraphicSquare);
  drawFillTriangle(rightPointTriangle);
  drawFillTriangle(leftPointTriangle);
  drawFillSquare(comTextSquare);

  setHeader("COM 1");
  addButtons();

  for (int i = 0; i < 10; i++) {
    buttonArray[i].command = radioActions[i];
  }

  setCom(0, (float)conn.getActiveCom1() / 1000);
  setCom(1, (float)conn.getStandbyCom1() / 1000);
}

void radioMode() {
  if (!radioDrawn) {
    initializeRadio();
    resetDrawBooleans();
    radioDrawn = true;
  }
  float com1Active;
  float com1Standby;
  switch (radioDisplayed) {
  case 0:
    com1Active = (float)conn.getActiveCom1() / 1000;
    com1Standby = (float)conn.getStandbyCom1() / 1000;
    break;
  case 1:
    com1Active = (float)conn.getActiveCom2() / 1000;
    com1Standby = (float)conn.getStandbyCom2() / 1000;
    break;
  case 2:
    com1Active = (float)conn.getActiveNav1() / 1000;
    com1Standby = (float)conn.getStandbyNav1() / 1000;
    break;
  case 3:
    com1Active = (float)conn.getActiveNav2() / 1000;
    com1Standby = (float)conn.getStandbyNav2() / 1000;
    break;
  default:
    com1Active = (float)conn.getActiveCom1() / 1000;
    com1Standby = (float)conn.getStandbyCom1() / 1000;
    break;
  }

  if (com1Active != oldComs[0]) {
    setCom(0, com1Active);
    oldComs[0] = com1Active;
  }
  if (com1Standby != oldComs[1]) {
    setCom(1, com1Standby);
    oldComs[1] = com1Standby;
  }
}
long innerCount;
long oldInnerCount;

long outerCount;
long oldOuterCount;

long lowerCount;
long oldLowerCount;
void gpsMode() {}
roundedSquare g1000Menu = {
    xMargin, margin, 100, 50, 4, static_cast<int16_t>(0xF693), 5005};

roundedSquare g1000Procedure = {
    xMargin, g1000Menu.height + g1000Menu.yStart + margin,
    100,     50,
    4,       static_cast<int16_t>(0xF693),
    5006};
roundedSquare g1000FPLN = {g1000Menu.xStart + g1000Menu.width + margin,
                           margin,
                           100,
                           50,
                           4,
                           static_cast<int16_t>(0xF693),
                           5007};

roundedSquare g1000Plus = {g1000FPLN.xStart + g1000FPLN.width + margin,
                           margin,
                           50,
                           50,
                           4,
                           static_cast<int16_t>(0xF693),
                           5008};
roundedSquare g1000Min = {g1000Plus.xStart + g1000Plus.width + margin,
                          margin,
                          50,
                          50,
                          4,
                          static_cast<int16_t>(0xF693),
                          5009};

roundedSquare g1000Dir = {g1000Menu.xStart + g1000Menu.width + margin,
                          g1000Plus.yStart + g1000Plus.height + margin,
                          100,
                          50,
                          4,
                          static_cast<int16_t>(0xF693),
                          5010};

roundedSquare g1000Clear = {g1000Dir.xStart + g1000Dir.width + margin,
                            g1000Plus.yStart + g1000Plus.height + margin,
                            100,
                            50,
                            4,
                            static_cast<int16_t>(0xF693),
                            5011};

roundedSquare g1000Array[] = {g1000Menu, g1000Procedure, g1000FPLN, g1000Plus,
                              g1000Min,  g1000Dir,       g1000Clear};
char *radioHeaders[] = {"COM 1", "COM 2", "NAV 1", "NAV 2"};

void g1000Mode() {
  if (!g1000IsDrawn) {
    tft.fillScreen(TFT_BLACK);
    resetDrawBooleans();
    g1000IsDrawn = true;
    addButtons();

    for (int i = 0; i < 12; i++) {
      buttonArray[i].command = g1000Actions[i];
    }
    drawFillRoundRect(tft, g1000Menu);
    drawFillRoundRect(tft, g1000Procedure);
    drawFillRoundRect(tft, g1000FPLN);
    drawFillRoundRect(tft, g1000Plus);
    drawFillRoundRect(tft, g1000Min);
    drawFillRoundRect(tft, g1000Dir);
    drawFillRoundRect(tft, g1000Clear);
    tft.drawString("MENU", g1000Menu.xStart + g1000Menu.width / 5 - 4,
                   g1000Menu.yStart + g1000Menu.height / 2 - 9);
    tft.drawString("PRCD", g1000Procedure.xStart + g1000Procedure.width / 5 - 4,
                   g1000Procedure.yStart + g1000Procedure.height / 2 - 9);
    tft.drawString("FPLN", g1000FPLN.xStart + g1000FPLN.width / 5 - 3,
                   g1000FPLN.yStart + g1000FPLN.height / 2 - 9);
    tft.drawString("+", g1000Plus.xStart + g1000Plus.width / 3 + 2,
                   g1000Plus.yStart + g1000Plus.height / 2 - 12);
    tft.drawString("-", g1000Min.xStart + g1000Min.width / 3 + 8,
                   g1000Min.yStart + g1000Min.height / 2 - 12);
    tft.drawString("DIR", g1000Dir.xStart + g1000Dir.width / 3 - 2,
                   g1000Dir.yStart + g1000Dir.height / 2 - 9);
    tft.drawString("CLR", g1000Clear.xStart + g1000Clear.width / 3 - 8,
                   g1000Clear.yStart + g1000Clear.height / 2 - 9);
  }
}
float percentage = 0.01;

void switchAPTarget(int dir) {
  if (APTargetSelected == VSAP && dir == 1) {
    APTargetSelected = ALTAP;
  } else if (APTargetSelected == ALTAP && dir == -1) {
    APTargetSelected = VSAP;
  } else {
    APTargetSelected += dir;
  }
  drawAPIndicator(tft, APRow(APTargetSelected));
}

void checkEncoders() {

  innerCount = (long)encoderInner.getCount();

  if (innerCount != oldInnerCount) {
    if (mode == ALT) {
      APEncoderEvent(conn,
                     innerCount > oldInnerCount ? ENCODER_UP : ENCODER_DOWN,
                     APRow(APTargetSelected));
    }
    if (mode == RADIO) {
      int cmd = 0;
      switch (radioDisplayed) {
      case 0:
        cmd =
            (innerCount < oldInnerCount) ? sendCom1FractInc : sendCom1FractDecr;
        break;
      case 1:
        cmd =
            (innerCount < oldInnerCount) ? sendCom2FractInc : sendCom2FractDecr;
        break;
      case 2:
        cmd =
            (innerCount < oldInnerCount) ? sendIncFractNav1 : sendDecFractNav1;
        break;
      case 3:
        cmd =
            (innerCount < oldInnerCount) ? sendIncFractNav2 : sendDecFractNav2;
        break;
      default:
        break;
      }

      conn.send(cmd);
    } else if (mode == FUEL) {
      int gvalue = (innerCount > oldInnerCount) ? 250 : 251;
      Serial.println(gvalue);
    } else if (mode == G1000) {
      // Group down/up GPS
      int gvalue = (innerCount > oldInnerCount) ? 5001 : 5000;
      Serial.println(gvalue);
    }

    oldInnerCount = innerCount;
  }

  outerCount = (long)encoderOuter.getCount();
  if (outerCount != oldOuterCount) {

    if (mode == ALT) {
      switchAPTarget(outerCount > oldOuterCount ? 1 : -1);
    }
    int cmd = 0;
    if (mode == RADIO) {
      switch (radioDisplayed) {
      case 0:
        cmd =
            (outerCount > oldOuterCount) ? sendCom1WholeInc : sendCom1WholeDec;
        break;
      case 1:
        cmd =
            (outerCount > oldOuterCount) ? sendCom2WholeInc : sendCom2WholeDec;
        break;
      case 2:
        cmd =
            (outerCount > oldOuterCount) ? sendIncWholeNav1 : sendDecWholeNav1;
        break;
      case 3:
        cmd =
            (outerCount > oldOuterCount) ? sendIncWholeNav2 : sendDecWholeNav2;
        break;
      default:
        break;
      }

      conn.send(cmd);
    }
    if (mode == G1000) {
      // Page down/up GPS
      int gvalue = (outerCount > oldOuterCount) ? 5003 : 5002;
      Serial.println(gvalue);
    }
    oldOuterCount = outerCount;
  }

  lowerCount = (long)encoderLower.getCount();
  if (abs(lowerCount - oldLowerCount) > 1) {
    mode = (lowerCount > oldLowerCount) ? mode - 1 : mode + 1;
    oldLowerCount = lowerCount;
    if (mode < 1) {
      mode = 4;
    }
    if (mode > 4) {
      mode = 1;
    }
  }

  if (digitalRead(btnDual) == LOW) {
    if (mode == ALT) {
      APButtonEvent(conn, APRow(APTargetSelected), tft);
    } else if (mode == G1000) {
      // GPS enter button
      conn.send(5004);
    } else if (mode == RADIO) {
      int cmd = sendSwapCom1;
      switch (radioDisplayed) {
      case 0:
        cmd = sendSwapCom1;
        break;
      case 1:
        cmd = sendSwapCom2;
        break;
      case 2:
        cmd = sendSwapNav1;
        break;
      case 3:
        cmd = sendSwapNav2;
        break;
      default:
        break;
      }
      conn.send(cmd);
    }
    delay(250);
  }
  if (digitalRead(btnLower) == LOW) {
    if (mode == RADIO) {
      radioDisplayed++;
      if (radioDisplayed == 4) {
        radioDisplayed = 0;
      }
      setHeader(radioHeaders[radioDisplayed]);
    }
    delay(150);
  }
}

void registeredTouch() {

  t_y = 0 + (320 - t_y);
  if (mode == ALT) {
    if ((t_x > APMasterTouchZone.xStart &&
         t_x < APMasterTouchZone.xStart + APMasterTouchZone.radius) &&
        (t_y > APMasterTouchZone.yStart &&
         t_y < APMasterTouchZone.yStart + APMasterTouchZone.radius)) {
      conn.send(APMasterTouchZone.command);
    }
  }
  if (mode == RADIO || mode == G1000) {

    for (auto &button : buttonArray) {
      if ((t_x > button.xStart && t_x < button.xStart + button.width) &&
          (t_y > button.yStart && t_y < button.yStart + button.height)) {
        conn.send(button.command);
      }
    }
  }

  if (mode == G1000) {
    for (auto &button : g1000Array) {
      if ((t_x > button.xStart && t_x < button.xStart + button.width) &&
          (t_y > button.yStart && t_y < button.yStart + button.height)) {
        conn.send(button.command);
      }
    }
  }
  delay(200);
}

byte oldLMain;
byte oldLAux;
byte oldRMain;
byte oldRAux;

void initFuelScreen() {
  tft.fillScreen(TFT_BLACK);

  byte bars = 4;
  byte width = 20;

  int height = (int)(0.6 * screenHeight);
  byte yBar = (screenHeight - height) / 2;
  int barMargin = (screenWidth - width * bars) / (bars + 1);

  for (int i = 0; i < bars; i++) {
    barContainer[i] = {barMargin + (width * i) + (barMargin * i), yBar, width,
                       height, static_cast<int16_t>(TFT_WHITE)};

    drawFillSquare(barContainer[i]);
  }
}
void percentageBar(float percentageToSet, square parent) {

  int bar = (int)percentageToSet * parent.height;
  square value = {parent.xStart, parent.yStart + (parent.height - bar),
                  parent.width, bar, static_cast<int16_t>(TFT_RED)};
  square emptyBar = {parent.xStart, parent.yStart, parent.width,
                     parent.height - bar, static_cast<int16_t>(TFT_WHITE)};
  drawFillSquare(emptyBar);
  drawFillSquare(value);
}
void fuelMode() {
  byte leftMainFuel = conn.getFuelTankLeftMainLevel();
  byte leftAuxFuel = conn.getFuelTankLeftAuxLevel();
  byte rightMainFuel = conn.getFuelTankRightMainLevel();
  byte rightAuxFuel = conn.getFuelTankRightAuxLevel();
  if (!fuelDrawn) {
    resetDrawBooleans();
    initFuelScreen();
    fuelDrawn = true;

    percentageBar((float)(leftAuxFuel / 100.0), barContainer[0]);
    oldLAux = leftAuxFuel;

    percentageBar((float)(leftMainFuel / 100.0), barContainer[1]);
    oldLMain = leftMainFuel;

    percentageBar((float)(rightMainFuel / 100.0), barContainer[2]);
    oldRMain = rightMainFuel;

    percentageBar((float)(rightAuxFuel / 100.0), barContainer[3]);
    oldRAux = rightAuxFuel;
  }

  if (oldLAux != leftAuxFuel) {
    percentageBar((float)(leftAuxFuel / 100.0), barContainer[0]);
    oldLAux = leftAuxFuel;
  }
  if (oldLMain != leftMainFuel) {
    percentageBar((float)(leftMainFuel / 100.0), barContainer[1]);
    oldLMain = leftMainFuel;
  }
  if (oldRMain != rightMainFuel) {
    percentageBar((float)(rightMainFuel / 100.0), barContainer[2]);
    oldRMain = rightMainFuel;
  }
  if (oldRAux != rightAuxFuel) {
    percentageBar((float)(rightAuxFuel / 100.0), barContainer[3]);
    oldRAux = rightAuxFuel;
  }
}

void touchCheck() {
  bool pressed = tft.getTouch(&t_x, &t_y);
  if (pressed) {
    registeredTouch();
  }
}

void setup() {
  Serial.begin(115200);
  mode = ALT;
  tft.setRotation(1);
  tft.init();
  encoderLower.attachHalfQuad(14, 27);
  encoderOuter.attachHalfQuad(5, 26);
  encoderInner.attachHalfQuad(25, 33);
  pinMode(14, INPUT_PULLUP);
  pinMode(27, INPUT_PULLUP);

  pinMode(btnDual, INPUT_PULLUP);
  pinMode(btnLower, INPUT_PULLUP);

  initializeRadio();
}

void initAltMode() {
  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(&FreeSansBold9pt7b);
  drawAPIndicator(tft, APRow(APTargetSelected));

  square leftSquare = {APFirstColXCoords - 35, 25, 150, screenHeight - 50,
                       static_cast<int16_t>(TFT_WHITE)};

  drawFillSquare(leftSquare);

  square leftSquareInner = {APFirstColXCoords - 33, 25 + 2, 150 - 4,
                            screenHeight - 50 - 4,
                            static_cast<int16_t>(TFT_BLACK)};

  drawFillSquare(leftSquareInner);

  square rightSquare = {APSecondColXCoords - 33, 25, 150, screenHeight - 50,
                        static_cast<int16_t>(TFT_WHITE)};

  drawFillSquare(rightSquare);

  square rightSquareInner = {APSecondColXCoords - 31, 25 + 2, 150 - 4,
                             screenHeight - 50 - 4,
                             static_cast<int16_t>(TFT_BLACK)};

  drawFillSquare(rightSquareInner);

  tft.drawString("INDICATED", APFirstColXCoords, 25 - 5);
  tft.drawString("TARGET", APSecondColXCoords, 25 - 5);
  tft.setFreeFont(&FreeSansBold12pt7b);

  drawAPFields(tft, conn);

  tft.setTextColor(TFT_WHITE, static_cast<int16_t>(TFT_WHITE));
  tft.drawString("AP", 38, tft.getViewportHeight() / 2 + 50);

  tft.drawNumber(conn.getIndicatedAltitude(), APFirstColXCoords,
                 APSecondRowYCoords);
  tft.drawNumber(conn.getApAltLock(), APSecondColXCoords, APSecondRowYCoords);
  tft.drawNumber(conn.getIndicatedHeading(), APFirstColXCoords,
                 APFourthRowYCoords);
  tft.drawNumber(conn.getApHeadingLock(), APSecondColXCoords,
                 APFourthRowYCoords);
  tft.drawNumber(conn.getTrueVerticalSpeed(), APFirstColXCoords,
                 APSixthRowYCoords);
  tft.drawNumber(conn.getApVerticalSpeed(), APSecondColXCoords,
                 APSixthRowYCoords);
}

int oldAlt = 0;
int oldHDG = 0;
int oldVS = 0;
int oldAltLock = 0;
int oldHDGLock = 0;
int oldVSLock = 0;

void altMode() {
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  if (!altDrawn) {
    initAltMode();
    resetDrawBooleans();
    altDrawn = true;
  }
  int curAlt = conn.getIndicatedAltitude();
  if (oldAlt != curAlt) {
    tft.drawString("           ", APFirstColXCoords, APSecondRowYCoords);
    tft.drawNumber(curAlt, APFirstColXCoords, APSecondRowYCoords);
    oldAlt = curAlt;
  }
  int curHDG = conn.getIndicatedHeading();
  if (oldHDG != curHDG) {
    tft.drawNumber(curHDG, APFirstColXCoords, APFourthRowYCoords);
    oldHDG = curHDG;
  }
  int curVS = conn.getTrueVerticalSpeed();
  if (oldVS != curVS) {
    tft.drawNumber(curVS, APSecondColXCoords, APSixthRowYCoords);
    oldVS = curVS;
  }
  int curAltLock = conn.getApAltLock();
  if (oldAltLock != curAltLock) {
    tft.drawString("           ", APSecondColXCoords, APSecondRowYCoords);
    tft.drawNumber(curAltLock, APSecondColXCoords, APSecondRowYCoords);
    oldAltLock = curAltLock;
  }
  int curHDGLock = conn.getApHeadingLock();
  if (oldHDGLock != curHDGLock) {
    tft.drawNumber(curHDGLock, APSecondColXCoords, APFourthRowYCoords);
    oldHDGLock = curHDGLock;
  }
  int curVSLock = conn.getApVerticalSpeed();
  if (oldVSLock != curVSLock) {
    tft.drawString("           ", APSecondColXCoords, APSixthRowYCoords);
    tft.drawNumber(curVSLock, APSecondColXCoords, APSixthRowYCoords);
    oldVSLock = curVSLock;
  }
}

void loop() {
  conn.dataHandling();
  checkEncoders();
  touchCheck();
  poll_fields(conn, MODES(mode), tft);

  switch (mode) {
  case RADIO:
    radioMode();
    break;
  case FUEL:
    fuelMode();
    break;
  case G1000:
    g1000Mode();
    break;
  case ALT:
    altMode();
    break;
  default:
    radioMode();
    break;
  }
}
