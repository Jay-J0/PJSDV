#include "Fridge.h" 

Fridge::Fridge(const char* ipAddresses) : Furniture(ipAddresses) {

}

void Fridge::setDoorAlert(bool state){
    door_state = state;
}

bool Fridge::returnDoorAlert(){
    return door_state;
}

