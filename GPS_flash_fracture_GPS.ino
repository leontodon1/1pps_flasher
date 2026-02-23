// 01.02.2026, Dr. Björn Kattentidt
// Arduino NANO
// programming: Arduino Nano, ATmega238P (boot loader ATmega328P) at COM5
// Purpose: A GPS LED flasher at telescope's optical entry for time stamps
// The timing is given by a GPS 1PPS signal. This is feeding an output (SigOut) by feeding through only a fracture A + 10 x B of impulses in normal modes
// Two Hexadecimal or BCD switches A (SelHexA?)and B (SelHexB?) set an input value from 0 to 15 (4 bit each)
// The values A and B are the number A + 10 x B and after the duration in seconds the output (SigOut) is activated for 100ms
// 260201 v61: extension 4 LUTs with omitted flash before 0, internal LED blink debugged
// 260125 v60: changed pin setting of lines! 2k2 in line from PCB to GPS PPS, port commands removed, change to AltSoftSerial, several bugs, FUNCTIONS CHECKED OK !! <250µsec
// 260122 v50_chg1: old pin setting intended for production
// 260119 v50: rearrangement of special function order, voids new inside loo p => v50_prod
// 250112 v49: LUT05 added and LUTs with 0s not blinking, bootloader delay, digital.write to portB.bitx, sek05, function shift before PPS trigger,
//             print signals for testing/debugging, GPS alive check with LED signalling, GPS time debug
// 250110 v48: BCD switches A and B exchanged, UBATLow and UBatCut defined, variable names optimzed (LC)
// 251218 v47: tested!: battery survaillance; under voltage protection, low battery indicator
// 251202 v46: LCspeFuDE and LCspeFuDD
// 251127 v45: battery survaillance; under voltage protection, low battery indicator
// 251125 v44: BCD and HEX switches merges, back to polling - no interrupts, speed optimization: LUTs, precompute BCD in loo p, parameters for different hardware
// chatGPT: best Case (GPS valid, LUT direct) < 20 µs; worst Case (GPS invalid, all bool calulation) ~50–60 µs
// 251121 v43: A5 as output for battery cutoff
// 251121 v43: low battery indicator for internal LED #to be tested# (ok)
// 251119 v42: under voltage protection #to be tested# (ok)
// 251119 v41: special functions and code optimization (manual)
// 251015 v40: read GPS sequential
// 251015 v31: cleanup "hex" to "Hex" as well as max numbers from 15 to 9
// 250908 v30: Feature: started: Power down to protect LiFePo4 against under voltage at low battery measured at analogue pin (removed)
// 250908 v21: Feature: At "00" the flash is strong elongated for purpose to calibrate LED illumination by potentiometer, do not use for flashing

// NMEA data on D8
#include <AltSoftSerial.h>
#include <TinyGPS++.h>

// RX=D8, TX=D9; GPS PPS (TX) on Pin D8 (RX), (-1: TX not mandatory)
AltSoftSerial gpsSerial;  // AltSoftSerial object for GPS
TinyGPSPlus gps;          // TinyGPS++ object
String nmeaLine = "";
bool LCgpsFix = false;
bool LCgpsTimeValid = 0;

//checkit
#define DEBUG_NMEA 1             // 1 = raw data, 0 = text debug
const bool LCserialPrintON = 1;  // !!! 0: main switch to deactivate output to serial monitor for production

// // GPS PPS (TX) on Pin D8 (RX), (-1: TX not mandatory)
// SoftwareSerial gpsSerial(8, -1);  // RX (input nano), TX (not used)
// TinyGPSPlus gps;

// PPS pins
const uint8_t PinInPPS = 6;  // pin for PPS input
const uint8_t SigOut = 7;    // pin for LED output

// BCD switches
const uint8_t SelHexA[4] = { 9, 10, 11, 12 };  // D9, D10, D11, D12 pin for input A selection
const uint8_t SelHexB[4] = { 2, 3, 4, 5 };     // D2, D3, D4, D5 pin    for input B selection
bool LCspeFuAct = 0;                           // special function is active
bool LCspecIntAct = 0;                         // LED interval during special function is active


// GPS alive test
bool LCalive;
const unsigned long timeoutMs = 3000;         // timeout to display that NMEB data is stable valid or invalid
const unsigned long gpsBlinkInterval = 1000;  // flash duration
static unsigned long lastBlink = 0;

// voltage survaillance
#define LED_PIN 13                // internal LED, pin D13
const bool LCLowBatEna = 1;       // SET: enable low battery check
float frequency = 4;              // SET: blink frequency if internal LED at low battery (Hz)
float voltage;                    // battery voltage (V)
float UBatLow = 3.1;              // 3.1V: low battery indicator at 15% SOC
float UBatCut = 2.8;              // 2.8V: final battery cutoff, signal system cutoff at <5% SOC
float halfPeriod;                 // calculate half period for internal LED blinking
const float refVoltage = 5.0;     // reference voltage (standard: 5.0V at UNO/Nano, sometimes 3.3V)
const uint8_t cutoffPinOut = A5;  // A5 as digital output to cutoff battery
bool LCBatCut;                    // battery cutoff reached
const uint8_t UBatanaIn = A7;     // Pin for voltage measurement
int UBatdigVal;                   // battery voltage digital value (0...1023)
unsigned long TprevBat;           // time marker
unsigned long TcurBat;            // current LED time
bool LCledState = LOW;            // current LED state
bool LCTrigBatCheck;              // do battery check
bool LClowBatActive;              // low battery
bool LCblinkIntLED;               // blink internal LED

// timing variables
const unsigned long ONdurLight = 800;       // SET: default long flash duration for illumination control
const unsigned long ONdurNorm = 40;         // SET: default short flash duration for time measurement
float TNMEAhalf = 1000;                     // T for change internal LED state for no NMEA data
unsigned long ONduration;                   // flash duration
bool LCsecValid = 0;                        // GPS second is valid
uint8_t Sekunde;                            // second received from GPS
uint8_t SekundeOld;                         // old Sekunde
uint8_t sek05;                              // corrected second from GPS shifted by +0,5
unsigned long TGPSRx;                       // time second from GPS (millis)
unsigned long TRMC;                         // time of last $GNRMC validation
unsigned long TRMCold;                      // old TRMC
int loopsPsec = 0;                          // loops per second
bool LCgpsStable = 0;                       // time is reliable
bool LCgpsFresh;                            // GPS signal is fresh, location not older than approx 1,2 sec.
bool LCgpsAlive;                            // GPS signal is alive
bool LCPPSfall;                             // shows during 1 loop that PPS has arrived
const unsigned long TlowBatDlylong = 2500;  // SET: ms long delay after begin GPS PPS signal
const unsigned long TlowBatDly = 500;       // ms delay after begin GPS PPS signal
unsigned long TBatcheck = 0;                // time of last battery check
unsigned long TBatcheckOld;
unsigned long TpulseStart = 0;  // start time for LED pulse duration
uint8_t codeSwiA, codeSwiB;     // manual setting number wheel value, A: right, B: left
int codeSwiBA = 0;              // manual setting number wheels
uint8_t counter = 0;            // second counter
bool LCppsSig = false;          // PPS pin signal
bool LCppsSigOld = false;

// special functions
const bool LC_codeBCD = 1;      // SET: 1: BCD number wheels, 0: Hex number wheels
const bool LCcodeSwiOrder = 0;  // changes order of number wheels installed
bool LCspeFu00 = false;         // special function 00 is active
bool LCspeFuFF = false;         // special function FF is active
bool LCspeFuFE = false;         // special function FE is active
bool LCspeFuFD = false;         // special function FD is active
bool LCspeFuFC = false;         // special function FC is active
bool LCspeFuFB = false;         // special function FB is active
bool LCspeFuFA = false;         // special function FA is active
bool LCspeFuEF = false;         // special function EF is active
bool LCspeFuEE = false;         // special function EE is active
bool LCspeFuED = false;         // special function ED is active
bool LCspeFuEC = false;         // special function EC is active
bool LCspeFuEB = false;         // special function EB is active
bool LCspeFuDF = false;         // special function DF is active
bool LCspeFuDE = false;         // special function DE is active
bool LCspeFuDD = false;         // special function DD is active
bool LCLUTMin_DD = false;       // minute info is active for DD
bool LCval_DD = false;          // minute info is valid  for DD
bool LCspeFuCF = false;         // special function EF is active
bool LCspeFuCE = false;         // special function EE is active
bool LCspeFuCD = false;         // special function ED is active
bool LCspeFuCC = false;         // special function EC is active
bool LCLUT_DF = false;          // LUT DF is active
bool LCLUT_DE = false;          // LUT DE is active
bool LCLUT_DD = false;          // LUT DD is active
bool LCLUT_05s = false;         // LUT 05s is active
bool LCLUT_10s = false;         // LUT 10s is active
bool LCLUT_15s = false;         // LUT 15s is active
bool LCLUT_20s = false;         // LUT 20s is active
bool LCLUT_30s = false;         // LUT 30s is active
bool LCLUT_60s = false;         // LUT 60s is active
bool LCLUT_05s0 = false;        // LUT 05s0 is active
bool LCLUT_10s0 = false;        // LUT 10s0 is active
bool LCLUT_15s0 = false;        // LUT 15s0 is active
bool LCLUT_20s0 = false;        // LUT 20s0 is active
bool LCLUT_30s0 = false;        // LUT 20s0 is active
bool LCLUT_05sv0 = false;       // 05sv0 LUT is active
bool LCLUT_10sv0 = false;       // 10sv0 LUT is active
bool LCLUT_15sv0 = false;       // 15sv0 LUT is active
bool LCLUT_20sv0 = false;       // 20sv0 LUT is active
bool LCparity = 0;              // parity of GPS minute
bool LCminuteBitPos;            // minute bit of bitPos
uint8_t byteIndex;              // Index for LUTs
uint8_t bitIndex;               // Index for LUTs
uint8_t bitPos;                 // bit position for minute reading
uint8_t minuteBits;             // GPS minute (6 bits)
uint8_t i;                      // while parameter

// SET: Lookup-tables for 0–59 seconds
const uint8_t LUT_05s[8] = { 0b00100001, 0b10000100, 0b00010000, 0b01000010, 0b00001000, 0b00100001, 0b10000100, 0b00010000 };    // blink at 0,5,10,15...,50,55
const uint8_t LUT_10s[8] = { 0b00000001, 0b00000100, 0b00010000, 0b01000000, 0b00000000, 0b00000001, 0b00000100, 0b00000000 };    // blink at 0,10,20,30,40,50
const uint8_t LUT_15s[8] = { 0b00000001, 0b10000000, 0b00000000, 0b01000000, 0b00000000, 0b00100000, 0b00000000, 0b00000000 };    // blink at 0,15,30,45
const uint8_t LUT_20s[8] = { 0b00000001, 0b00000000, 0b00010000, 0b00000000, 0b00000000, 0b00000001, 0b00000000, 0b00000000 };    // blink at 0,20,40
const uint8_t LUT_30s[8] = { 0b00000001, 0b00000000, 0b00000000, 0b01000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };    // blink at 0,30
const uint8_t LUT_05s0[8] = { 0b00100000, 0b10000100, 0b00010000, 0b01000010, 0b00001000, 0b00100001, 0b10000100, 0b00010000 };   // LUT_05s with no blink at 0
const uint8_t LUT_10s0[8] = { 0b00000000, 0b00000100, 0b00010000, 0b01000000, 0b00000000, 0b00000001, 0b00000100, 0b00000000 };   // LUT_10s with no blink at 0
const uint8_t LUT_15s0[8] = { 0b00000000, 0b10000000, 0b00000000, 0b01000000, 0b00000000, 0b00100000, 0b00000000, 0b00000000 };   // LUT_15s with no blink at 0
const uint8_t LUT_20s0[8] = { 0b00000000, 0b00000000, 0b00010000, 0b00000000, 0b00000000, 0b00000001, 0b00000000, 0b00000000 };   // LUT_20s with no blink at 0
const uint8_t LUT_30s0[8] = { 0b00000000, 0b00000000, 0b00000000, 0b01000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };   // LUT_30s with no blink at 0
const uint8_t LUT_05sv0[8] = { 0b00100001, 0b10000100, 0b00010000, 0b01000010, 0b00001000, 0b00100001, 0b00000100, 0b00010000 };  // blink at 0,5,10,15...,50, not at 55
const uint8_t LUT_10sv0[8] = { 0b00000001, 0b00000100, 0b00010000, 0b01000000, 0b00000000, 0b00000001, 0b00000000, 0b00000000 };  // blink at 0,10,20,30,40, not at 50
const uint8_t LUT_15sv0[8] = { 0b00000001, 0b10000000, 0b00000000, 0b01000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };  // blink at 0,15,30, not at 45
const uint8_t LUT_20sv0[8] = { 0b00000001, 0b00000000, 0b00010000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };  // blink at 0,20, not at 40

// SET: Here additional LUTS can be added but need to be defined and used in code
// LUT_DF: LED blinks at 0s, 5s, 10s, 20s, 40s
const uint8_t LUT_DF[8] = {
  0b00100001,  // Byte 0: Bits 0–7:   seconds 0 (=1) und 5 (=1)
  0b00000100,  // Byte 1: Bits 8–15:  seconds 10 (=1)
  0b00010000,  // Byte 2: Bits 16–23: seconds 20 (=1)
  0b00000000,  // Byte 3: Bits 24–31
  0b00000000,  // Byte 4: Bits 32–39
  0b00000001,  // Byte 5: Bits 40–47: seconds 40 (=1)
  0b00000000,  // Byte 6: Bits 48–55
  0b00000000   // Byte 7: Bits 56–59
};
// LUT_DE blinks LED by pattern 0, 11, 23, 34, 45 due to intervals 11/12/11/11/15 (similar to Pseudo-Barker-code)
const uint8_t LUT_DE[8] = {
  0b00000001,  // Byte 0: Bits 0–7:   seconds 0
  0b00001000,  // Byte 1: Bits 8–15:  seconds 11
  0b10000000,  // Byte 2: Bits 16–23: seconds 23
  0b00000000,  // Byte 3: Bits 24–31
  0b00000100,  // Byte 4: Bits 32–39: seconds 34
  0b00100000,  // Byte 5: Bits 40–47: seconds 45
  0b00000000,  // Byte 6: Bits 48–55:
  0b00000000   // Byte 7: Bits 56–59: no minutes, no check, no LCparity
};
// LUT_DD blinks LED by pattern 0, 11, 23, 34, 45 due to intervals 11/12/11/11/15 (similar to Pseudo-Barker-code)
// additional functionality: minute coding at 48-53 and doubled at 54-59 and 46, even LCparity: flash + parity -> even sum
const uint8_t LUT_DD[8] = {
  0b00000001,  // Byte 0: Bits 0–7:   seconds 0
  0b00001000,  // Byte 1: Bits 8–15:  seconds 11
  0b10000000,  // Byte 2: Bits 16–23: seconds 23
  0b00000000,  // Byte 3: Bits 24–31
  0b00000100,  // Byte 4: Bits 32–39: seconds 34
  0b00100000,  // Byte 5: Bits 40–47: seconds 45, 46: parity
  0b00000000,  // Byte 6: Bits 48–55: for minute bits
  0b00000000   // Byte 7: Bits 56–59: for minute bits
};

void setup() {
  delay(20);              // bootloader delay, if too long (>350), device will be shut down by deep discharge protection
  Serial.begin(9600);     // Serial Monitor to PC
  gpsSerial.begin(9600);  // GPS-Modul Baud rate (mostly 9600)
  Serial.println("Wait for GPS data...");

  pinMode(PinInPPS, INPUT_PULLUP);   // define GPS input pin
  pinMode(SigOut, OUTPUT);           // define clamp LED output pin
  pinMode(LED_PIN, OUTPUT);          // Pin D13 as output: internal LED indicator (low bat, GPS data invalid)
  digitalWrite(LED_PIN, LOW);        // D13 auf LOW, switch off internal LED
  pinMode(cutoffPinOut, OUTPUT);     // A5 as cutoff output pin, cutoff = 0
  digitalWrite(cutoffPinOut, HIGH);  // battery cutoff OFF
  for (int i = 0; i < 4; i++) {      // define number wheels
    pinMode(SelHexB[i], INPUT_PULLUP);
    pinMode(SelHexA[i], INPUT_PULLUP);
  }
}  // end void setup

void loop() {
  loopsPsec = loopsPsec + 1;
  LCppsSigOld = LCppsSig;
  LCppsSig = (PIND & (1 << 6)) ? 1 : 0;  // D6, (PIND >> 6) & 1, read PPS pin: fast digitalRead(PinInPPS)
  PPStrigger();                          // PPS trigger: at PPS falling edge: pick time, read LUT, evaluate special functions
  ReadGPSComputeSecond();                // read and compute NMEA data
  //checkit
  GPStimeDebug(1, 1);   // debug: (LCLocTimeon, LCSekon) list GPS data and second; 1: active; 0: deactivated
  SwitchOffClampLED();  // switch off SigOut (clamp LED) after elapsed time
  READcodeSwiBA();      // read stationary code wheels, calculate codeSwiBA which is a number (for HEX artificial)
  SELspecFun();         // select special function for BCD and HEX code wheels

  // carry out battery checks, serve low battery indicator
  BattCheck();        // battery check with and w/o valid GPS
  LowBatIndicator();  // low bat indicator: internal LED is blinking with high frequency, battery cutoff flag
  // ShowLUTs();         // for testing!!, 0: for production
  UpdateGpsAlive();  // GPS alive checks, flash LED slow
}  // end void loop

// read and compute GPS data
void ReadGPSComputeSecond() {
  // read GPS data, hand over to TinyGPS++
  while (gpsSerial.available()) {
    char c = gpsSerial.read();
    gps.encode(c);
    if (DEBUG_NMEA) {
      Serial.write(c);  // serial monitor, only for check
    }
    // NMEA line formation
    if (c == '\n') {
      processNMEALine(nmeaLine);
      nmeaLine = "";
    } else if (c != '\r') {
      nmeaLine += c;
    }
  }
  if (millis() - TRMC >= 2500) {  // GNRMC is out of date
    TRMCold = TRMC;
    LCgpsTimeValid = 0;
  }

  LCsecValid = gps.time.isValid();  // only for 1 loop
  if (LCsecValid) {
    Sekunde = gps.time.second();  // 0…59  read GPS second
    // Serial.print("Sekunde: ");    // temporary
    // Serial.println(Sekunde);      // temporary
  }
  if (Sekunde != SekundeOld) {  // at every change of GPS second
    SekundeOld = Sekunde;
    TGPSRx = millis();  // GPS second is valid
  }
}  // end void ReadGPSComputeSecond

void PPStrigger() {
  LCPPSfall = 0;                   // reset after 1 loop
  if (LCppsSigOld && !LCppsSig) {  // falling edge
    Serial.print(loopsPsec);       // for clc of loops per second
    Serial.print(" ");
    Serial.print(1000000 / loopsPsec);  // loop µs
    Serial.println(" µs ");
    loopsPsec = 0;
    TpulseStart = millis();  // start time for LED pulse duration
    LCPPSfall = 1;           // only for 1 loop

    // definition of the time second to be used
    sek05 = (Sekunde + 1) % 60;  // triggering sek05 calculation
    // if (!LCalive) { sek05 = 0; }  // reset time for clamp LED flashing
    if (!LCsecValid || !LCgpsTimeValid) { sek05 = 0; }  // reset time to 0 sec

    //checkit
    if (DEBUG_NMEA) {  // for testing
      Serial.print(" sek05: ");
      Serial.print(sek05);
      Serial.print(" |B ");
      Serial.print(codeSwiB);
      Serial.print(" |A ");
      Serial.print(codeSwiA);
      Serial.print(" |BA ");
      Serial.println(codeSwiBA);
    }

    // duration of flash: 00 = long, else standard for time measurement
    ONduration = (LCspeFu00) ? ONdurLight : ONdurNorm;

    // selection of LUT bit according to sek05
    byteIndex = sek05 / 8;                                                // read at byte 0..7
    bitIndex = sek05 % 8;                                                 // read at bit  0..7
    if (LCspeFuFF) LCLUT_05s = (LUT_05s[byteIndex] >> bitIndex) & 1;      // FF/99: flash every 5 seconds
    if (LCspeFuFE) LCLUT_10s = (LUT_10s[byteIndex] >> bitIndex) & 1;      // FE/98: flash every 10 seconds
    if (LCspeFuFD) LCLUT_15s = (LUT_15s[byteIndex] >> bitIndex) & 1;      // FD/97: flash every 15 seconds
    if (LCspeFuFC) LCLUT_20s = (LUT_20s[byteIndex] >> bitIndex) & 1;      // FC/96: flash every 20 seconds
    if (LCspeFuFB) LCLUT_30s = (LUT_30s[byteIndex] >> bitIndex) & 1;      // FB/95: flash every 30 seconds
    if (LCspeFuFA) LCLUT_60s = (sek05 == 0);                              // FA/94: flash every 60 seconds
    if (LCspeFuEF) LCLUT_05s0 = (LUT_05s0[byteIndex] >> bitIndex) & 1;    // EF/:93 flash every 5 seconds
    if (LCspeFuEE) LCLUT_10s0 = (LUT_10s0[byteIndex] >> bitIndex) & 1;    // EE/92: flash every 10 seconds except 0
    if (LCspeFuED) LCLUT_15s0 = (LUT_15s0[byteIndex] >> bitIndex) & 1;    // ED/91: flash every 15 seconds except 0
    if (LCspeFuEC) LCLUT_20s0 = (LUT_20s0[byteIndex] >> bitIndex) & 1;    // EC/90: flash every 20 seconds except 0
    if (LCspeFuEB) LCLUT_30s0 = (LUT_30s0[byteIndex] >> bitIndex) & 1;    // EB/89: flash every 30 seconds except 0
    if (LCspeFuDF) LCLUT_DF = (LUT_DF[byteIndex] >> bitIndex) & 1;        // DF/88: LUT_DF seconds: 0,5,10,20,40
    if (LCspeFuDE) LCLUT_DE = (LUT_DE[byteIndex] >> bitIndex) & 1;        // DE/87: LUT_DE seconds: 0,11,23,34,45
    if (LCspeFuDD) LCLUT_DD = (LUT_DD[byteIndex] >> bitIndex) & 1;        // DD/86: LUT_DD seconds: 0,11,23,34,45,(46+48–55+56–59)
    if (LCspeFuCF) LCLUT_05sv0 = (LUT_05sv0[byteIndex] >> bitIndex) & 1;  // FF/85: flash every 5 seconds except 0 preceeding
    if (LCspeFuCE) LCLUT_10sv0 = (LUT_10sv0[byteIndex] >> bitIndex) & 1;  // FE/84: flash every 10 seconds except 0 preceeding
    if (LCspeFuCD) LCLUT_15sv0 = (LUT_15sv0[byteIndex] >> bitIndex) & 1;  // FD/83: flash every 15 seconds except 0 preceeding
    if (LCspeFuCC) LCLUT_20sv0 = (LUT_20sv0[byteIndex] >> bitIndex) & 1;  // FC/82: flash every 20 seconds except 0 preceeding

    // calculations for DD/86: minute code DD twice and parity bit at 46-59
    LCparity = 0;
    LCminuteBitPos = 0;                                                         // reset
    if (LCspeFuDD) {                                                            // bit 46: use minute bitcoded and even LCparity if GPS is valid
      if (gps.time.isValid()) minuteBits = gps.time.minute() & 0x3F;            // bits 0–5 of minute
      if (sek05 == 46) {                                                        // parity bit on bit 46
        for (i = 0; i < 6; i++) LCparity ^= (minuteBits >> i) & 1;              // even LCparity across all 6 minute bits + parity: 0 if even, 1 if odd
        digitalWrite(SigOut, LCparity ? HIGH : LOW);                            // write LCparity independent of LUT to clamp LED, pin D7
      }                                                                         // end LCparity bit code
      else if ((sek05 >= 48 && sek05 <= 53) || (sek05 >= 54 && sek05 <= 59)) {  // LCLUT_DD is 0 in this range
        // determine bit position
        bitPos = (sek05 <= 53) ? (sek05 - 48) : (sek05 - 54);              // 0..5 at each start position
        LCminuteBitPos = (minuteBits >> bitPos) & 1;                       // read minuteBits at bit bitPos
        digitalWrite(SigOut, LCminuteBitPos ? HIGH : LOW);                 // minute bit independent of LUT to clamp LED, pin D7
      }                                                                    // end minute bit codes
    }                                                                      // end LCLUT_DD
    LCval_DD = LCspeFuDD && gps.time.isValid();                            // DD/92: minute bitcode active
    LCLUTMin_DD = LCLUT_DD || (LCval_DD && (LCparity || LCminuteBitPos));  // LUT_DD for 0...45, LCval_DD for 48-53, 54-59 and even LCparity at 46

    // special function is active:
    LCspeFuAct = LCspeFu00 || LCspeFuFF || LCspeFuFE || LCspeFuFD || LCspeFuFC || LCspeFuFB || LCspeFuFA
                 || LCspeFuEF || LCspeFuEE || LCspeFuED || LCspeFuEC || LCspeFuEB
                 || LCspeFuDF || LCspeFuDE || LCspeFuDD
                 || LCspeFuCF || LCspeFuCE || LCspeFuCD || LCspeFuCC;

    // conditions to activate the LED:
    LCspecIntAct = LCspeFu00
                   || (LCspeFuFF && LCLUT_05s) || (LCspeFuFE && LCLUT_10s) || (LCspeFuFD && LCLUT_15s)
                   || (LCspeFuFC && LCLUT_20s) || (LCspeFuFB && LCLUT_30s) || (LCspeFuFA && LCLUT_60s)
                   || (LCspeFuEF && LCLUT_05s0) || (LCspeFuEE && LCLUT_10s0) || (LCspeFuED && LCLUT_15s0)
                   || (LCspeFuEC && LCLUT_20s0) || (LCspeFuEB && LCLUT_30s0)
                   || (LCspeFuDF && LCLUT_DF) || (LCspeFuDE && LCLUT_DE) || (LCspeFuDD && LCLUTMin_DD)
                   || (LCspeFuCF && LCLUT_05sv0) || (LCspeFuCE && LCLUT_10sv0) || (LCspeFuCD && LCLUT_15sv0) || (LCspeFuCC && LCLUT_20sv0);

    if ((!LCsecValid || !LCgpsTimeValid) && LCspeFuAct) {
      digitalWrite(SigOut, HIGH);  // clamp LED ON in case of invalid time signal
    }
    if (LCspeFuAct) {                // special functions for LED activation
      if (LCspecIntAct) {            // when dedicatEE second is active
        digitalWrite(SigOut, HIGH);  // clamp LED ON for special functions
      }
    } else {  // for normal operation flashes according to number wheels
      counter++;
      if (codeSwiBA > 0 && counter >= codeSwiBA) {  // after counted seconds have elapsed
        counter = 0;
        digitalWrite(SigOut, HIGH);  // clamp LED ON for individual time distances
      }
    }  // end LCspeFuAct and codeSwiBA
  }    // end LCppsSig
}  // end void PPStrigger

void GPStimeDebug(bool LCLocTimeon, bool LCSekon) {  // output: Fix: YES | time valid: JA | time updated: NO | hh:mm:ss = 16:9:23
  if (1 && !DEBUG_NMEA && LCLocTimeon && LCserialPrintON && (LCPPSfall)) {
    Serial.print(" Seku: ");
    Serial.print(Sekunde);
    Serial.print(" sek05; ");
    Serial.print(sek05);
    Serial.print(" byI ");
    Serial.print(byteIndex);
    Serial.print(" biI ");
    Serial.print(bitIndex);
    Serial.print(" BA ");
    Serial.print(codeSwiBA);
    Serial.print(codeSwiB);
    Serial.print(codeSwiA);
    Serial.print(" min ");
    Serial.print(minuteBits);
    Serial.print(" Fix: ");
    Serial.print(gps.location.isValid() ? "YES" : "NO");
    Serial.print(" | time valid: ");
    Serial.print(gps.time.isValid() ? "YES" : "NO");
    Serial.print(" | time updated: ");
    Serial.print(gps.time.isUpdated() ? "YES" : "NO");
    Serial.print(" | hh:mm:ss = ");
    Serial.print(gps.time.hour());
    Serial.print(":");
    Serial.print(gps.time.minute());
    Serial.print(":");
    Serial.print(gps.time.second());
    Serial.print(" | alive = ");
    Serial.print(LCalive);
    // Serial.print(" LCgpsTimeValid: ");
    // Serial.print(LCgpsTimeValid);
    // Serial.print(" LCgpsFix: ");
    // Serial.print(LCgpsFix);
    Serial.print("| ");

    if (1 && LCserialPrintON) {  // once a second for NMEA time data test
      Serial.print(gps.time.hour());
      Serial.print(":");
      Serial.print(gps.time.minute());
      Serial.print(":");
      Serial.print(gps.time.second());
      Serial.print(" ");
      Serial.print(Sekunde);
      Serial.print(" ");
      Serial.print(sek05);
    }
    Serial.println();
  }
  if (!DEBUG_NMEA && LCSekon && LCserialPrintON) {  // every loop! for testing; 0: for production
    Serial.print(codeSwiBA);
    Serial.print(" Sekunde = ");
    Serial.print(Sekunde);
    Serial.print(" sek05 = ");
    Serial.print(sek05);
    Serial.print(" TGPSRx = ");
    Serial.print(TGPSRx);
    Serial.print(" , millis() = ");
    Serial.print(millis());
    Serial.print(" , alive = ");
    Serial.print(LCalive);
    Serial.println();
  }
}  // end void GPStimeDebug

void SwitchOffClampLED() {  // switch off SigOut (clamp LED) after elapsed time first
  if (digitalRead(SigOut) == HIGH && (millis() - TpulseStart >= ONduration)) {
    digitalWrite(SigOut, LOW);  // switch off clamp LED
  }
} // void SwitchOffClampLED

void READcodeSwiBA() {  // calculate codeSwiBA which is a (for HEX artificial) number based on the number wheels
  codeSwiA = 0, codeSwiB = 0;
  for (int i = 0; i < 4; i++) {
    codeSwiB += (!digitalRead(SelHexB[i])) << i;
    codeSwiA += (!digitalRead(SelHexA[i])) << i;
  }
  if (LC_codeBCD) {
    codeSwiBA = LCcodeSwiOrder ? codeSwiB + 10 * codeSwiA : codeSwiA + 10 * codeSwiB;         // for BCD number wheel
  } else codeSwiBA = LCcodeSwiOrder ? codeSwiB + 100 * codeSwiA : codeSwiA + 100 * codeSwiB;  // for HEX number wheel
  // read number wheels only once every second, BCD wheels are stationary therefore read behind LED turn on for next 1PPS
} // void READcodeSwiBA

void SELspecFun() {                   // special functions
  if (LC_codeBCD) {                   // BCD number wheels
    LCspeFu00 = (codeSwiBA == 0);     // special function at "00": flash duration elongated for illumination control purpose
    LCspeFuFF = (codeSwiBA == 99);    // LCLUT_05s special function at "99": flash custom synchronized with GPS time
    LCspeFuFE = (codeSwiBA == 98);    // LCLUT_10s special function at "98": flash every 10s synchronized with GPS time
    LCspeFuFD = (codeSwiBA == 97);    // LCLUT_15s special function at "97": flash every 15s synchronized with GPS time
    LCspeFuFC = (codeSwiBA == 96);    // LCLUT_20s special function at "96": flash every 20s synchronized with GPS time
    LCspeFuFB = (codeSwiBA == 95);    // LCLUT_30s special function at "95": flash every 30s synchronized with GPS time
    LCspeFuFA = (codeSwiBA == 94);    // LCLUT_60s special function at "94": flash every 60s synchronized with GPS time
    LCspeFuEF = (codeSwiBA == 93);    // LCLUT_05s0 special function at "93" flash custom synchronized with GPS time
    LCspeFuEE = (codeSwiBA == 92);    // LCLUT_10s0 special function at "92": flash custom synchronized with GPS time
    LCspeFuED = (codeSwiBA == 91);    // LCLUT_15s0 special function at "91": flash custom synchronized with GPS time
    LCspeFuEC = (codeSwiBA == 90);    // LCLUT_20s0 special function at "90": flash custom synchronized with GPS time
    LCspeFuEB = (codeSwiBA == 89);    // LCLUT_30s0 special function at "89": flash custom synchronized with GPS time
    LCspeFuDF = (codeSwiBA == 88);    // LCLUT_DF special function at "88": flash custom synchronized with GPS time
    LCspeFuDE = (codeSwiBA == 87);    // LCLUT_DE special function at "87": flash custom synchronized with GPS time
    LCspeFuDD = (codeSwiBA == 86);    // LCLUT_DD special function at "86": flash custom synchronized with GPS time
    LCspeFuCF = (codeSwiBA == 85);    // LCLUT_05s0 special function at "85" flash custom synchronized with GPS time
    LCspeFuCE = (codeSwiBA == 84);    // LCLUT_10s0 special function at "84": flash custom synchronized with GPS time
    LCspeFuCD = (codeSwiBA == 83);    // LCLUT_15s0 special function at "83": flash custom synchronized with GPS time
    LCspeFuCC = (codeSwiBA == 82);    // LCLUT_20s0 special function at "82": flash custom synchronized with GPS time
  } else {                            // HEX number wheels
    LCspeFu00 = (codeSwiBA == 0);     // special function at "00": flash duration elongated for illumination control purpose
    LCspeFuFF = (codeSwiBA == 1515);  // LCLUT_05s  special function at "FF": flash custom synchronized with GPS time
    LCspeFuFE = (codeSwiBA == 1514);  // LCLUT_10s  special function at "FE": flash every 10s synchronized with GPS time
    LCspeFuFD = (codeSwiBA == 1513);  // LCLUT_15s  special function at "FD": flash every 15s synchronized with GPS time
    LCspeFuFC = (codeSwiBA == 1512);  // LCLUT_20s  special function at "FC": flash every 20s synchronized with GPS time
    LCspeFuFB = (codeSwiBA == 1511);  // LCLUT_30s  special function at "FB": flash every 30s synchronized with GPS time
    LCspeFuFA = (codeSwiBA == 1510);  // LCLUT_60s  special function at "FA": flash every 60s synchronized with GPS time
    LCspeFuEF = (codeSwiBA == 1415);  // LCLUT_05s0 special function at "EF": flash custom synchronized with GPS time
    LCspeFuEE = (codeSwiBA == 1414);  // LCLUT_10s0 special function at "EE": flash custom synchronized with GPS time
    LCspeFuED = (codeSwiBA == 1413);  // LCLUT_15s0 special function at "ED": flash custom synchronized with GPS time
    LCspeFuEC = (codeSwiBA == 1412);  // LCLUT_20s0 special function at "EC": flash custom synchronized with GPS time
    LCspeFuEB = (codeSwiBA == 1411);  // LCLUT_30s0 special function at "EB": flash custom synchronized with GPS time
    LCspeFuDF = (codeSwiBA == 1315);  // LCLUT_DF special function at "DF": flash custom synchronized with GPS time
    LCspeFuDE = (codeSwiBA == 1314);  // LCLUT_DE special function at "DE": flash custom synchronized with GPS time
    LCspeFuDD = (codeSwiBA == 1313);  // LCLUT_DD special function at "DD": flash custom synchronized with GPS time
    LCspeFuCF = (codeSwiBA == 1215);  // LCLUT_05s0 special function at "CF": flash custom synchronized with GPS time
    LCspeFuCE = (codeSwiBA == 1214);  // LCLUT_10s0 special function at "CE": flash custom synchronized with GPS time
    LCspeFuCD = (codeSwiBA == 1213);  // LCLUT_15s0 special function at "CD": flash custom synchronized with GPS time
    LCspeFuCC = (codeSwiBA == 1212);  // LCLUT_20s0 special function at "CC": flash custom synchronized with GPS time
  }
}  // end void SELspecFun

void BattCheck() {
  // battery check with valid GPS
  if (LCsecValid && (millis() - TpulseStart >= TlowBatDly)) {  // carry out battery check after PPS pulse has served LED
    LCTrigBatCheck = 1;
    //Serial.print(LClowBatActive); Serial.print("bc-"); Serial.print(LCBatCut); Serial.print("-"); Serial.print(UBatdigVal); Serial.print("-"); Serial.println(voltage);
  }
  // carry out battery check anyway without valid GPS, at least every 5 seconds
  if ((!LCsecValid && (millis() - TBatcheck >= TlowBatDlylong)) || abs(TBatcheck - TBatcheckOld) > 5000) {
    TBatcheckOld = TBatcheck;
    TBatcheck = millis();  // time of last battery check
    LCTrigBatCheck = 1;
    //Serial.print(LClowBatActive); Serial.print("Gi-"); Serial.print(LCBatCut); Serial.print("-"); Serial.print(voltage); Serial.print("-"); Serial.println(UBatdigVal);
  }
  if (LCTrigBatCheck) {  // check battery voltage
    LCTrigBatCheck = 0;
    UBatdigVal = analogRead(UBatanaIn);            // Value between 0 and 1023
    voltage = UBatdigVal * (refVoltage / 1023.0);  // conversion to battery voltage
    // info: 2,50V 0% # 3,0V 10% # 3,2V 20% # 3,22V 30% # 3,25V 40% # 3,26V 50% # 3,27V 60% # 3,3V 70% # 3,32V 80% # 3,35V 90% # 3,4V 100% ## 3,65V 100% charging
    LClowBatActive = LCLowBatEna && voltage < UBatLow;  // low battery indicator at 15% SOC
    // battery connection cutoff
    LCBatCut = LCLowBatEna && voltage < UBatCut;  // final battery cutoff, signal system cutoff at <5% SOC
    if (LCBatCut) {
      digitalWrite(cutoffPinOut, LOW);  // carry out battery cutoff, electrical system shutdown
    }
  }  // end LCTrigBatCheck
}  // end void BatCheck


void LowBatIndicator() {                      // battery indication
  if (LClowBatActive) {                       // low bat indicator: internal LED is blinking with high frequency
    halfPeriod = 1000.0 / (2.0 * frequency);  // half period for internal LED blinking
    LCblinkIntLED = 1;
  } else if (!LCsecValid || !LCgpsTimeValid) {
    halfPeriod = TNMEAhalf;  //  half period for NMEA data invalid
    LCblinkIntLED = 1;
  } else {
    LCblinkIntLED = 0;
    LCledState = 0;
    digitalWrite(LED_PIN, LOW);  // write internal LED LOW
  }

  if (LCblinkIntLED) {  // low bat indicator: internal LED is blinking with high frequency
    TcurBat = millis();
    if (TcurBat - TprevBat >= halfPeriod) {  // internal LED blinking fast
      TprevBat = TcurBat;                    // time update
      LCledState = !LCledState;              // invert LED state
      digitalWrite(LED_PIN, LCledState);     // write internal LED
    }                                        // end halfPeriod
  }                                          // end LCblinkIntLED
}  // end void LowBatIndicator

void UpdateGpsAlive() {  // let internal LED blink slowly if NMEA data is not updated
  LCalive = (millis() - TGPSRx) < timeoutMs;
  LCgpsStable = gps.location.isValid();                                           // one-time approval
  LCgpsFresh = LCgpsStable && (gps.location.age() < 1200);                        // optional additional
  LCgpsAlive = gps.location.isValid() && (millis() - gps.location.age() < 1800);  // gps.location.isValid is true over seconds to minutes
} // end void UpdateGpsAlive

void processNMEALine(String line) {
  // check, if NMEA-Line $GNRMC contains a valid state at field 3
  // $GNRMC,123519.00,A,4807.038,N,01131.000,E,0.0,0.0,230394,,,A*6C
  // if (statusChar == 'A') second is valid
  if (line.startsWith("$GNRMC")) {
    int commaIndex = 0;
    int lastIndex = 0;
    int field = 0;
    while (field < 2) {  // bis Feld 2 (Status)
      commaIndex = line.indexOf(',', lastIndex);
      if (commaIndex == -1) break;
      lastIndex = commaIndex + 1;
      field++;
    }
    if (commaIndex != -1) {
      char statusChar = line.charAt(lastIndex);
      LCgpsTimeValid = (statusChar == 'A');  // A = Sekunde gültig, !! used for internal LED
      TRMC = millis();                       //update time of GNRMC
    }
  }
} // end void processNMEALine
