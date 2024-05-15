#ifndef __COMMON_H__
#define __COMMON_H__

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>

#define EXIT_ERR(cond, msg) \
  EXIT_IF(cond, std::string{} + (msg) + ": " + strerror(errno))

#define EXIT_IF(cond, msg)                                   \
  do { if (cond) {                                           \
    std::cerr << "[" << __LINE__ << " " << __func__ << "] "; \
    std::cerr << (msg) << std::endl;                         \
    exit(EXIT_FAILURE);                                      \
  } } while (0)

#endif
