#include <iostream>
#include <format>
#include <emscripten.h>
using std::cout;
using std::format;

extern "C" void EMSCRIPTEN_KEEPALIVE say_hello()
{
    cout << "Hello World!\n";
    cout << format("{}\n", __cplusplus);
}