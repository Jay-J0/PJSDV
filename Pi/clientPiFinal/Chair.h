#ifndef CHAIR_H
#define CHAIR_H  

#include <string>
#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include "Furniture.h"

using namespace std;
#pragma once

class Chair : public Furniture {
  public:
  Chair(const char* ipAddresses);
};

#endif 