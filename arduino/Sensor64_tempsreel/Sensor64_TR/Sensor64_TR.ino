#include <Servo.h>
#include <MLX90620.h>
#include <i2cmaster.h>

// Data and configuration parameters
byte EEPROM_DATA[64];
int sensorData[64];
int CFG, CPIX;
unsigned int PTAT;

// Temperatures
float t_amb;
float t_pix[64];

// Calculation variables
int a_cp, b_cp;          //
int a_ij[64];            //
int b_ij[64];            //
int b_i_scale;           //
float emissivity;          //
float k_t1, k_t2;        //
int v_th, tgc;           //
float alpha_ij[64];      //

// Refresh frequency
int frequency;

// Other variables
int counter;

void setup() {
  // standard setup
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  i2c_init();
  PORTC = (1 << PORTC4) | (1 << PORTC5);
  delay(5);
  
  // data initialization
  frequency = 2;
  read_EEPROM_MLX90620(EEPROM_DATA);
  varInitialization();
  config_MLX90620_Hz(frequency);
}

void loop(){
  
  delay(1000.0/frequency); // a little delay
  
  // **************** IMAGE ACQUISITION **************************************
   if(counter == 0){		//TA refresh is slower than the pixel readings, I'll read the values and computate them not every loop.
     PTAT = read_PTAT_Reg_MLX90620(); // PTAT stands for pro Proportional To Absolute Temperature
     t_amb = calculate_TA(k_t1, k_t2, v_th, PTAT); // ambient's temperature
     check_Config_Reg_MLX90620(frequency);        // reconfigure if it's needed
   }
   
   counter++;
   if(counter >=16){
     counter = 0;
   }
   
   // read and calculate
   read_IR(sensorData); // gets the lecture from the sensor
   CPIX = read_CPIX_Reg_MLX90620(); // 
   calculate_TO(a_cp, b_cp, a_ij, b_ij, alpha_ij, emissivity, t_amb, tgc, CPIX, b_i_scale, sensorData, t_pix);
   // ****************************************************************************
   
   // DATA TRANSMITION
   transmit_temperatures(t_pix);        // after we transmit all the image
   
}

/*
  Identifies the data on the EEPROM and assigns to the specific variable (always to make the code cleaner as we could use directly EEPROM_DATA)
  Obs: some of them are stored in two bytes (most and less significant) and some of them are store as 2's complement
*/
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
