#ifndef MLX90620_H_
#define MLX90620_H_

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

class MLX90620
{
public:
  // constructor and destructor
  MLX90620(int);
  ~MLX90620();
  void loop();
  // get functions
  byte getEEPROM(int);
  int getIRDATA(int);
protected:
  // calculations
  void calculateTA();
  void calculateTO();
  // configuration/initialization functions
  void checkConfigReg(int);
  void config(int);
  void writeTrimmingValue(byte val);
  void varInitialization();
  // read function get from the sensor a specified quantity and store it into the respective variable
  void readConfigReg();
  void readCPIX();
  void readEEPROM();
  void readIR(int*);
  void readPTAT();
  // serial interface
  void transmitTemperatures();

private:
  // Data and configuration parameters
  byte EEPROM_DATA[64];
  int IRDATA[64];
  int CFG, CPIX;
  unsigned int PTAT;
  // Temperatures
  float t_amb;
  float temperatures[64];
  // Calculation variables
  int a_cp, b_cp;          //
  int a_ij[64];            //
  int b_ij[64];            //
  int b_i_scale;           //
  float emissivity;        //
  float k_t1, k_t2;        //
  int v_th, tgc;           //
  float alpha_ij[64];      //
  // Refresh frequency
  int frequency;
  // auxiliar variable
  int counter;
};
#endif // __cplusplus
