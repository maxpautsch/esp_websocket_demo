#pragma once
#include <stdint.h>
#include <string>

struct status_t
{
  bool bool_mode;
  int counter;
  int number;
  std::string state;
};

extern struct status_t system_status;

// only set in main.cpp - this way, the init code and the values can be right here
#ifdef STATUS_INIT
void init_status()
{
  system_status.bool_mode = false;
  system_status.counter = 0;
  system_status.number = 0;
  system_status.state = "Idle";
}

#endif
