#ifndef WINDOW_H
#define WINDOW_H  

#include <string>
#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include "Furniture.h"

using namespace std;
#pragma once

class Window : public Furniture {
  private:
  bool isNight;
  uint16_t ldrValue;
  double brightness;

  public:
  Window(const char* ipAddresses);
  void setIsNight(bool state);
  bool returnIsNight();
  uint16_t returnLDRValue();
  void setBrightness();
  double returnBrightness();
};

#endif 