# configuring i2c/smbus
* [raspberry pi using i2c](https://learn.adafruit.com/adafruits-raspberry-pi-lesson-4-gpio-setup/configuring-i2c)
* [raspberry pi using i2c](https://raspberry-projects.com/pi/programming-in-c/i2c/using-the-i2c-interface)
* install `i2c-tools`
* enable kernel support through `raspi-config`
* detect i2c devices with command `sudo i2cdetect -y 1`
* install `libi2c-dev`, and add `-li2c` flag when linking
* note, `<i2c/smbus.h>` is a pure c header, so needs `extern "C"` in cpp files

# RaspberyPi I2C Pinout
* [I2C Pinout](https://pinout.xyz/pinout/i2c)

# I2C interface references
* [i2c ioctl method example](https://stackoverflow.com/questions/9974592/i2c-slave-ioctl-purpose)
* [i2c ioctl smbus method example](https://stackoverflow.com/questions/55976683/read-a-block-of-data-from-a-specific-registerfifo-using-c-c-and-i2c-in-raspb)
* [linux kernel i2c interface](https://www.kernel.org/doc/Documentation/i2c/dev-interface)
* [linux kernel smbus interface](https://manpages.debian.org/unstable/i2c-tools/libi2c.3.en.html)
* [linux kernel smbus protocol](https://www.kernel.org/doc/Documentation/i2c/smbus-protocol)
* [linux i2c smbus subsystem](https://docs.kernel.org/driver-api/i2c.html)
* [adafruit bmp3xx python lib](https://github.com/adafruit/Adafruit_CircuitPython_BMP3XX)

# Notes
* direct file I/O interface with `read`/`write`
  - for reading, `write` register address, then `read` data
  - for writing, combine register address and data into single buffer, then `write` this buffer
* `i2c_smbus_read_block_data`/`i2c_smbus_write_block_data` seems not working on raspberry pi
  - try `i2c_smbus_read_i2c_block_data`/`i2c_smbus_write_i2c_block_data` instead
* try ioctl smbus method with `I2C_SMBUS` request and `I2C_SMBUS_I2C_BLOCK_DATA` size option
