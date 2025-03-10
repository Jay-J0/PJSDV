#ifndef DOOR_H
#define DOOR_H  

#include <string>
#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include "Furniture.h"
#include "Timer.h"

using namespace std;
#pragma once

class Door : public Furniture {
  public:
  Door(const char* ipAddresses);
};

#endif 