/*
 * MLX90620.cpp
 * 13.05.2015
 * Authors: Alexandre Loeblein Heinen & Clyvian Ribeiro Borges
 */

/*
* Attention! I commented out the alpha_ij array, so if you're going to compile the sketch you'll get for sure an error.
* You should replace all 64 values with the alpha_ij calculated using the values stored in your MLX90620's EEPROM.
* I suggest you to make an EEPROM dump, print it on the Serial port and store it in a file. From there, with the help of a spreadsheet (Libreoffice, Google Docs, Excel...) calculate your own alpha_ij values.
* Please also pay attention to your emissivity value: since in my case it was equal to 1, to save SRAM i cut out that piece of calculation. You need to restore those lines if your emissivity value is not equal to 1.
*/

#include "MLX90620.h"

MLX90620::MLX90620(int f)
{
  // data initialization
  this->frequency = f;
  read_EEPROM_MLX90620();
  varInitialization();
  config_MLX90620_Hz(this->frequency);
  this->counter = 0;
}

MLX90620::~MLX90620()
{

}

void MLX90620::loop()
{
  delay(1000.0/this->frequency); // a little delay

  // **************** IMAGE ACQUISITION **************************************
   if(counter == 0)
   {		                //TA refresh is slower than the pixel readings, I'll read the values and computate them not every loop.
     readPTAT();        // PTAT stands for pro Proportional To Absolute Temperature
     calculateTA();     // ambient's temperature
     checkConfigReg();  // reconfigure if it's needed
   }

   counter++;
   if(counter >= 16)
      counter = 0;

   // read and calculate
   readIR();              // gets the lecture from the sensor
   readCPIX();
   calculateTO();

   // DATA TRANSMITION
   transmitTemperatures();        // after we transmit all the image
}

void MLX90620::calculateTA ()
{
    t_amb = (-k_t1 + sqrt(k_t1*k_t1 - 4.0*k_t2*(v_th - PTAT)))/(2.0*k_t2) + 25.0;
}

void MLX90620::calculateTO()
{
    float temp = 0;                                                             // auxiliare
    float v_comp = CPIX - (a_cp +  b_cp/pow(2, b_i_scale)*(ta - 25.0));         // temperature gradient pixel
    for (int i = 0; i < 64; i++)
    {
        temp = IRDATA[i] - (a_ij[i] +  b_ij[i]/pow(2, b_i_scale)*(ta - 25.0));  // offset compensation
        temp = temp - tgc/32.0*v_comp;                                          // temperature gradient compensation
        temp = temp/emissivity;                                                 // emissivity compensation
        temperatures[i] = sqrt(sqrt( temp/alpha_ij[i] + pow((ta + 273.15), 4) )) - 273.15; // temperature calculation
    }
}

void MLX90620::checkConfigReg()
{
    int CFG = readConfigReg();
    byte CFG_MSB = (byte) (CFG >> 8);
    if ((!CFG_MSB & 0x04) == 0x04)
        config(this->frequency);
}

void MLX90620::config(int Hz)
{
    byte Hz_LSB;
    switch(Hz)
    {
      case 0:
          Hz_LSB = B00001111;
          break;
      case 1:
          Hz_LSB = B00001110;
          break;
      case 2:
          Hz_LSB = B00001101;
          break;
      case 4:
          Hz_LSB = B00001100;
          break;
      case 8:
          Hz_LSB = B00001011;
          break;
      case 16:
          Hz_LSB = B00001010;
          break;
      case 32:
          Hz_LSB = B00001001;
          break;
      default:
          Hz_LSB = B00001110;
    }
    i2c_start_wait(0xC0);
    i2c_write(0x03);
    i2c_write((byte)Hz_LSB-0x55);
    i2c_write(Hz_LSB);
    i2c_write(0x1F);
    i2c_write(0x74);
    i2c_stop();
}

void MLX90620::readConfigReg()
{
    byte CFG_MSB, CFG_LSB;
    i2c_start_wait(0xC0);
    i2c_write(0x02);
    i2c_write(0x92);
    i2c_write(0x00);
    i2c_write(0x01);
    i2c_rep_start(0xC1);
    CFG_LSB = i2c_readAck();
    CFG_MSB = i2c_readAck();
    i2c_stop();
    this->CFG = (CFG_MSB << 8) + CFG_LSB;
}

void MLX90620::readCPIX()
{
    byte CPIX_LSB, CPIX_MSB;
    i2c_start_wait(0xC0);
    i2c_write(0x02);
    i2c_write(0x91);
    i2c_write(0x00);
    i2c_write(0x01);
    i2c_rep_start(0xC1);
    CPIX_LSB = i2c_readAck();
    CPIX_MSB = i2c_readAck();
    i2c_stop();
    this->CPIX = (CPIX_MSB << 8) + CPIX_LSB;
}

byte MLX90620::getEEPROM(int ind)
{
  return EEPROM_DATA[ind];
}

void MLX90620::readEEPROM()
{
    i2c_start_wait(0xA0);
    i2c_write(0x00);
    i2c_rep_start(0xA1);
    for(unsigned int i = 0; i < 256; i++)
    {
      this->EEPROM_DATA[i] = i2c_readAck();
    }
    i2c_stop();
    writeTrimmingValue(this->EEPROM_DATA[0xF7]);
}

int getIRDATA(int ind)
{
  return this->IRDATA[ind];
}

void MLX90620::readIR()
{
    byte PIX_LSB;
    byte PIX_MSB;
    i2c_start_wait(0xC0);
    i2c_write(0x02);
    i2c_write(0x00);
    i2c_write(0x01);
    i2c_write(0x40);
    i2c_rep_start(0xC1);
    for(int i = 0; i < 64; i++)
    {
        PIX_LSB = i2c_readAck();
        PIX_MSB = i2c_readAck();
        this->IRDATA[i] = (PIX_MSB << 8) + PIX_LSB;
    }
    i2c_stop();
}

void MLX90620::readPTAT()
{
    byte PTAT_LSB, PTAT_MSB;
    i2c_start_wait(0xC0);
    i2c_write(0x02);
    i2c_write(0x90);
    i2c_write(0x00);
    i2c_write(0x01);
    i2c_rep_start(0xC1);
    PTAT_LSB = i2c_readAck();
    PTAT_MSB = i2c_readAck();
    i2c_stop();
    this->PTAT = ((unsigned int)PTAT_MSB << 8) + PTAT_LSB;
}

void transmitTemperatures()
{
    for(int i=0; i<=63; i++)
        Serial.println(this->temperatures[i]);
}

void writeTrimmingValue(byte val)
{
    i2c_start_wait(0xC0);
    i2c_write(0x04);
    i2c_write((byte)val-0xAA);
    i2c_write(val);
    i2c_write(0x56);
    i2c_write(0x00);
    i2c_stop();
}

void varInitialization(){
  // those formulas can be found on page 14 of the datasheet
   v_th = (EEPROM_DATA[0xDB] <<8) + EEPROM_DATA[0xDA];
   k_t1 = ((EEPROM_DATA[0xDD] <<8) + EEPROM_DATA[0xDC])/1024.0;
   k_t2 =((EEPROM_DATA[0xDF] <<8) + EEPROM_DATA[0xDE])/1048576.0;

    // page 17
   a_cp = EEPROM_DATA[0xD4];
   if(a_cp > 127){
     a_cp = a_cp - 256;
   }

   b_cp = EEPROM_DATA[0xD5];
   if(b_cp > 127){
     b_cp = b_cp - 256;
   }

   tgc = EEPROM_DATA[0xD8];
   if(tgc > 127){
     tgc = tgc - 256;
   }

   b_i_scale = EEPROM_DATA[0xD9];

   emissivity = (((unsigned int)EEPROM_DATA[0xE5] << 8) + EEPROM_DATA[0xE4])/32768.0;

   for(int i=0;i < 64; i++){
     a_ij[i] = EEPROM_DATA[i]; // les valeurs des As commencent en 0x00
     if(a_ij[i] > 127){
       a_ij[i] = a_ij[i] - 256; // stored as 2's complement!
     }
     b_ij[i] = EEPROM_DATA[0x40+i]; // et celles des Bs, en 0x40
     if(b_ij[i] > 127){
       b_ij[i] = b_ij[i] - 256;  // stored as 2's complement!
     }
   }

   // calculation of alphas
   // the datasheet specifies where each value can be found on the EEPROM
   unsigned int alpha0_H, alpha0_L, alpha0_S, delta_alpha_S, delta_alpha_ij; // we just name those variables to make the code cleaner
   alpha0_L = EEPROM_DATA[0xE0];
   alpha0_H = EEPROM_DATA[0xE1];
   alpha0_S = EEPROM_DATA[0xE2];
   delta_alpha_S = EEPROM_DATA[0xE3]; // because we could use directly the EEPROM_DATA
   int c;
   for (c=0; c < 64; c++) {
     delta_alpha_ij = EEPROM_DATA[0x80 + c];
     alpha_ij[c] = (256.0*alpha0_H + alpha0_L)/(pow(2, alpha0_S)) + delta_alpha_ij/(pow(2, delta_alpha_S)); // we can found this formula on datasheet's page 17
   }
}
