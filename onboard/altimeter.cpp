/***************************************************************************
  This is a library for the BMP3XX temperature & pressure sensor

  Designed specifically to work with the Adafruit BMP388 Breakout
  ----> http://www.adafruit.com/products/3966

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include <chrono>
#include <cstdlib>
#include <thread>

#include "adafruit_bmp3xx.h"

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BMP3XX bmp;

void setup() {
  DPRINTLN("Adafruit BMP388 / BMP390 test");

  if (!bmp.begin_I2C(BMP3XX_I2C_BUS, BMP3XX_DEFAULT_ADDRESS)) {
    DPRINTLN("Could not find a valid BMP3 sensor, check wiring!");
    std::exit(EXIT_FAILURE);
  }

  // Set up oversampling and filter initialization
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
}

void loop() {
  if (!bmp.performReading()) {
    DPRINTLN("Failed to perform reading :(");
    std::exit(EXIT_FAILURE);
  }
  DPRINT("Temperature = "); DPRINT(bmp.temperature); DPRINTLN(" *C");

  DPRINT("Pressure = "); DPRINT(bmp.pressure / 100.0); DPRINTLN(" hPa");

  DPRINT("Approx. Altitude = ");
  DPRINT(bmp.readAltitude(SEALEVELPRESSURE_HPA)); DPRINTLN(" m");

  DPRINTLN("");
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

int main() {
  setup();
  while (1) { loop(); }
  return 0;
}
