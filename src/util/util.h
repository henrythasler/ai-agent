#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>

std::string currentTime(std::chrono::time_point<std::chrono::system_clock> now);
std::string readFile(std::string fileName);