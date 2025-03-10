#ifndef WALL_H
#define WALL_H  

#include <string>
#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include "Furniture.h"	

using namespace std;
#pragma once

class Wall : public Furniture {
  public:
  Wall(const char* ipAddresses);
};

#endif 