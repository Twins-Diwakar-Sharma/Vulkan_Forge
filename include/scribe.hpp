#ifndef H_Scr1bE_H
#define H_Scr1bE_H

#include <string>
#include <iostream>
#include <stdexcept>

#define DEBUG_MOD 1

#if DEBUG_MOD == 1
#define scribe(x) std::cout << x << std::endl;
#else
#define scribe(x)
#endif

#endif
