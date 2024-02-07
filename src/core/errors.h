#pragma once

#ifndef NDEBUG
#include <assert.h>
#endif

#ifdef NDEBUG
#   ifdef BANJO_PEDANTIC
#      define bjExpectValue(ptr, result) if(!ptr)     {return result;}
#      define bjExpectNull(ptr, result)  if(!!ptr)    {return result;}
#      define bjExpect(cond, result)     if(!(cond))  {return result;}
#   else
#      define bjExpectValue(...)
#      define bjExpectNull(...)
#      define bjExpect(...)
#   endif
#else
#   define bjExpectValue(ptr, result) assert(ptr)
#   define bjExpectNull(ptr, result)  assert(!ptr)
#   define bjExpect(cond, result)     assert(cond)
#endif
