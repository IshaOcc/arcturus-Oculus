//
//  IRLEDDriver.h
//  Driver_iOS
//
//  Created by Forrest Heller on 5/17/13.
//  Copyright (c) 2013 Occipital. All rights reserved.
//

#pragma once

/* These #define's are taken directly from the OCIrLedDriver.h file.
   The IRLEDDriver iOS module just writes an Occipital command that can 
   be sent over the accessory config session (io.structure.control) */
    
// Specifies main operation of the driver.
// Light assist means medium current, constantly on
// Light indicator means low pwm current, constantly on
// Flash is a reoccuring flash at high current
typedef enum {
    OC_IR_LED_DRIVER_MODE_SHUTDOWN = 0,
    OC_IR_LED_DRIVER_MODE_LIGHT_INDICATOR = 1,
    OC_IR_LED_DRIVER_MODE_LIGHT_ASSIST =2,
    OC_IR_LED_DRIVER_MODE_FLASH = 3,
} OCIrLedDriver_Mode;


// Individual CurrentMode means that the flash, indicator, and assist modes
// will be driven at the currents contained in the appropriate types defined
// below.
// When CurrentMode is OC_IR_LED_DRIVER_CURRENT_MODE_BITS_SMALL_RANGE or
// OC_IR_LED_DRIVER_CURRENT_MODE_BITS_LARGE_RANGE, this value will be used
// to control the current for flash, indicator, and assist. It gets 5 bits,
// so the acceptable range is from 0 to 0x1F. The small range goes from 10mA
// to 320mA and the large range goes from 15.6mA to 500mA.
typedef enum {
    OC_IR_LED_DRIVER_CURRENT_MODE_INDIVIDUAL = 0, // default
    OC_IR_LED_DRIVER_CURRENT_MODE_BITS_SMALL_RANGE = 1,
    OC_IR_LED_DRIVER_CURRENT_MODE_BITS_LARGE_RANGE = 3,
} OCIrLedDriver_CurrentMode;

// Current at which LEDs are driven in indicator mode
typedef enum {
    OC_IR_LED_DRIVER_INDICATOR_CURRENT_1_35mA = 0,
    OC_IR_LED_DRIVER_INDICATOR_CURRENT_2_90mA = 1,
    OC_IR_LED_DRIVER_INDICATOR_CURRENT_4_40mA = 2, // default
    OC_IR_LED_DRIVER_INDICATOR_CURRENT_6_00mA = 3,
} OCIrLedDriver_IndicatorCurrent;

// Current at which LEDs are driven in light assist mode
typedef enum {
    OC_IR_LED_DRIVER_LIGHT_ASSIST_CURRENT_51_6mA =  0,
    OC_IR_LED_DRIVER_LIGHT_ASSIST_CURRENT_72_3mA =  1, // default
} OCIrLedDriver_LightAssistCurrent;

// Current at which LEDs are driven in flash mode
typedef enum {
    OC_IR_LED_DRIVER_FLASH_CURRENT_260mA = 0,
    OC_IR_LED_DRIVER_FLASH_CURRENT_280mA = 1,
    OC_IR_LED_DRIVER_FLASH_CURRENT_300mA = 2, // default
    OC_IR_LED_DRIVER_FLASH_CURRENT_320mA = 3,
} OCIrLedDriver_FlashCurrent;

// How often the flash is triggered when in flash mode
typedef enum {
    OC_IR_LED_DRIVER_FLASH_TIMER_MS_30 = 0x00,
    OC_IR_LED_DRIVER_FLASH_TIMER_MS_60 = 0x01,
    OC_IR_LED_DRIVER_FLASH_TIMER_MS_90 = 0x02,
    OC_IR_LED_DRIVER_FLASH_TIMER_MS_120 = 0x03,
    OC_IR_LED_DRIVER_FLASH_TIMER_MS_150 = 0x04, // default
    OC_IR_LED_DRIVER_FLASH_TIMER_MS_180 = 0x05,
    OC_IR_LED_DRIVER_FLASH_TIMER_MS_210 = 0x06,
    OC_IR_LED_DRIVER_FLASH_TIMER_MS_240 = 0x07,
    OC_IR_LED_DRIVER_FLASH_TIMER_MS_270 = 0x08,
    OC_IR_LED_DRIVER_FLASH_TIMER_MS_300 = 0x09,
    OC_IR_LED_DRIVER_FLASH_TIMER_MS_330 = 0x0A,
    OC_IR_LED_DRIVER_FLASH_TIMER_MS_360 = 0x0B,
    OC_IR_LED_DRIVER_FLASH_TIMER_MS_390 = 0x0C,
    OC_IR_LED_DRIVER_FLASH_TIMER_MS_420 = 0x0D,
    OC_IR_LED_DRIVER_FLASH_TIMER_MS_450 = 0x0E,
    OC_IR_LED_DRIVER_FLASH_TIMER_MS_480 = 0x0F,
} OCIrLedDriver_FlashTimer;

typedef enum {
    OC_IR_LED_ACTION_DO_NOTHING = 0,
    OC_IR_LED_ACTION_TURN_ON = 1,
    OC_IR_LED_ACTION_TURN_OFF = 2
} OCIrLedDriver_Action;
