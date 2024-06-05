#ifndef __COMMON_H__
#define __COMMON_H__

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <chrono>
#include <ctime>
#include <iostream>
#include <random>
#include <string>

#define EXIT_ERR(cond, msg) \
  EXIT_IF(cond, std::string{} + (msg) + ": " + strerror(errno))

#define EXIT_IF(cond, msg)                                   \
  do { if (cond) {                                           \
    std::cerr << "[" << __LINE__ << " " << __func__ << "] "; \
    std::cerr << (msg) << std::endl;                         \
    exit(EXIT_FAILURE);                                      \
  } } while (0)

template<typename Time>
std::string get_filename(const Time& t, const char *prefix, const char *postfix) {
  const auto t_ts = std::chrono::duration_cast<std::chrono::nanoseconds>(
      t.time_since_epoch()).count();
  const std::time_t t_t = std::chrono::system_clock::to_time_t(t);
  char startTimeStr[std::size("yymmdd-HHMMSS")];
  int nbytes = std::strftime(std::data(startTimeStr), std::size(startTimeStr),
      "%y%m%d-%H%M%S", std::localtime(&t_t));
  EXIT_IF(nbytes <= 0, "failed to format time");

  constexpr int RandMag = 1000000;
  std::random_device dev{};
  std::mt19937 rng{dev()};
  std::uniform_int_distribution<std::mt19937::result_type> dist{0, RandMag - 1};
  int rand_num = RandMag + dist(rng);

  std::string fname = std::string(prefix) + startTimeStr + "_" + std::to_string(t_ts)
      + "_" + std::to_string(rand_num) + postfix;
  return fname;
}

#endif
