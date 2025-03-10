#include "Window.h" 

Window::Window(const char* ipAddresses) : Furniture(ipAddresses) {

}

void Window::setIsNight(bool state){
    isNight = state;
  }

bool Window::returnIsNight(){
    return isNight;
}

uint16_t Window::returnLDRValue(){
  return ldrValue = returnConversionValue();
}

void Window::setBrightness(){
  brightness = (double)returnConversionValue() /1024.0;
}

double Window::returnBrightness(){
  return brightness;
}





