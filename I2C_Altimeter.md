## raspberry pi i2c/smbus

* [raspberry pi I2C Pinout](https://pinout.xyz/pinout/i2c)
* [raspberry pi using i2c](https://learn.adafruit.com/adafruits-raspberry-pi-lesson-4-gpio-setup/configuring-i2c)
* [raspberry pi using i2c](https://raspberry-projects.com/pi/programming-in-c/i2c/using-the-i2c-interface)
* install `i2c-tools`
* enable kernel support through command `raspi-config`
* detect i2c devices with command `sudo i2cdetect -y 1`
* install `libi2c-dev` for i2c programming, and add `-li2c` flag when linking
* note, `<i2c/smbus.h>` is a pure c header, so needs `extern "C"` in cpp files

## BMP388

* [Adafruit BMP388 product](https://www.adafruit.com/product/3966)
* [Adafruit BMP388 cpp library](https://github.com/adafruit/Adafruit_BMP3XX)
* [BMP280 Pinout](https://learn.adafruit.com/adafruit-bmp280-barometric-pressure-plus-temperature-sensor-breakout/pinouts)
* [BMP3 API](https://github.com/boschsensortec/BMP3_SensorAPI)
* [BMP388 Datasheet](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmp388-ds001.pdf)

## I2C interface references

* [i2c ioctl method example](https://stackoverflow.com/questions/9974592/i2c-slave-ioctl-purpose)
* [i2c ioctl smbus method example](https://stackoverflow.com/questions/55976683/read-a-block-of-data-from-a-specific-registerfifo-using-c-c-and-i2c-in-raspb)
* [linux kernel i2c interface](https://www.kernel.org/doc/Documentation/i2c/dev-interface)
* [linux kernel smbus interface](https://manpages.debian.org/unstable/i2c-tools/libi2c.3.en.html)
* [linux kernel smbus protocol](https://www.kernel.org/doc/Documentation/i2c/smbus-protocol)
* [linux i2c smbus subsystem](https://docs.kernel.org/driver-api/i2c.html)
* [adafruit bmp3xx python lib](https://github.com/adafruit/Adafruit_CircuitPython_BMP3XX)
* note, i2c direct file I/O interface with `read`/`write`:
  - for reading, `write` register address, then `read` data
  - for writing, **combine** register address and data into single buffer, then `write` this buffer
  - i2c/smbus seems to work in so-called 'transaction's, and is not fully supported by file I/O interface
* note, i2c smbus `i2c_smbus_read_block_data`/`i2c_smbus_write_block_data` seems not working on raspberry pi
  - try `i2c_smbus_read_i2c_block_data`/`i2c_smbus_write_i2c_block_data` instead
* note, for i2c ioctl smbus method, use `I2C_SMBUS` request and `I2C_SMBUS_I2C_BLOCK_DATA` size option
