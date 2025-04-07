// THIS HAS BEEN COPIED FROM https://github.com/Dcraftbg/MinOS BECAUSE I'M WAY TOO LAZY TO MAKE THIS BY MYSELF
#pragma once
#include "print.h"
#include "../../../panic.h"
#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)
#define assert(x) \
   do { \
      if(!(x)) { \
         kpanic(__FILE__ ":" STRINGIFY(__LINE__) " Assertion failed: " STRINGIFY(x)); \
         for(;;) asm volatile("hlt");\
      }\
   } while(0)

#define debug_assert(x) assert(x)