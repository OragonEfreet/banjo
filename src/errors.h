#pragma once

#define bjExpectValue(ptr, result) if(!ptr)     {return result;}
#define bjExpectNull(ptr, result)  if(!!ptr)    {return result;}
#define bjExpect(cond, result)     if(!(cond))  {return result;}
