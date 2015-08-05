#ifndef MLX90620_H_
#define MLX90620_H_
#endif // MLX90620_H_

/****************************************************************************
 * Title: Arduino based library to MLX90620 sensor
 * This library was based on IlBaboomba's code to MLX90620 sensor. The original code is disponible in http://forum.arduino.cc/index.php?topic=126244.0
 * Any other iformation like formulas or definition can be found on the sensor's datasheet which is disponible in http://www.melexis.com/Infrared-Thermometer-Sensors/Infrared-Thermometer-Sensors/MLX90620-776.aspx
 * Authors: Alexandre Loeblein Heinen & Clyvian Ribeiro Borges
 * CentraleSupélec :: Projet de Conception
 * Binôme A6.66 :: 2014/2015
 * Date: May 22, 2015
 ****************************************************************************/

#define byte uint8_t

#include <i2cmaster.h>
#include <Arduino.h>

/**
 \brief Calculate the ambient temperatured based on the input data. All the formulas can be found on the datasheet. See pages 14 and 15 of the datasheet.
 \param  The calculation required parameters.
 \return Ambient temperature.
 */
extern float calculate_TA(float, float, int, int);

/**
 \brief Calculate all the 64 pixels temperatures based on the IRDATA values and stores them into the temperatures vector. All the formulas can be found on the datasheet. See page 16 of the datasheet.
 \param  The calculation required parameters
 */
extern void calculate_TO(int, int, int*, int*, float*, float, float, int, int, int, int*, float*);

/**
 \brief Configure the oscilator frequency (uses config_MLX90620_Hz function) if the configuration register is equal to 0x04.
 \param  Frequency value.
 */
extern void check_Config_Reg_MLX90620(int);

/**
 \brief Configure the oscilator frequency. The frequency value must be 32, 16, 8, 4, 2 1 or 0 Hz. By default (if it's none of them) the set value is 1.
 \param  Frequency value.
 */
extern void config_MLX90620_Hz(int);

/**
 \brief Read the MLX90620 configuration refister.
 \return The two register bytes composed as an integer.
 */
extern int read_Config_Reg_MLX90620();

/**
 \brief Read the CPIX register used to the termal gradient compensation.
 \return The two register bytes composed as an integer.
 */
extern int read_CPIX_Reg_MLX90620();

/**
 \brief Read the whole EEPROM data and stores it at the parameter vector.
 \param The vector where the data will be stored in.
 */
extern void read_EEPROM_MLX90620(byte*);

/**
 \brief Read the IR sensors data from the RAM and stores it at the parameter vector.
 \param The vector where the data will be stored in.
*/
extern void read_IR(int*);

/**
 \brief Read the PTAT sensor value.
 \return The sensor's value.
 */
extern unsigned int read_PTAT_Reg_MLX90620();

/**
 \brief Send all the 64 values of the parameter vector by the established serial port (a serial communication must be established).
 \param The 64 entries vector to be sent.
 */
extern void transmit_temperatures(float*);

/**
 \brief Sets the sensor's trimming value.
 \param  The trimming value.
 */
extern void write_trimming_value(byte val);
//#endif // __cplusplus
