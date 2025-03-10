#ifndef BED_H
#define BED_H  

#include <string>
#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include "Furniture.h"
#include "Timer.h"

using namespace std;
#pragma once

class Bed : public Furniture {

  public:
  Bed(const char* ipAddresses);
};

#endif 