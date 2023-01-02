#include <chrono> // Nice time lib
#include <thread> // Helper library for chrono me thinks.
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
/*
sleep_for(nanoseconds(10));
sleep_until(system_clock::now() + seconds(1));
*/
#include <stdio.h>      // printf, scanf, puts, NULL
#include <stdlib.h>     // srand, rand
#include <time.h>       // time(NULL)
#include <string> // for string class
#include <vector> // For vector class.
#include <iostream> // For cin and cout.
#include <algorithm>
#include <stdexcept>
#include "cpp-terminal/base.hpp" // Fancy colors. =]
#define T_RESET "\033[0m"

using std::string;
using std::vector;
using std::to_string;
using std::cout;
using std::cin;
using std::isdigit;
using std::stoi;

typedef unsigned int uint;
// Early functions:
// Early functions.
int RandRange(int min, int max)
{
    return (min == max ? min : (rand() % (max - min + 1)) + min);
}

bool IsNumber(const string& s)
{
    string::const_iterator it = s.begin();
    while (it != s.end() && isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

string Input(string toPrint)
{
    cout << toPrint;
    string result;
    std::getline(std::cin, result);
    return result;
}



// The settings class is so low-level that I decided to put it here.
class Settings
{
public:
    int sleepTime;

    Settings(int sleepTime) :
        sleepTime(sleepTime)
    { }

    Settings() = default;
};