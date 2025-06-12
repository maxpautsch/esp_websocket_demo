#pragma once
#include <stdint.h>
#include <string>

#define ONLINE true
#define OFFLINE false


struct status_t {
  bool auto_mode;
  int counter;
  int number;
  std::string state;
};

extern struct status_t system_status;

// only set in main.cpp - this way, the init code and the values can be right
#ifdef STATUS_INIT
void init_status(){
    system_status.auto_mode = true;
    system_status.counter = 0;
    system_status.number = 0;
    system_status.state = "Idle";
}

#endif 
