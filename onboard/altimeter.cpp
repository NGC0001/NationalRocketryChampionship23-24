#include <math.h>

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>

#include "adafruit_bmp3xx.h"
#include "common.h"

#define SEALEVELPRESSURE_HPA (1013.25)

constexpr const char *const OutputPrefix = "data/a_";

int main(int argc, char *argv[]) {
  const char *const outputPostfix = argc > 1 ? argv[1] : "";

  const auto start = std::chrono::system_clock::now();
  const auto start_ts = std::chrono::duration_cast<std::chrono::nanoseconds>(
      start.time_since_epoch()).count();
  const std::time_t start_t = std::chrono::system_clock::to_time_t(start);

  char startTimeStr[std::size("yymmdd-HHMMSS")];
  int nbytes = std::strftime(std::data(startTimeStr), std::size(startTimeStr),
      "%y%m%d-%H%M%S", std::localtime(&start_t));
  EXIT_IF(nbytes <= 0, "failed to format time");
  std::string fOutput = std::string(OutputPrefix)
      + startTimeStr + "_" + std::to_string(start_ts) + outputPostfix;
  std::ofstream ofsOutput(fOutput,
      std::ios_base::binary | std::ios_base::out | std::ios_base::app);
  EXIT_IF(!ofsOutput, "failed to open " + fOutput);
  std::cout << "data file: " << fOutput << std::endl;
  ofsOutput << std::put_time(std::localtime(&start_t), "%F %T %Z") << "\n"
            << start_ts << "\n" << std::endl;
  EXIT_IF(!ofsOutput, "failed to write output");

  Adafruit_BMP3XX bmp{};
  EXIT_IF(!bmp.begin_I2C(BMP3XX_I2C_BUS, BMP3XX_DEFAULT_ADDRESS),
      "could not find a valid BMP3 sensor");

  // Note, max measurement time for 8x/1x oversampling is 21.53ms
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setTemperatureOversampling(BMP3_NO_OVERSAMPLING);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_DISABLE);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);

  while (true) {
    EXIT_IF(!bmp.performReading(), "failed to perform reading");
    float atmospheric = bmp.pressure / 100.0F;
    float altitude = 44330.0 * (1.0 - pow(atmospheric / SEALEVELPRESSURE_HPA, 0.1903));
    const auto now = std::chrono::system_clock::now();
    const auto lapse = now - start;
    ofsOutput << std::chrono::duration_cast<std::chrono::nanoseconds>(lapse).count()
              << " " << bmp.temperature
              << " " << (bmp.pressure / 100.0)
              << " " << altitude
              << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
  }

  ofsOutput.close();
  return 0;
}
