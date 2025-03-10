#ifndef LAMP_H
#define LAMP_H  

#include <string>
#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include "Furniture.h"
#include "Timer.h"

using namespace std;
#pragma once

class Lamp : public Furniture {
  private:
  Timer lampTimer;
  bool lampState;

  public:
  Lamp(const char* ipAddresses);
  void timerStart();
  void timerStop();
  void timerReset();
  unsigned int giveTime();
  bool returnState();
  void lampStateON();
  void lampStateOFF();
};

#endif 