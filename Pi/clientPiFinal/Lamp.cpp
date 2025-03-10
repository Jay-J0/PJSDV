#include "Lamp.h" 

Lamp::Lamp(const char* ipAddresses) : Furniture(ipAddresses) {

}

void Lamp::timerStart(){
    lampTimer.reset();
    lampTimer.begin();
}

void Lamp::timerStop(){
    lampTimer.stop();
}

void Lamp::timerReset(){
    lampTimer.reset();
}

bool Lamp::returnState(){
    return lampState;
}

void Lamp::lampStateON(){
    lampState = true;
}

void Lamp::lampStateOFF(){
    lampState = false;
}

unsigned int Lamp::giveTime(){
    unsigned int t = lampTimer.getTime();
    return t;
}