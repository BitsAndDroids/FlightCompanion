
#include "BitsAndDroidsFlight32.h"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
BitsAndDroidsFlight32::BitsAndDroidsFlight32() {
    this->serial = &Serial;
}

int BitsAndDroidsFlight32::smoothPot(byte potPin) {
    int readings[samples] = {};
    total = 0;
    for (int &reading : readings) {
        total = total - reading;
        reading = analogRead(potPin);
        total = total + reading;
        delay(1);
    }
    average = total / samples;
    return average;
}

void
BitsAndDroidsFlight32::sendSetYokeAxis(byte elevatorPin, bool reversedElevator, byte aileronPin, bool reversedAileron,
                                       int minVal, int maxVal) {

    elevator = smoothPot(elevatorPin);
    if (reversedElevator) {
        elevator = 1023 - elevator;
    }

    aileron = smoothPot(aileronPin);
    if (reversedAileron) {
        aileron = 1023 - aileron;
    }

    if (elevator != oldElevator || oldAileron != aileron) {
        packagedData = sprintf(valuesBuffer, "%s %i %i", "103", elevator,
                               aileron);
        oldElevator = elevator;
        oldAileron = aileron;
        this->serial->println(valuesBuffer);
    }


}

void BitsAndDroidsFlight32::sendCombinedThrottleValues() {
    packagedData = sprintf(valuesBuffer, "%s %i %i %i %i", "199", engines[0],
                           engines[1], engines[2], engines[3]);
    this->serial->println(valuesBuffer);
}

void BitsAndDroidsFlight32::sendCombinedPropValues() {
    packagedData = sprintf(valuesBuffer, "%s %i %i", "198", props[0], props[1]);
    this->serial->println(valuesBuffer);
}

void BitsAndDroidsFlight32::sendCombinedMixtureValues() {
    packagedData = sprintf(valuesBuffer, "%s %i %i", "115", mixturePercentage[0], mixturePercentage[1]);
    this->serial->println(valuesBuffer);
}

void BitsAndDroidsFlight32::sendFlaps() {
    packagedData = sprintf(valuesBuffer, "%s %i", "421", flaps);
    this->serial->println(valuesBuffer);
}

void BitsAndDroidsFlight32::sendSetElevatorTrimPot(byte potPin, int minVal, int maxVal) {
    currentTrim = (EMA_a * analogRead(potPin)) + ((1 - EMA_a) * currentTrim);
    if (currentTrim != oldTrim) {
        int trimFormatted = calculateAxis(currentTrim, 0, 1023);
        packagedData = sprintf(valuesBuffer, "%s %i", "900", trimFormatted);
        oldTrim = currentTrim;
        this->serial->println(valuesBuffer);
    }
}

void BitsAndDroidsFlight32::sendSetBrakePot(byte leftPin, byte rightPin, int minVal, int maxVal) {
    currentLeftBrake = (EMA_a * analogRead(leftPin)) + ((1 - EMA_a) * currentLeftBrake);
    currentRightBrake = (EMA_a * analogRead(rightPin)) + ((1 - EMA_a) * currentRightBrake);

    bool changed = false;
    if (oldLeftBrake != currentLeftBrake) {

        leftBrakeFormated = calculateAxis(currentLeftBrake, minVal, maxVal);
        oldLeftBrake = currentLeftBrake;
        changed = true;
    }
    if (oldRightBrake != currentRightBrake) {

        rightBrakeFormated = calculateAxis(currentRightBrake, minVal, maxVal);
        oldRightBrake = currentRightBrake;
        changed = true;
    }


    if (changed) {
        packagedData = sprintf(valuesBuffer, "%s %i %i", "902", leftBrakeFormated, rightBrakeFormated);
        this->serial->println(valuesBuffer);
        delay(50);
    }
}

void BitsAndDroidsFlight32::sendSetRudderPot(byte potPin) {
    currentRudder = smoothPot(potPin);
    if (currentRudder != oldRudderAxis) {
        packagedData = sprintf(valuesBuffer, "%s %i", "901", currentRudder);
        oldRudderAxis = currentRudder;
        this->serial->println(valuesBuffer);
    }

}

int BitsAndDroidsFlight32::calculateAxis(int value, int minVal, int maxVal) {
    return map(value, minVal, maxVal, -16383, 16383);
}

void BitsAndDroidsFlight32::sendSetElevatorTrim(int value) {
    packagedData = sprintf(valuesBuffer, "%s %i", "900", value);
    this->serial->println(valuesBuffer);
}


bool convBool(String input) {
    if (input == "0") {
        return false;
    } else {
        return true;
    }
}

void BitsAndDroidsFlight32::dataHandling() {

    if (this->serial->available() > 0) {
        receivedValue = this->serial->readStringUntil('\n');
        switchHandling();
    }

}

void BitsAndDroidsFlight32::switchHandling() {

    prefix = receivedValue.substring(0, 4);
    cutValue = receivedValue.substring(4);
    int prefixVal = prefix.toInt();
    lastPrefix = prefixVal;

    switch (prefixVal) {
        case 9001:{
            testInt = cutValue.toInt();
            break;
        }
        case 9002:{
            testFloat = cutValue.toFloat();
            break;
        }
        case 9003:{
            testBool = convBool(cutValue);
            break;
        }


        // Ap
        case 6010:{
            lGreenPress = convBool(cutValue);
            break;
        }
        case 6011:{
            lBluePress = convBool(cutValue);
            break;
        }
        case 6012:{
            lightTestConcorde = convBool(cutValue);
            break;
        }
        case 5901: {
            curAltC = cutValue.toInt();
            break;
        }
        case 9201:{
            eltState = cutValue.toInt();
            break;
        }
    case 7001:{
        fuelQuantity = cutValue.toFloat();
        break;
    }
    case 7002:{
        fuelUsed = cutValue.toFloat();
        break;
       }
    case 7003:{
        fuelFlowEng1 = cutValue.toFloat();
        break;
    }
    case 7004:{
        fuelFlowEng2 = cutValue.toFloat();
        break;
    }
        case 7008:{
            rpmEng1 = cutValue.toInt();
            break;
        }
        case 7009:{
            rpmEng2 = cutValue.toInt();
            break;
        }
        // lights
        case 133: {
            lightTaxiOn = convBool(cutValue);
            break;
        }
        case 134: {
            lightStrobeOn = convBool(cutValue);
            break;
        }

        case 135: {
            lightPanelOn = convBool(cutValue);
            break;
        }
        case 136: {
            lightRecognitionOn = convBool(cutValue);
            break;
        }
        case 137: {
            lightWingOn = convBool(cutValue);
            break;
        }
        case 138: {
            lightLogoOn = convBool(cutValue);
            break;
        }
        case 139: {
            lightCabinOn = convBool(cutValue);
            break;
        }
        case 140: {
            lightHeadOn = convBool(cutValue);
            break;
        }
        case 141: {
            lightBrakeOn = convBool(cutValue);
            break;
        }
        case 142: {
            lightNavOn = convBool(cutValue);
            break;
        }
        case 143: {
            lightBeaconOn = convBool(cutValue);
            break;
        }
        case 144: {
            lightLandingOn = convBool(cutValue);
            break;
        }
        case 275: {
            fuelTotalPercentage = cutValue.toInt();
            break;
        }
        case 312: {
            feetAboveGround = cutValue.toInt();
            break;
        }
        case 323: {
            onGround = convBool(cutValue);
            break;
        }

            // warnings
        case 333: {
            stallWarning = convBool(cutValue);
            break;
        }
        case 334: {
            overspeedWarning = convBool(cutValue);
            break;
        }
            //GPS
        case 454: {
            gpsCourseToSteer = cutValue.toInt();
        }

            // Flaps
        case 510: {
            flapsHandlePct = cutValue.toInt();
            break;
        }
        case 511: {
            flapsHandleIndex = cutValue.toInt();
            break;
        }
        case 512: {
            flapsNumHandlePos = cutValue.toInt();
            break;
        }
        case 513: {
            trailingEdgeFlapsLeftPercent = cutValue.toInt();
            break;
        }
        case 514: {
            trailingEdgeFlapsRightPercent = cutValue.toInt();
            break;
        }
        case 515: {
            trailingEdgeFlapsLeftAngle = cutValue.toInt();
            break;
        }
        case 516: {
            trailingEdgeFlapsRightAngle = cutValue.toInt();
            break;
        }
        case 517: {
            leadingEdgeFlapsLeftPct = cutValue.toInt();
            break;
        }
        case 518: {
            leadingEdgeFlapsRightPct = cutValue.toInt();
            break;
        }
        case 519: {
            leadingEdgeFlapsLeftAngle = cutValue.toInt();
            break;
        }
        case 520: {
            leadingEdgeFlapsRightAngle = cutValue.toInt();
            break;
        }

            // Gears
        case 526: {
            gearHandlePos = convBool(cutValue);
            break;
        }
        case 527: {
            gearHydraulicPressure = cutValue.toInt();
            break;
        }
        case 528: {
            tailWheelLock = convBool(cutValue);
            break;
        }
        case 529: {
            gearCenterPositionPct = cutValue.toInt();
            break;
        }
        case 530: {
            gearLeftPositionPct = cutValue.toInt();
            break;
        }
        case 531: {
            gearRightPositionPct = cutValue.toInt();
            break;
        }
        case 532: {
            gearTailPositionPct = cutValue.toInt();
            break;
        }
        case 533: {
            gearAuxPosition = cutValue.toInt();
            break;
        }
        case 536: {
            gearTotalPct = cutValue.toInt();
            break;
        }

            // AP
        case 576: {
            APAvailable = convBool(cutValue);
            break;
        }
        case 577: {
            APMasterOn = convBool(cutValue);
            break;
        }
        case 579: {
            APWingLevelerOn = convBool(cutValue);
            break;
        }
        case 580: {
            APNav1LockOn = convBool(cutValue);
            break;
        }
        case 581: {
            APHeadingLockOn = convBool(cutValue);
            break;
        }
        case 583: {
            APAltitudeLockOn = convBool(cutValue);
            break;
        }

        case 585: {
            APAttitudeLockOn = convBool(cutValue);
            break;
        }
        case 586: {
            APGlideslopeHoldOn = convBool(cutValue);
            break;
        }
        case 588: {
            APApproachHoldOn = convBool(cutValue);
            break;
        }
        case 589: {
            APBackcourseHoldOn = convBool(cutValue);
            break;
        }
        case 591: {
            APFlightDirectorOn = convBool(cutValue);
            break;
        }
        case 594: {
            APAirspeedHoldOn = convBool(cutValue);
            break;
        }
        case 596: {
            APMachHoldOn = convBool(cutValue);
            break;
        }
        case 598: {
            APYawDampenerOn = convBool(cutValue);
            break;
        }
        case 600: {
            APAutothrottleArm = convBool(cutValue);
            break;
        }
        case 601: {
            APTakeoffPowerOn = convBool(cutValue);
            break;
        }
        case 602: {
            APAutothrottleOn = convBool(cutValue);
            break;
        }
        case 604: {
            APVerticalHoldOn = convBool(cutValue);
            break;
        }
        case 605: {
            APRPMHoldOn = convBool(cutValue);
            break;
        }

            // Rudder trim
        case 498: {
            elevatorTrimPos = cutValue.toInt();
            break;
        }
        case 500: {
            elevatorTrimPct = cutValue.toInt();
            break;
        }
        case 562: {
            aileronTrimDegr = cutValue.toInt();
            break;
        }
        case 563: {
            aileronTrimPct = cutValue.toInt();
            break;
        }
        case 566: {
            rudderTrimDegr = cutValue.toInt();
            break;
        }
        case 567: {
            rudderTrimPct = cutValue.toInt();
            break;
        }

        case 330: {
            trueVerticalSpeed = cutValue.toInt();
            break;
        }

        case 326: {
            indicatedAirspeed = cutValue.toInt();
            break;
        }
        case 335: {
            indicatedAltitude = cutValue.toInt();
            break;
        }

        case 337: {
            kohlmanAltimeter = cutValue.toInt();
            break;
        }
        case 344: {
            indicatedHeading = cutValue.toInt();
            break;
        }
        case 430: {
            indicatedGPSGroundspeed = cutValue.toInt();
            break;
        }
        case 582: {
            apHeadingLock = cutValue.toInt();
            break;
        }
        case 584: {
            apAltLock = cutValue.toInt();
            break;
        }
        case 590: {
            apVerticalSpeed = cutValue.toInt();
            break;
        }
        case 632: {
            barPressure = cutValue.toInt();
            break;
        }
        case 900: {
            activeCom1 = cutValue.toInt();
            break;
        }
        case 901: {
            standByCom1 = cutValue.toInt();
            break;
        }
        case 902: {
            activeCom2 = cutValue.toInt();
            break;
        }
        case 903: {
            standByCom2 = cutValue.toInt();
            break;
        }
        case 910: {
            activeNav1 = cutValue.toInt();
            break;
        }
        case 911: {
            standbyNav1 = cutValue.toInt();
            break;
        }
        case 912: {
            activeNav2 = cutValue.toInt();
            break;
        }
        case 913: {
            standbyNav2 = cutValue.toInt();
            break;
        }
        case 914: {
            navRadialError1 = cutValue.toInt();
            break;
        }
        case 915: {
            navVorLationalt1 = cutValue.toInt();
            break;
        }
            // DME
        case 950: {
            navDme1 = cutValue;
            break;
        }
        case 951: {
            navDme2 = cutValue;
            break;
        }
        case 952: {
            navDmeSpeed1 = cutValue;
            break;
        }
        case 953: {
            navDmeSpeed2 = cutValue;
            break;
        }

            // ADF
        case 954: {
            adfActiveFreq1 = cutValue;
            break;
        }
        case 955: {
            adfStandbyFreq1 = cutValue;
            break;
        }
        case 956: {
            adfRadial1 = cutValue;
            break;
        }
        case 957: {
            adfSignal1 = cutValue;
            break;
        }
        case 958: {
            adfActiveFreq2 = convertToNavFreq(cutValue);
            break;
        }
        case 959: {
            adfStandbyFreq2 = convertToNavFreq(cutValue);
            break;
        }
        case 960: {
            adfRadial2 = cutValue;
            break;
        }
        case 961: {
            adfSignal2 = cutValue;
            break;
        }

            // Transponder
        case 962: {
            transponderCode1 = cutValue;
            break;
        }
        case 963: {
            transponderCode2 = cutValue;
            break;
        }

            // PLANE DATA
        case 999: {
            planeName = cutValue;
            break;
        }

        case 606: {
            navObs1 = cutValue.toInt();
            break;
        }
        case 607: {
            navObs2 = cutValue.toInt();
            break;
        }
        case 234: {
            fuelTankCenterLevel = cutValue.toInt();
            break;
        }
        case 235: {
            fuelTankCenter2Level = cutValue.toInt();
            break;
        }

        case 236: {
            fuelTankCenter3Level = cutValue.toInt();
            break;
        }

        case 237: {
            fuelTankLeftMainLevel = cutValue.toInt();
            break;
        }

        case 238: {
            fuelTankLeftAuxLevel = cutValue.toInt();
            break;
        }

        case 239: {
            fuelTankLeftTipLevel = cutValue.toInt();
            break;
        }

        case 240: {
            fuelTankRightMainLevel = cutValue.toInt();
            break;
        }

        case 241: {
            fuelTankRightAuxLevel = cutValue.toInt();
            break;
        }

        case 242: {
            fuelTankRightTipLevel = cutValue.toInt();
            break;
        }

        case 243: {
            fuelTankExternal1Level = cutValue.toInt();
            break;
        }

        case 244: {
            fuelTankExternal2Level = cutValue.toInt();
            break;
        }

        case 245: {
            fuelTankCenterCapacity = cutValue.toInt();
            break;
        }

        case 246: {
            fuelTankCenter2Capacity = cutValue.toInt();
            break;
        }

        case 247: {
            fuelTankCenter3Capacity = cutValue.toInt();
            break;
        }

        case 248: {
            fuelTankLeftMainCapacity = cutValue.toInt();
            break;
        }

        case 249: {
            fuelTankLeftAuxCapacity = cutValue.toInt();
            break;
        }

        case 250: {
            fuelTankLeftTipCapacity = cutValue.toInt();
            break;
        }

        case 251: {
            fuelTankRightMainCapacity = cutValue.toInt();
            break;
        }

        case 252: {
            fuelTankRightAuxCapacity = cutValue.toInt();
            break;
        }

        case 253: {
            fuelTankRightTipCapacity = cutValue.toInt();
            break;
        }

        case 254: {
            fuelTankExternal1Capacity = cutValue.toInt();
            break;
        }

        case 255: {
            fuelTankExternal2Capacity = cutValue.toInt();
            break;
        }

        case 256: {
            fuelTankLeftCapacity = cutValue.toFloat();
            break;
        }
        case 257: {
            fuelTankRightCapacity = cutValue.toFloat();
            break;
        }
        case 258: {
            fuelTankCenterQuantity = cutValue.toInt();
            break;
        }
        case 259: {
            fuelTankCenter2Quantity = cutValue.toInt();
            break;
        }
        case 260: {
            fuelTankCenter3Quantity = cutValue.toInt();
            break;
        }
        case 261: {
            fuelTankLeftMainQuantity = cutValue.toInt();
            break;
        }

        case 262: {
            fuelTankLeftAuxQuantity = cutValue.toInt();
            break;
        }

        case 263: {
            fuelTankLeftTipQuantity = cutValue.toInt();
            break;
        }

        case 264: {
            fuelTankRightMainQuantity = cutValue.toInt();
            break;
        }

        case 265: {
            fuelTankRightAuxCapacity = cutValue.toInt();
            break;
        }

        case 266: {
            fuelTankRightTipQuantity = cutValue.toInt();
            break;
        }

        case 267: {
            fuelTankExternal1Quantity = cutValue.toInt();
            break;
        }
        case 268: {
            fuelTankExternal2Quantity = cutValue.toInt();
            break;
        }

        case 269: {
            fuelTankLeftQuantity = cutValue.toFloat();
            break;
        }
        case 270: {
            fuelTankRightQuantity = cutValue.toFloat();
            break;
        }
        case 271: {
            fuelTankTotalQuantity = cutValue.toInt();
            break;
        }
        case 505: {
            parkingBrakeIndicator = convBool(cutValue);
            break;
        }
        case 1000: {
            masterCautionOn = convBool(cutValue);
            break;
        }
        case 1001: {
            masterWarningOn = convBool(cutValue);
            break;
        }


        default:
            break;
    }

}


void BitsAndDroidsFlight32::propsInputHandling(int propPin1,
                                               int propPin2) {
    bool changed = false;
    propValue1 = smoothPot(propPin1);
    propValue2 = smoothPot(propPin2);
    if (propValue1 != oldPropValue1 || propValue2 != oldPropValue2) {


        if (abs(propValue1 - oldPropValue1) > 2) {
            props[0] = propValue1;
            oldPropValue1 = propValue1;
            changed = true;
        }
        if (abs(propValue2 - oldPropValue2) > 2) {
            props[1] = propValue2;
            oldPropValue2 = propValue2;
            changed = true;
        }
        if (changed) {
            sendCombinedPropValues();
        }
    }
}

void BitsAndDroidsFlight32::mixtureInputHandling(int mixturePin1,
                                                 int mixturePin2) {
    bool changed = false;
    mixtureValue1 = smoothPot(mixturePin1);
    mixtureValue2 = smoothPot(mixturePin2);
    if (mixtureValue1 != oldMixtureValue1 || mixtureValue2 != oldMixtureValue2) {


        if (abs(mixtureValue1 - oldMixtureValue1) > 2) {
            mixturePercentage[0] = mixtureValue1;
            oldMixtureValue1 = mixtureValue1;
            changed = true;
        }
        if (abs(mixtureValue2 - oldMixtureValue2) > 2) {
            mixturePercentage[1] = mixtureValue2;
            oldMixtureValue2 = mixtureValue2;
            changed = true;
        }
        if (changed) {
            sendCombinedMixtureValues();
        }
    }


}

void BitsAndDroidsFlight32::simpleInputHandling(int throttlePin) {
    value = smoothPot(throttlePin);

    if (value != oldValue && abs(oldValue - value) > 1) {
        oldValue = value;

        engines[0] = value;
        engines[1] = value;
        engines[2] = value;
        engines[3] = value;

        sendCombinedThrottleValues();
    }
}

void BitsAndDroidsFlight32::setPotFlaps(byte flapsPin) {
    flaps = smoothPot(flapsPin);
    if (flaps != oldFlaps && abs(oldFlaps - flaps) > 2) {
        oldFlaps = flaps;
        sendFlaps();
    }
}

void BitsAndDroidsFlight32::advancedInputHandling(
        int eng1Pin, int eng2Pin, int eng3Pin, int eng4Pin) {
    valueEng1 = smoothPot(eng1Pin);
    valueEng2 = smoothPot(eng2Pin);
    valueEng3 = smoothPot(eng3Pin);
    valueEng4 = smoothPot(eng4Pin);
    bool changed = false;

    if (valueEng1 != oldValueEng1) {
        oldValueEng1 = valueEng1;
        engines[0] = valueEng1;
        changed = true;
    }
    if (valueEng2 != oldValueEng2) {
        oldValueEng2 = valueEng2;
        engines[1] = valueEng2;
        changed = true;
    }
    if (valueEng3 != oldValueEng3) {
        oldValueEng3 = valueEng3;
        engines[2] = valueEng3;
        changed = true;
    }
    if (valueEng4 != oldValueEng4) {
        oldValueEng4 = valueEng4;
        engines[3] = valueEng4;
        changed = true;
    }

    if (changed) {
        sendCombinedThrottleValues();
    }
}

void BitsAndDroidsFlight32::superAdvancedInputHandling(
        byte eng1Percentage, byte eng2Percentage, byte eng3Percentage,
        byte eng4Percentage) {
    engines[0] = eng1Percentage;
    engines[1] = eng2Percentage;
    engines[2] = eng3Percentage;
    engines[3] = eng4Percentage;
    sendCombinedThrottleValues();
}

String BitsAndDroidsFlight32::convertToFreq(const String& unprocFreq) {
    String stringA = unprocFreq.substring(0, 3);
    String stringB = unprocFreq.substring(3);
    return stringA + "." + stringB;
}

String BitsAndDroidsFlight32::convertToNavFreq(const String& unprocFreq) {
    String stringA = unprocFreq.substring(0, 3);
    String stringB = unprocFreq.substring(3, 5);
    return stringA + "." + stringB;
}

int BitsAndDroidsFlight32::getLastPrefix() const { return lastPrefix; }

// Set jitter algorithm EMA_a
void BitsAndDroidsFlight32::setEMA_a(float a) { EMA_a = a; }

void BitsAndDroidsFlight32::send(int command) {
    Serial.println(command);
}
// RECEIVING VALUES
// GPS



//----------------------
// TRANSMIT FUNCTIONS
// AP







#pragma clang diagnostic pop
