#include <iostream>
#include <thread>
#include <vector>

#include "Logging/LoggerMacros.h"

int main() {
    std::cout << "Hello, World!" << std::endl;

     LOG_I("This is an info log. {}", 123);
    LOG_D("Some debug {} print", "YOLO");
    LOG_W("Some warn {} print", "YOLOLOLO");
    LOG_E("Some error {} print", "DELULU");
    LOG_C("Some critical {} print", "CRITICALLLL");
}