#ifndef _CALCULIB_H_
#define _CALCULIB_H_

#if CALCULIB

#include "CalculibKeyboard.h"
#include "Calculib3Pin.h"
#include "CalculibGraphics.h"
#include "CalculibML.h"
#include "CalculibTime.h"
#include "CalculibPrint.h"

#include <cstring>

bool calculibInit();

#else

#include "../../CasioLib/fxlib.h"
    
#endif


#endif //_CALCULIB_H_