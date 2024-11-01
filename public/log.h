#pragma once

#include <cstdio>

#define DEBUG "dbg"
#define INFO "inf"
#define WARN "wrn"
#define ERROR "err"

#define LOG(level, msg, ...) \
  printf("[" level "]: " msg "\n", ##__VA_ARGS__)
