#include <Servo.h>
#include <MLX90620.h>
#include <i2cmaster.h>

void setup() {
  // standard setup
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  i2c_init();
  PORTC = (1 << PORTC4) | (1 << PORTC5);
  delay(5);

  MLX90620 sensor(2);
}

void loop()
{
  sensor.loop();
}
