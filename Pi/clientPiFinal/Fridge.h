#ifndef FRIDGE_H
#define FRIDGE_H  

#include <string>
#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include "Furniture.h"
#include "Timer.h"

using namespace std;
#pragma once

class Fridge : public Furniture {
  private:
  uint16_t inside_temp;
  uint16_t outside_temp;
  bool  door_state;
  
  public:
  Fridge(const char* ipAddresses);
  void setDoorAlert(bool state);
  bool returnDoorAlert();
};

#endif 