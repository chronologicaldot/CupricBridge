// Copyright 2018 Nicolaus Anderson

#ifndef _CUBR_ASCII_H_
#define _CUBR_ASCII_H_

#include <Strings.h> // Copper

namespace cubr {

inline bool
isAscii(char c) {
	return c >= 0;
}

// Filter to Low-end ASCII (e.g. from UTF-8)
// Ignores all characters greater than 127 (standard ANSI range)
inline
util::String
filterToStdASCII( util::String in ) {
	util::CharList  out;
	util::uint  i = 0;
	for(; i < in.size(); ++i) {
		if ( isAscii(in[i]) )
			out.append(in[i]);
	}
	// Slow final conversion in exchange for fast building
	return util::String(out);
}


// Filter out all non-low-end-ASCII letters
// Removes all high-bit letters (multiple bytes)
inline
util::String
utf8ToASCII( util::String in ) {
	util::CharList  out;
	util::uint  i = 0;
	char c;
	char  count;
	while(i < in.size()) {
		if ( isAscii(in[i]) ) {
			out.append(c);
			++i;
			continue;
		}
		c = (in[i]) >> 4; // Look at high-4 bits
		count = (c & 0x01) + ((c >> 1) & 0x01) + ((c >> 2) & 0x01) + ((c >> 3) & 0x01);
		i += count; // Skip for as many as there are high-bits
	}
	// Slow final conversion in exchange for fast building
	return util::String(out);
}

}

#endif
