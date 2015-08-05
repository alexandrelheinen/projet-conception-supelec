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

extern float calculate_TA (float k_t1, float k_t2, int v_th, int PTAT)
{
    float ta;
    ta = (-k_t1 + sqrt(k_t1*k_t1 - 4.0*k_t2*(v_th - PTAT)))/(2.0*k_t2) + 25.0; 	//it's much more simple now, isn't it? :)
    return ta;
}

extern void calculate_TO(int a_cp, int b_cp, int a_ij[64], int b_ij[64], float alpha_ij[64], float emissivity, float ta, int tgc, int CPIX, int b_i_scale, int IRDATA[64], float temperatures[64])
{
    float temp = 0;                                                             // auxiliare
    float v_comp = CPIX - (a_cp +  b_cp/pow(2, b_i_scale)*(ta - 25.0));         // temperature gradient pixel
    for (int i=0; i<64; i++)
    {
        temp = IRDATA[i] - (a_ij[i] +  b_ij[i]/pow(2, b_i_scale)*(ta - 25.0));  // offset compensation
        temp = temp - tgc/32.0*v_comp;                                          // temperature gradient compensation
        temp = temp/emissivity;                                                 // emissivity compensation
        temperatures[i] = sqrt(sqrt( temp/alpha_ij[i] + pow((ta + 273.15), 4) )) - 273.15; // temperature calculation
    }
}

extern void check_Config_Reg_MLX90620(int freq)
{
    int CFG = read_Config_Reg_MLX90620();
    byte CFG_MSB = (byte) (CFG >> 8);
    if ((!CFG_MSB & 0x04) == 0x04)
    {
        config_MLX90620_Hz(freq);
    }
}

extern void config_MLX90620_Hz(int Hz)
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

extern int read_Config_Reg_MLX90620()
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
    int CFG = (CFG_MSB << 8) + CFG_LSB;
    return CFG;
}

extern int read_CPIX_Reg_MLX90620()
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
    int CPIX = (CPIX_MSB << 8) + CPIX_LSB;
    return CPIX;
}

extern void read_EEPROM_MLX90620(byte* EEPROM_DATA)
{
    i2c_start_wait(0xA0);
    i2c_write(0x00);
    i2c_rep_start(0xA1);
    for(int i=0; i<=255; i++)
    {
        EEPROM_DATA[i] = i2c_readAck();
    }
    i2c_stop();
    write_trimming_value(EEPROM_DATA[0xF7]);
}

extern void read_IR(int IRDATA[64])
{
    byte PIX_LSB;
    byte PIX_MSB;
    i2c_start_wait(0xC0);
    i2c_write(0x02);
    i2c_write(0x00);
    i2c_write(0x01);
    i2c_write(0x40);
    i2c_rep_start(0xC1);
    for(int i=0; i<=63; i++)
    {
        PIX_LSB = i2c_readAck();
        PIX_MSB = i2c_readAck();
        IRDATA[i] = (PIX_MSB << 8) + PIX_LSB;
    }
    i2c_stop();
}

extern unsigned int read_PTAT_Reg_MLX90620 ()
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
    unsigned int PTAT = ((unsigned int)PTAT_MSB << 8) + PTAT_LSB;
    return PTAT;
}

extern void transmit_temperatures (float* temperatures)
{
    for(int i=0; i<=63; i++)
    {
        Serial.println(temperatures[i]);
    }
}

void write_trimming_value(byte val)
{
    i2c_start_wait(0xC0);
    i2c_write(0x04);
    i2c_write((byte)val-0xAA);
    i2c_write(val);
    i2c_write(0x56);
    i2c_write(0x00);
    i2c_stop();
}

/** THIS FUNCTION IS JUST AN EXAMPLE OF HOW TO INITIALIZE THE SENSOR'S VARIABLES

void varInitialization(){
 v_th = (EEPROM_DATA[219] <<8) + EEPROM_DATA[218];
 k_t1 = ((EEPROM_DATA[221] <<8) + EEPROM_DATA[220])/1024.0;
 k_t2 =((EEPROM_DATA[223] <<8) + EEPROM_DATA[222])/1048576.0;

 a_cp = EEPROM_DATA[212];
 if(a_cp > 127){
   a_cp = a_cp - 256;
 }
 b_cp = EEPROM_DATA[213];
 if(b_cp > 127){
   b_cp = b_cp - 256;
 }
 tgc = EEPROM_DATA[216];
 if(tgc > 127){
   tgc = tgc - 256;
 }

 b_i_scale = EEPROM_DATA[217];

 emissivity = (((unsigned int)EEPROM_DATA[229] << 8) + EEPROM_DATA[228])/32768.0;

 for(int i=0;i<=63;i++){
   a_ij[i] = EEPROM_DATA[i];
   if(a_ij[i] > 127){
     a_ij[i] = a_ij[i] - 256;
   }
   b_ij[i] = EEPROM_DATA[64+i];
   if(b_ij[i] > 127){
     b_ij[i] = b_ij[i] - 256;
   }
 }
}
************************************************************************************/
