#ifndef _COMMON_HEADER_INCLUDED_
#define _COMMON_HEADER_INCLUDED_

#include <assert.h>
#include <windows.h>

#include "GLString.h"
#include "GLException.h"
#include "GLSimpleArray.h"
#include "GLArray.h"
#include "GLArray2D.h"

#include "GLFile.h"

namespace GLC {
	typedef CGLArray<CGLString> CGLStrings;
}

#endif