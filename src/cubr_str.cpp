// Copyright 2018 Nicolaus Anderson

#include "cubr_str.h"
#include <irrString.h> // From Irrlicht source (gets utf8.cpp)
#include <path.h> // From Irrlicht source
#include "cubr_ascii.h"

namespace cubr {

irr::u64 wcstrlen(const wchar_t* s) {
	irr::u64  l = 0;
	for (; s[l] != L'\0'; ++l); // or maybe: while(s[l]) ++l;
	return l;
}

irr::core::array<wchar_t>
CuStrToWcharArray( util::String in ) {
	// Copper strings are byte strings assumed to be UTF8 only.
	// Irrlicht uses multibyte, so we have to convert.
	irr::u64  s = (irr::u64)in.size();
	irr::core::array<wchar_t>  wca(s);
	wca.set_used( in.size() - (irr::u32)s );
	irr::core::utf8ToWchar(in.c_str(), wca.pointer(), s);
	// FIXME: This will leak memory if the string is copied but not deleted.
	wca.set_free_when_destroyed(false);
	return wca;
}

/*
wchar_t*
CuStrToWchar( util::String in ) {
	// Copper strings are byte strings assumed to be UTF8 only.
	// Irrlicht uses multibyte, so we have to convert.
	irr::u64  s = (irr::u64)in.size();
	irr::core::array<wchar_t>  wca(s);
	wca.set_used( in.size() - (irr::u32)s );
	irr::core::utf8ToWchar(in.c_str(), wca.pointer(), s);
	// FIXME: This will leak memory if the string is copied but not deleted.
	wca.set_free_when_destroyed(false);
	//return wca.const_pointer();
	return wca.pointer();
}*/

util::String
IrrStrWToCuStr( irr::core::stringw in ) {
	irr::u64 size = (in.size()+1)*6; // wchar_t is 2-4 bytes, utf8 is up to 6.
	char*  cs = new char[size];
	irr::core::wcharToUtf8(in.c_str(), cs, size);
	util::String  cus(cs);
	delete [] cs;
	return cus;
}

//irr::core::stringw
//CuStrToIrrStrW( util::String in ) {
//	irr::core::stringw  wcs;
//	multibyteToWString(wcs, in.c_str(), in.size()); // Depends on LC_CTYPE / locale
//	return wcs;
//}

irr::core::stringw
CuStrToIrrStrW( util::String in ) {
	irr::core::stringw  wcs;
	irr::u64  s = (irr::u64)(in.size()+1)*sizeof(wchar_t);
	wchar_t* buf = new wchar_t[s];
	irr::core::utf8ToWchar(in.c_str(), buf, s);
	wcs.append(buf);
	delete [] buf;
	return wcs;
}

util::String
wcharToCuStr( const wchar_t* wcs,  irr::u64  size ) {
	char*  cs = new char[(size+1)*4]; // wchar_t is 2-4 bytes, utf8 is up to 6.
	irr::core::wcharToUtf8(wcs, cs, (size+1)*4);
	util::String  cus(cs);
	delete [] cs;
	return cus;
}

irr::io::path
CuStrToIrrPath( util::String s ) {
	// Copper strings are read as UTF-8, whereas paths may be wchar_t, so we may have to convert.
	irr::io::path  p;
	// FIXME: For some reason, fschar_t is set to char but the path is still read as wchar_t
	//if ( sizeof(char) == sizeof(irr::fschar_t) ) {
		// fschar_t is char
		//p = utf8ToASCII(s).c_str();
	//} else {
		// Multibyte filesystem (fschar_t == wchar_t)
		p = irr::io::path(CuStrToIrrStrW(s));
	//}
	return p;
}


}
