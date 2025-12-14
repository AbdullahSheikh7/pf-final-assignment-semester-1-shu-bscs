#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <cstdlib>
#include <sys/ioctl.h>
#include <chrono>
#include <thread>
#include <random>
#include <termios.h>
#include <unistd.h>

using namespace std;
using namespace chrono;
using namespace this_thread;

int generateRandom(int start, int end);

pair<int, int> getTerminalSize();

void wait(int ms);

void splashScreen();

string getCurrentTimestamp();

char getch();

string maskPasswordInput();

void clearScreen();

#endif
