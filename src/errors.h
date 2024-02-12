#pragma once

#ifdef BANJO_PEDANTIC
#   define bjExpectValue(ptr, result) if(!ptr)     {return result;}
#   define bjExpectNull(ptr, result)  if(!!ptr)    {return result;}
#   define bjExpect(cond, result)     if(!(cond))  {return result;}
#else
#   define bjExpectValue(...)
#   define bjExpectNull(...)
#   define bjExpect(...)
#endif
