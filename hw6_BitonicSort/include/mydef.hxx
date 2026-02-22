#pragma once 

#include <iostream>

#ifndef ANALYZE
#define ANALYZE 1
#endif

#define dbgs                                                                   \
  if (!ANALYZE) {                                                              \
  } else                                                                       \
    std::cout

#define STRINGIFY(X) #X
#define TSTRINGIFY(X) STRINGIFY(X)
#define STYPE TSTRINGIFY(TYPE)