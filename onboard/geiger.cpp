// Handling GPIO-connected switches:
//   https://kevinboone.me/pi-button-pipe.html
//   https://forums.raspberrypi.com/viewtopic.php?t=205986

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <thread>

#ifndef GEIGER_PIN
#define GEIGER_PIN 22
#endif

#define EXIT_ERR(cond, msg) \
  EXIT_IF(cond, std::string{} + (msg) + ": " + strerror(errno))

#define EXIT_IF(cond, msg)                                   \
  do { if (cond) {                                           \
    std::cerr << "[" << __LINE__ << " " << __func__ << "] "; \
    std::cerr << (msg) << std::endl;                         \
    exit(EXIT_FAILURE);                                      \
  } } while (0)

constexpr const char *const GpioDir = "/sys/class/gpio/";
constexpr const char *const PinDirection = "in";
constexpr const char *const PinEdge = "falling";
constexpr char PinActive = '0';

constexpr int GeigerPin = GEIGER_PIN;
constexpr const char *const OutputPrefix = "data/geiger_out";

void open_gpio_input(int pin) {
  // unexport pin
  std::string fGpioUnexport = std::string(GpioDir) + "unexport";
  EXIT_IF(!std::filesystem::exists(fGpioUnexport), "gpio unexport not exists");
  std::ofstream ofsGpioUnexport(fGpioUnexport, std::ios_base::binary | std::ios_base::out);
  EXIT_IF(!ofsGpioUnexport, "failed to open gpio unexport");
  ofsGpioUnexport << pin << std::flush;
  if(!ofsGpioUnexport) { std::cerr << "failed to write gpio unexport" << std::endl; }
  ofsGpioUnexport.close();

  // export pin
  std::string fGpioExport = std::string(GpioDir) + "export";
  EXIT_IF(!std::filesystem::exists(fGpioExport), "gpio export not exists");
  std::ofstream ofsGpioExport(fGpioExport, std::ios_base::binary | std::ios_base::out);
  EXIT_IF(!ofsGpioExport, "failed to open gpio export");
  ofsGpioExport << pin << std::flush;
  EXIT_IF(!ofsGpioExport, "failed to write gpio export");
  ofsGpioExport.close();
  std::string pinDir = std::string(GpioDir) + "gpio" + std::to_string(pin) + "/";

  // set pin direction
  std::string fPinDirection = pinDir + "direction";
  for (int i = 0; i < 10 && !std::filesystem::exists(fPinDirection); ++i) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  EXIT_IF(!std::filesystem::exists(fPinDirection), "pin direction not exists");
  std::ofstream ofsPinDirection(fPinDirection, std::ios_base::binary | std::ios_base::out);
  while (!ofsPinDirection) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ofsPinDirection.open(fPinDirection, std::ios_base::binary | std::ios_base::out);
  }
  EXIT_IF(!ofsPinDirection, "failed to open pin direction");
  ofsPinDirection << PinDirection << std::flush;
  EXIT_IF(!ofsPinDirection, "failed to write pin direction");
  ofsPinDirection.close();

  // set pin signal edge RISING_EDGE
  std::string fPinEdge = pinDir + "edge";
  for (int i = 0; i < 10 && !std::filesystem::exists(fPinEdge); ++i) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  EXIT_IF(!std::filesystem::exists(fPinEdge), "pin edge not exists");
  std::ofstream ofsPinEdge(fPinEdge, std::ios_base::binary | std::ios_base::out);
  while (!ofsPinEdge) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ofsPinEdge.open(fPinEdge, std::ios_base::binary | std::ios_base::out);
  }
  EXIT_IF(!ofsPinEdge, "failed to open pin edge");
  ofsPinEdge << PinEdge << std::flush;
  EXIT_IF(!ofsPinEdge, "failed to write pin edge");
  ofsPinEdge.close();
}

size_t seek_and_read(int fd, char *buf, size_t count) {
  EXIT_ERR(lseek(fd, 0, SEEK_SET) < 0, "failed to seek pin value");
  size_t total = 0;
  while (total < count) {
    ssize_t rc = read(fd, buf + total, count - total);
    if (rc == 0) { return total; }
    if (rc < 0 && errno == EINTR) { errno = 0; continue; }
    EXIT_ERR(rc < 0, "failed to read gpio value");
    total += rc;
  }
  return total;
}

void poll_gpio_input(int pin, char *buf, size_t sz, std::function<void(char *, size_t)> f) {
  open_gpio_input(pin);
  std::string pinValue = std::string(GpioDir) + "gpio" + std::to_string(pin) + "/value";
  int fd = open(pinValue.c_str(), O_RDONLY);
  EXIT_ERR(fd < 0, "failed to open pin value");

  struct pollfd pfd;
  pfd.fd = fd;
  pfd.events = POLLPRI;

  // consume any prior interrupt
  seek_and_read(fd, buf, sz);
  while (true) {
    int res = poll(&pfd, 1, -1);
    EXIT_ERR(res < 0, "failed to poll");
    if (res != 1) {  // will this happen?
      std::cerr << "unexpected poll return " << res << "\n";
      continue;
    }
    if (!(pfd.revents & POLLPRI)) {  // will this happen?
      std::cerr << "unexpected poll revents " << pfd.revents << "\n";
      continue;
    }
    if (pfd.revents != (POLLPRI | POLLERR)) {  // will this happen?
      std::cerr << "strange poll revents " << pfd.revents << "\n";
    }
    size_t cnt = seek_and_read(fd, buf, sz);
    if (cnt <= 0) {  // will this happen?
      std::cerr << "read nothing from gpio\n";
      continue;
    }
    if (buf[0] != PinActive) {
      // continue;
    }
    f(buf, cnt);
  }
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  const auto start = std::chrono::system_clock::now();
  const auto start_ts = std::chrono::duration_cast<std::chrono::nanoseconds>(
      start.time_since_epoch()).count();
  const std::time_t start_t = std::chrono::system_clock::to_time_t(start);

  char startTimeStr[std::size("yymmdd-HHMMSS")];
  int nbytes = std::strftime(std::data(startTimeStr), std::size(startTimeStr),
      "%y%m%d-%H%M%S", std::localtime(&start_t));
  EXIT_IF(nbytes <= 0, "failed to format time");
  std::string fOutput = std::string(OutputPrefix)
      + "_" + startTimeStr + "_" + std::to_string(start_ts);
  std::ofstream ofsOutput(fOutput,
      std::ios_base::binary | std::ios_base::out | std::ios_base::app);
  EXIT_IF(!ofsOutput, "failed to open " + fOutput);
  std::cout << "data file: " << fOutput << std::endl;
  ofsOutput << std::put_time(std::localtime(&start_t), "%F %T %Z") << "\n"
            << start_ts << "\n" << std::endl;
  EXIT_IF(!ofsOutput, "failed to write output");
  auto flushTime = start;

  char buffer[9];
  poll_gpio_input(GeigerPin, buffer, sizeof buffer - 1,
      [start, &flushTime, &ofsOutput] (char *buf, size_t cnt) {
        // This function consumes around 200-500ns, sometimes 1ms.
        // The time functions alone consume around 10-20us.
        buf[cnt] = 0;
        const auto now = std::chrono::system_clock::now();
        const auto lapse = now - start;
        ofsOutput << std::chrono::duration_cast<std::chrono::nanoseconds>(lapse).count()
                  << " " << buf;
        EXIT_IF(!ofsOutput, "failed to write output");
        const auto flushLapse = now - flushTime;
        if (std::chrono::duration_cast<std::chrono::milliseconds>(flushLapse).count() >= 100) {
          // flush every 100ms
          ofsOutput << std::flush;
          flushTime = now;
        }
  });

  ofsOutput.close();
  return 0;
}
