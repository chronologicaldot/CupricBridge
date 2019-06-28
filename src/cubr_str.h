// Copyright 2018 Nicolaus Anderson

#ifndef _CUBR_STR_H_
#define _CUBR_STR_H_

#include <path.h> // From Irrlicht source
#include <irrArray.h>
#include <irrString.h>
#include "cubr_ascii.h"

namespace cubr {

irr::u64 wcstrlen(const wchar_t*);

irr::core::array<wchar_t>
CuStrToWcharArray( util::String in );

// Deprecated FIXME: Could result in memory leaks.
//wchar_t*
//CuStrToWchar( util::String in );

util::String
IrrStrWToCuStr( irr::core::stringw in );

irr::core::stringw
CuStrToIrrStrW( util::String in );

// Meant to be used in tandem with wcstrlen:
// ex: wcharToCuStr(value, wcstrlen(value))
util::String
wcharToCuStr( const wchar_t* wcs,  irr::u64  size );

irr::io::path
CuStrToIrrPath( util::String s );

}

#endif
