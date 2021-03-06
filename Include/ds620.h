/**
 * @file   ds620.h
 * @author David <david@edeca.net>
 * @date   November, 2010
 * @brief  Header for DS620 temperature sensor library.
 * @sa     <a href="http://datasheets.maxim-ic.com/en/ds/DS620.pdf">DS620 datasheet</a>
 * @details
 *
 * A library for accessing the DS620 temperature sensor, using i2c.
 *
 * Example Usage:
 * @code
 *    unsigned short temp = ds620_GetTemperature(0b001);
 *    ds620_PrintTemperature(temp);
 *    signed short decimal = ds620_ToDecimal(temp);
 * @endcode
 *
 * @todo Add a function to set continuous or one-shot mode
 * @todo Add a function to set thermostat trip points
 * @todo Add code to set ADC resolution
 * @todo Add function to return portion of temperature after decimal point
 */

#ifndef _DS620_H
#define _DS620_H

/* 
 * User definable settings
 */

// None
  
/*
 * Constants, do not edit below this line
 */

/** 
 * Used to determine the I2C address of the DS620.  First 4 bits are fixed, followed by 3 bit sensor ID then the I2C R/W bit.
 */
#define DS620_ADDRESS_MASK 0b10010000

/** Start converting temperature command.  In one-shot mode, this triggers a single conversion.  In continuous mode it starts periodic conversion. */
#define DS620_START_CONVERT 0x51
/** Stop converting temperature command. */
#define DS620_STOP_CONVERT 0x22
/** Copy data from EEPROM to SRAM (shadow registers) command. */
#define DS620_RECALL_DATA 0xB8
/** Copy data from SRAM (shadow registers) to EEPROM command. */
#define DS620_COPY_DATA 0x48

/** @defgroup ds620_addresses DS620 internal memory locations
 *  @{
 */

/** Thermostat upper trip-point (MSB) */
#define DS620_TH_MSB 0xA0
/** Thermostat upper trip-point (LSB) */
#define DS620_TH_LSB 0xA1
/** Thermostat lower trip-point (MSB) */
#define DS620_TL_MSB 0xA2
/** Thermostat lower trip-point (LSB) */
#define DS620_TL_LSB 0xA3
/** Current temperature (MSB) */
#define DS620_TEMP_MSB 0xAA
/** Current temperature (LSB) */
#define DS620_TEMP_LSB 0xAB
/** Internal configuration register (MSB) */
#define DS620_CONFIG_MSB 0xAC
/** Internal configuration register (LSB) */
#define DS620_CONFIG_LSB 0xAD

/** User register, can be used for general purpose data storage */
#define DS620_USER1 0xA4
/** User register, can be used for general purpose data storage */
#define DS620_USER2 0xA5
/** User register, can be used for general purpose data storage */
#define DS620_USER3 0xA6
/** User register, can be used for general purpose data storage */
#define DS620_USER4 0xA7



/** @} */

/** The configuration register */
typedef union
{
	struct
	{
		/** Is conversion finished? 1 = YES, 0 = NO.  Read only. */
		unsigned DONE:1;
		/** Is an EEPROM write in progress? 1 = YES, 0 = NO.  Read only. */
		unsigned NVB:1;
		/** If set, the temperature has met or exceeded the value set in the TEMP_HIGH register.  Cleared by the user or device reset. */
		unsigned THF:1;
		/** If set, the temperature has met or fallen below the value set in the TEMP_LOW register.  Cleared by the user or device reset. */
		unsigned TLF:1;
		/** Used to set conversion resolution */
		unsigned R1:1;
		/** Used to set conversion resolution */
		unsigned R0:1;
		/** Should the DS620 power up converting? 1 = YES, 0 = NO. */
		unsigned AUTOC:1;
		/** Conversion mode: 1 = One shot mode, 0 = Continuous mode. */
		unsigned ONESHOT:1;
		/** Configures thermostat mode for the PO pin.  See datasheet. */
		unsigned PO2:1;
		/** Configures thermostat mode for the PO pin.  See datasheet. */
		unsigned PO1:1;
		/** Address bit 2, as set by the A2 pin.  Read only. */
		unsigned A2:1;
		/** Address bit 1, as set by the A1 pin.  Read only. */
		unsigned A1:1;
		/** Address bit 0, as set by the A0 pin.  Read only. */
		unsigned A0:1;
		/** User memory for general purpose storage. */
		unsigned USER2:1;
		/** User memory for general purpose storage. */
		unsigned USER1:1;
		/** User memory for general purpose storage. */
		unsigned USER0:1;
	} bits;
	unsigned short value;
} ds620_config;

/**
 * @internal
 * Convert a 3 byte address to the correct I2C address.
 *
 * @param address  The sensor address (between 0 and 7)
 * @return The I2C bus address of this device
 * @endinternal
 */
int _ds620_GetI2CAddress(int address);
/**
 * Print a temperature value using printf.
 *
 * @param reading A 16-bit value from the sensor
 */
void ds620_PrintTemperature(short reading);
/**
 * Read an 8-bit register.
 *
 * This function retrieves one bytes from the DS620.
 *
 * @param address 	The sensor address (0-7)
 * @param reg		The internal register
 */
unsigned int ds620_ReadRegister8(int address, int reg);
/**
 * Write an 8-bit register.
 *
 * This function writes one byte to the SRAM of the 
 * DS620.  In order to save changes to EEPROM, the function 
 * ds620_CopyData() should be called.
 *
 * @param address 	The sensor address (0-7)
 * @param reg		The internal register
 * @param data		The data to be written
 */
void ds620_WriteRegister8(int address, int reg, int data);

/**
 * Read a 16-bit register.
 *
 * This function retrieves two adjacent bytes from the
 * DS620, storing the result as a 16 bit value.
 *
 * @param address 	The sensor address (0-7)
 * @param reg		The internal register
 */
unsigned short ds620_ReadRegister16(int address, int reg);
/**
 * Write a 16-bit register.
 *
 * This function writes two adjacent bytes to the SRAM of the 
 * DS620.  In order to save changes to EEPROM, the function 
 * ds620_CopyData() should be called.
 *
 * @param address 	The sensor address (0-7)
 * @param reg		The internal register
 * @param data		The data to be written
 */
void ds620_WriteRegister16(int address, int reg, unsigned short data);
/**
 * Copy data from SRAM (shadow registers) to EEPROM.  See datasheet
 * section "COPYING TO EEPROM COMMAND SEQUENCE".
 *
 * Note that this will start converting temperature and temporarily
 * place the sensor in continuous mode.  After copying, conversion
 * will be disabled.  It is up to the user to reinstate continuous
 * conversion if required.
 *
 * @param address 	The sensor address (0-7)
 */
void ds620_CopyData(int address);
/**
 * Retrieve the decimal portion of the sensor reading.
 *
 * Removes the fraction part and handles sign extension
 * from 9-bit to a 16-bit short.
 *
 * @param reading A 16-bit value from the sensor
 */
signed short ds620_ToDecimal(short reading);
/**
 * Get the current temperature register as a 16-bit value.
 * 
 * First 9 bits of return value are a twos-complement signed
 * integer.  The next 4 bits depend on the configured resolution.
 * The last 3 bits are always zero.
 *
 * @param address	The sensor address (0-7)
 */
unsigned short ds620_GetTemperature(int address);
/**
 * Get the internal configuration as a 16-bit value.
 *
 * @param address	The sensor address (0-7)
 */
ds620_config ds620_GetConfiguration(int address);
/**
 * Set the internal configuration.  To save it for usage after
 * a power cycle, use ds620_CopyData().
 *
 * @param address	The sensor address (0-7)
 * @param config	The new configuration
 */
void ds620_SetConfiguration(int address, ds620_config config);
/**
 * In one-shot mode, intiate a single conversion.  In continuous
 * mode, start automatic conversions.  See ds620_StopConversion()
 * for continuous mode.
 *
 * @param address	The sensor address (0-7)
 */
void ds620_StartConversion(int address);
/**
 * Stop automatic conversions, applicable only in continuous mode.
 *
 * @param address	The sensor address (0-7)
 */
void ds620_StopConversion(int address);

#endif
