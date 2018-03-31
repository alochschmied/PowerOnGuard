#ifndef Globals_h
#define Globals_h
#include <Arduino.h>
#include <TimerOne.h>
#include <EmonLib.h>



const byte PIN_IN_POTI_AMPS=15;
const byte PIN_IN_POTI_EXT=16;
const byte PIN_IN_POTI_DUR=14;

const byte PIN_OUT_LED_RED = 4;
// const byte PIN_OUT_LED_GREEN = 6;
const byte PIN_OUT_LED_GREEN = 5;
const byte PIN_IN_RESET_BUTTON = 2;
const byte PIN_OUT_SSR = 7;


typedef void (*buttonOnHigh)(void);

#endif
