#include <iostream>
#include <emscripten.h>
using std::cout;

extern "C" void EMSCRIPTEN_KEEPALIVE say_hello()
{
    cout << "Hello World!\n";
}